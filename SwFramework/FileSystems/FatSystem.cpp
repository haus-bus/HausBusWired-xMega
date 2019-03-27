/*
 * FatSystem.cpp
 *
 *  Created on: 08.04.2016
 *      Author: viktor.pankraz
 */

#include "FatSystem.h"
#include <IDiskIo.h>
#include <Time/Calender.h>

#define MAX_DRIVES 1
#define MAX_OPEN_FILES 4
#define RELATIVE_PATH 0

// Name status flags
#define NS      11    // Index of name status byte in fn[]
#define NS_LOSS   0x01  // Out of 8.3 format
#define NS_LFN    0x02  // Force to create LFN entry
#define NS_LAST   0x04  // Last segment
#define NS_BODY   0x08  // Lower case flag (body)
#define NS_EXT    0x10  // Lower case flag (ext)
#define NS_DOT    0x20  // Dot entry

// FAT sub-type boundaries
// Note that the FAT spec by Microsoft says 4085 but Windows works with 4087!
#define MIN_FAT16 4086  // Minimum number of clusters for FAT16
#define MIN_FAT32 65526 // Minimum number of clusters for FAT32

// FatFs refers the members in the FAT structures as byte array instead of
// structure member because the structure is not binary compatible between
// different platforms

#define BS_jmpBoot        0   // Jump instruction (3)
#define BS_OEMName        3   // OEM name (8)
#define BPB_BytsPerSec    11  // Sector size [byte] (2)
#define BPB_SecPerClus    13  // Cluster size [sector] (1)
#define BPB_RsvdSecCnt    14  // Size of reserved area [sector] (2)
#define BPB_NumFATs       16  // Number of FAT copies (1)
#define BPB_RootEntCnt    17  // Number of root dir entries for FAT12/16 (2)
#define BPB_TotSec16      19  // Volume size [sector] (2)
#define BPB_Media         21  // Media descriptor (1)
#define BPB_FATSz16       22  // FAT size [sector] (2)
#define BPB_SecPerTrk     24  // Track size [sector] (2)
#define BPB_NumHeads      26  // Number of heads (2)
#define BPB_HiddSec       28  // Number of special hidden sectors (4)
#define BPB_TotSec32      32  // Volume size [sector] (4)
#define BS_DrvNum         36  // Physical drive number (2)
#define BS_BootSig        38  // Extended boot signature (1)
#define BS_VolID          39  // Volume serial number (4)
#define BS_VolLab         43  // Volume label (8)
#define BS_FilSysType     54  // File system type (1)
#define BPB_FATSz32       36  // FAT size [sector] (4)
#define BPB_ExtFlags      40  // Extended flags (2)
#define BPB_FSVer         42  // File system version (2)
#define BPB_RootClus      44  // Root dir first cluster (4)
#define BPB_FSInfo        48  // Offset of FSInfo sector (2)
#define BPB_BkBootSec     50  // Offset of backup boot sectot (2)
#define BS_DrvNum32       64  // Physical drive number (2)
#define BS_BootSig32      66  // Extended boot signature (1)
#define BS_VolID32        67  // Volume serial number (4)
#define BS_VolLab32       71  // Volume label (8)
#define BS_FilSysType32   82  // File system type (1)
#define FSI_LeadSig       0   // FSI: Leading signature (4)
#define FSI_StrucSig      484 // FSI: Structure signature (4)
#define FSI_Free_Count    488 // FSI: Number of free clusters (4)
#define FSI_Nxt_Free      492 // FSI: Last allocated cluster (4)
#define MBR_Table         446 // MBR: Partition table offset (2)
#define SZ_PTE            16  // MBR: Size of a partition table entry
#define BS_55AA           510 // Boot sector signature (2)

#define DIR_Name        0   // Short file name (11)
#define DIR_Attr        11  // Attribute (1)
#define DIR_NTres       12  // NT flag (1)
#define DIR_CrtTime     14  // Created time (2)
#define DIR_CrtDate     16  // Created date (2)
#define DIR_FstClusHI   20  // Higher 16-bit of first cluster (2)
#define DIR_WrtTime     22  // Modified time (2)
#define DIR_WrtDate     24  // Modified date (2)
#define DIR_FstClusLO   26  // Lower 16-bit of first cluster (2)
#define DIR_FileSize    28  // File size (4)
#define LDIR_Ord        0   // LFN entry order and LLE flag (1)
#define LDIR_Attr       11  // LFN attribute (1)
#define LDIR_Type       12  // LFN type (1)
#define LDIR_Chksum     13  // Sum of corresponding SFN entry
#define LDIR_FstClusLO  26  // Filled by zero (0)
#define SZ_DIR          32  // Size of a directory entry
#define LLE           0x40  // Last long entry flag in LDIR_Ord
#define DDE           0xE5  // Deleted directory enrty mark in DIR_Name[0]
#define NDDE          0x05  // Replacement of a character collides with DDE

// Code page is SBCS
#define IsDBCS1( c )  0
#define IsDBCS2( c )  0

// Misc definitions
#define LD_CLUST( dir ) ( ( (uint32_t)getWordLittle( dir + DIR_FstClusHI ) << 16 ) | getWordLittle( dir + DIR_FstClusLO ) )
#define ST_CLUST( dir, cl ) { STORE_WORDL( cl, ( dir + DIR_FstClusLO ) ); STORE_WORDL( (uint32_t)cl >> 16, ( dir + DIR_FstClusHI ) ); }
#define LD2PD( vol ) ( vol )  /* Each logical drive is bound to the same physical drive number */
#define LD2PT( vol ) 0    /* Always mounts the 1st partition or in SFD */

static FatSystem::FileSystem* fileSystems[MAX_DRIVES];
static FatSystem::File::Semaphor fileSemaphors[MAX_OPEN_FILES];

static uint16_t Fsid = 0;

static bool hasFreeLockEntries()
{
   for ( uint8_t i = 0; i < MAX_OPEN_FILES; i++ )
   {
      if ( fileSemaphors[i].isFree() )
      {
         return true;
      }
   }
   return false;
}

FatSystem::FileSystem::FileSystem( IDiskIo* _disk )
{
   memset( this, 0, sizeof( FileSystem ) );
   disk = _disk;
}

FatSystem::FileSystem::~FileSystem()
{
   clearLockEntries();
}

FatSystem::Result FatSystem::FileSystem::mount( uint8_t driveNumber )
{
   // Check if the drive number is valid
   if ( ( driveNumber >= MAX_DRIVES ) || !disk )
   {
      return INVALID_DRIVE;
   }

   if ( fileSystems[driveNumber] )
   {
      delete fileSystems[driveNumber];
   }

   // Clear new file system object
   fs_type = 0;

   // Register new file system object
   fileSystems[driveNumber] = this;

   return OK;
}

uint32_t FatSystem::FileSystem::createChain( uint32_t clst )
{
   uint32_t cs, ncl, scl;
   Result res;

   if ( clst == 0 )
   { /* Create a new chain */
      scl = last_clust; /* Get suggested start point */
      if ( !scl || scl >= n_fatent )
      {
         scl = 1;
      }
   }
   else
   { /* Stretch the current chain */
      cs = getFat( clst ); /* Check the cluster status */
      if ( cs < 2 )
      {
         return 1;          /* It is an invalid cluster */
      }
      if ( cs < n_fatent )
      {
         return cs;                 /* It is already followed by next cluster */
      }
      scl = clst;
   }

   ncl = scl; /* Start cluster */
   for (;; )
   {
      ncl++; /* Next cluster */
      if ( ncl >= n_fatent )
      { /* Wrap around */
         ncl = 2;
         if ( ncl > scl )
         {
            return 0;            /* No free cluster */
         }
      }
      cs = getFat( ncl ); /* Get the cluster status */
      if ( cs == 0 )
      {
         break;           /* Found a free cluster */
      }
      if ( cs == 0xFFFFFFFF || cs == 1 )/* An error occurred */
      {
         return cs;
      }
      if ( ncl == scl )
      {
         return 0;              /* No free cluster */
      }
   }

   res = putFat( ncl, 0x0FFFFFFF ); /* Mark the new cluster "last link" */
   if ( res == OK && clst != 0 )
   {
      res = putFat( clst, ncl ); /* Link it to the previous one if needed */
   }
   if ( res == OK )
   {
      last_clust = ncl; /* Update FSINFO */
      if ( free_clust != 0xFFFFFFFF )
      {
         free_clust--;
         fsi_flag = 1;
      }
   }
   else
   {
      ncl = ( res == DISK_ERR ) ? 0xFFFFFFFF : 1;
   }

   return ncl; /* Return new cluster number or error code */
}

FatSystem::Result FatSystem::FileSystem::removeChain( uint32_t clst )
{
   Result res;
   uint32_t nxt;
#if _USE_ERASE
   uint32_t scl = clst, ecl = clst, resion[2];
#endif

   // Check range
   if ( clst < 2 || clst >= n_fatent )
   {
      res = INT_ERR;

   }
   else
   {
      res = OK;
      // Not a last link?
      while ( clst < n_fatent )
      {
         nxt = getFat( clst ); // Get cluster status
         if ( nxt == 0 )
         {
            // Empty cluster?
            break;
         }
         if ( nxt == 1 )
         {
            // Internal error
            res = INT_ERR;
            break;
         }
         if ( nxt == 0xFFFFFFFF )
         {
            // Disk error?
            res = DISK_ERR;
            break;
         }
         // Mark the cluster "empty"
         res = putFat( clst, 0 );
         if ( res != OK )
         {
            break;
         }
         if ( free_clust != 0xFFFFFFFF )
         {
            // Update FSInfo
            free_clust++;
            fsi_flag = 1;
         }
#if _USE_ERASE
         if ( ecl + 1 == nxt )
         { /* Next cluster is contiguous */
            ecl = nxt;
         }
         else
         { /* End of contiguous clusters */
            resion[0] = clust2sect( fs, scl ); /* Start sector */
            resion[1] = clust2sect( fs, ecl ) + fs->csize - 1; /* End sector */
            disk->ioctl( CTRL_ERASE_SECTOR, resion ); /* Erase the block */
            scl = ecl = nxt;
         }
#endif
         clst = nxt; /* Next cluster */
      }
   }

   return res;
}

void FatSystem::FileSystem::clearLockEntries()
{
   for ( uint8_t i = 0; i < MAX_OPEN_FILES; i++ )
   {
      fileSemaphors[i].notifyRemovedFileSystem( this );
   }
}

uint32_t FatSystem::FileSystem::getFat( uint32_t cluster )
{

   uint8_t* p;

   // Chack range
   if ( cluster < 2 || cluster >= n_fatent )
   {
      return 1;
   }

   switch ( fs_type )
   {
      case FAT12:
      {
         unsigned int bc = (unsigned int) cluster;
         bc += bc / 2;
         if ( moveWindow( fatbase + ( bc / SECTOR_SIZE ) ) )
         {
            break;
         }
         unsigned int wc = win[bc % SECTOR_SIZE];
         bc++;
         if ( moveWindow( fatbase + ( bc / SECTOR_SIZE ) ) )
         {
            break;
         }
         wc |= win[bc % SECTOR_SIZE] << 8;
         return ( cluster & 1 ) ? ( wc >> 4 ) : ( wc & 0xFFF );
      }
      case FAT16:
      {
         if ( moveWindow( fatbase + ( cluster / ( SECTOR_SIZE / 2 ) ) ) )
         {
            break;
         }
         p = &win[cluster * 2 % SECTOR_SIZE];
         return getDwordLittle( p );
      }

      case FAT32:
      {
         if ( moveWindow( fatbase + ( cluster / ( SECTOR_SIZE / 4 ) ) ) )
         {
            break;
         }
         p = &win[cluster * 4 % SECTOR_SIZE];
         return getDwordLittle( p ) & 0x0FFFFFFF;
      }
   }

   return 0xFFFFFFFF; // An error occurred at the disk I/O layer
}

FatSystem::Result FatSystem::FileSystem::putFat( uint32_t clst, uint32_t val )
{
   uint8_t bc;
   uint8_t* p;
   Result res;

   if ( clst < 2 || clst >= n_fatent )
   {
      // Check range
      res = INT_ERR;
   }
   else
   {
      switch ( fs_type )
      {
         case FAT12:
         {
            bc = clst;
            bc += bc / 2;
            res = moveWindow( fatbase + ( bc / sectorSize ) );
            if ( res != OK )
            {
               break;
            }
            p = &win[bc % sectorSize];
            *p = ( clst & 1 ) ? ( ( *p & 0x0F ) | ( (uint8_t) val << 4 ) ) : (uint8_t) val;
            bc++;
            wflag = 1;
            res = moveWindow( fatbase + ( bc / sectorSize ) );
            if ( res != OK )
            {
               break;
            }
            p = &win[bc % sectorSize];
            *p
               = ( clst & 1 ) ?
                 (uint8_t) ( val >> 4 ) :
                 ( ( *p & 0xF0 ) | ( (uint8_t) ( val >> 8 ) & 0x0F ) );
            break;
         }
         case FAT16:
         {
            res = moveWindow( fatbase + ( clst / ( sectorSize / 2 ) ) );
            if ( res != OK )
            {
               break;
            }
            p = &win[clst * 2 % sectorSize];
            STORE_WORDL( (uint16_t ) val, p );
            break;
         }
         case FAT32:
         {
            res = moveWindow( fatbase + ( clst / ( sectorSize / 4 ) ) );
            if ( res != OK )
            {
               break;
            }
            p = &win[clst * 4 % sectorSize];
            val |= getDwordLittle( p ) & 0xF0000000;
            storeDwordLittle( val, p );
            break;
         }
         default:
            res = INT_ERR;
      }
      wflag = 1;
   }

   return res;
}
FatSystem::Result FatSystem::FileSystem::moveWindow( uint32_t sector )
{
   uint32_t wsect = winsect;
   if ( wsect != sector )
   {
      // Changed current window
      if ( wflag )
      {
         // Write back dirty window if needed
         if ( disk->write( win, wsect, 1 ) != IDiskIo::OK )
         {
            return DISK_ERR;
         }
         wflag = 0;
         if ( wsect < ( fatbase + fsize ) )
         {
            // In FAT area
            uint8_t nf;
            for ( nf = n_fats; nf > 1; nf-- )
            {
               // Reflect the change to all FAT copies
               wsect += fsize;
               disk->write( win, wsect, 1 );
            }
         }
      }
      if ( sector )
      {
         if ( disk->read( win, sector, 1 ) != IDiskIo::OK )
         {
            return DISK_ERR;
         }
         winsect = sector;
      }
   }

   return OK;
}

uint32_t FatSystem::FileSystem::clust2sect( uint32_t clst )
{
   clst -= 2;
   if ( clst >= ( n_fatent - 2 ) )
   {
      return 0;                           /* Invalid cluster# */
   }
   return clst * csize + database;
}

FatSystem::Result FatSystem::FileSystem::validate( uint16_t id )
{
   if ( !this || !fs_type || this->id != id )
   {
      return INVALID_OBJECT;
   }

   if ( !disk->getStatus().isInitialized() )
   {
      return NOT_READY;
   }

   return OK;
}

FatSystem::Result FatSystem::FileSystem::sync()
{
   Result res = moveWindow( 0 );
   if ( res == OK )
   {
      // Update FSInfo sector if needed
      if ( fs_type == FAT32 && fsi_flag )
      {
         winsect = 0;

         // Create FSInfo structure
         memset( win, 0, 512 );
         storeWordLittle( 0xAA55, win + BS_55AA );
         storeDwordLittle( 0x41615252, win + FSI_LeadSig );
         storeDwordLittle( 0x61417272, win + FSI_StrucSig );
         storeDwordLittle( free_clust, win + FSI_Free_Count );
         storeDwordLittle( last_clust, win + FSI_Nxt_Free );

         // Write it into the FSInfo sector
         disk->write( win, fsi_sector, 1 );
         fsi_flag = 0;
      }
      // Make sure that no pending write process in the physical drive
      if ( disk->ioctl( IDiskIo::CTRL_SYNC, 0 ) != IDiskIo::OK )
      {
         res = DISK_ERR;
      }
   }

   return res;
}

FatSystem::Result FatSystem::File::sync()
{
   Result res;
   uint32_t tim;
   uint8_t* dir;

   res = fs->validate( id ); // Check validity of the object
   if ( res == OK )
   {
      if ( flag & FA__WRITTEN )
      { // Has the file been written?
        // Write-back dirty buffer
         if ( flag & FA__DIRTY )
         {
            if ( fs->disk->write( buf, dsect, 1 ) != IDiskIo::OK )
            {
               return DISK_ERR;
            }
         }
         flag &= ~FA__DIRTY;

         // Update the directory entry
         res = fs->moveWindow( dir_sect );
         if ( res == OK )
         {
            dir = dir_ptr;
            dir[DIR_Attr] |= Directory::ARC; // Set archive bit
            storeDwordLittle( fsize, dir + DIR_FileSize ); // Update file size
            ST_CLUST( dir, sclust ); // Update start cluster
            tim = Calender::now.toFatSystemTimestamp(); // Update updated time
            storeDwordLittle( tim, dir + DIR_WrtTime );
            flag &= ~FA__WRITTEN;
            fs->wflag = 1;
            res = fs->sync();
         }
      }
   }
   return res;
}

FatSystem::Result FatSystem::File::close()
{
   Result res = sync();

   if ( res == OK )
   {
      if ( !lockid || ( lockid > MAX_OPEN_FILES ) )
      {
         res = INT_ERR;
      }
      else
      {
         fileSemaphors[lockid - 1].decrement();
      }
   }
   if ( res == OK )
   {
      // Discard file object
      fname[0] = 0;
      fs = 0;
   }
   return res;
}

FatSystem::Result FatSystem::File::open( const char* path, uint8_t mode )
{
   Directory dj;
   uint8_t* dir;

// Clear file object
   fs = NULL;

   mode &= FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS
           | FA_CREATE_NEW;
   Result res = isMounted( &path, &dj.fs, mode & ~FA_READ );

   dj.fn = (uint8_t*)fname;
   if ( res == OK )
   {
      res = dj.followPath( path ); // Follow the file path
   }

   dir = dj.dir;

   if ( res == OK )
   {
      if ( !dir ) // Current dir itself
      {
         res = INVALID_NAME;
      }
      else
      {
         res = dj.checkLock( ( mode & ~FA_READ ) ? 1 : 0 );
      }
   }

// Create or Open a file
   if ( mode & ( FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW ) )
   {
      uint32_t dw, cl;

      if ( res != OK )
      { // No file, create new
         if ( res == NO_FILE ) // There is no file to open, create a new entry
         {
            res = hasFreeLockEntries() ? dj.registerObject() : TOO_MANY_OPEN_FILES;
         }

         mode |= FA_CREATE_ALWAYS; // File is created
         dir = dj.dir; // New entry
      }
      else
      { // Any object is already existing
         if ( dir[DIR_Attr] & ( Directory::RDO | Directory::DIR ) )
         { // Cannot overwrite it (R/O or DIR)
            res = DENIED;
         }
         else
         {
            if ( mode & FA_CREATE_NEW ) // Cannot create as new file
            {
               res = EXIST;
            }
         }
      }
      // Truncate it if overwrite mode
      if ( res == OK && ( mode & FA_CREATE_ALWAYS ) )
      {
         // Created time
         dw = Calender::now.toFatSystemTimestamp();
         storeDwordLittle( dw, ( dir + DIR_CrtTime ) );

         // Reset attribute
         dir[DIR_Attr] = 0;
         storeDwordLittle( 0, ( dir + DIR_FileSize ) );

         // Get start cluster
         cl = LD_CLUST( dir );
         ST_CLUST( dir, 0 );
         dj.fs->wflag = 1;

         // Remove the cluster chain if exist
         if ( cl )
         {
            dw = dj.fs->winsect;
            res = dj.fs->removeChain( cl );
            if ( res == OK )
            {
               // Reuse the cluster hole
               dj.fs->last_clust = cl - 1;
               res = dj.fs->moveWindow( dw );
            }
         }
      }
   }
   else
   { // Open an existing file
      if ( res == OK )
      { // Follow succeeded
         if ( dir[DIR_Attr] & Directory::DIR )
         { // It is a directory
            res = NO_FILE;
         }
         else
         {
            if ( ( mode & FA_WRITE ) && ( dir[DIR_Attr] & Directory::RDO ) ) // R/O violation
            {
               res = DENIED;
            }
         }
      }
   }
   if ( res == OK )
   {
      if ( mode & FA_CREATE_ALWAYS ) // Set file change flag if created or overwritten
      {
         mode |= FA__WRITTEN;
      }
      dir_sect = dj.fs->winsect; // Pointer to the directory entry
      dir_ptr = dir;
      lockid = dj.lock( ( mode & ~FA_READ ) ? 1 : 0 );
      if ( !lockid )
      {
         res = INT_ERR;
      }
   }

   if ( res == OK )
   {
      flag = mode; // File access mode
      sclust = LD_CLUST( dir ); // File start cluster
      fsize = getDwordLittle( dir + DIR_FileSize ); // File size
      fptr = 0; // File pointer
      dsect = 0;
      fs = dj.fs;
      id = dj.fs->id; // Validate file object
   }

   return res;
}

FatSystem::Result FatSystem::File::write( const void* buff, uint16_t btw,
                                          uint16_t& bytesWritten )
{
   Result res;
   uint32_t clst, sect;
   uint16_t wcnt, cc;
   const uint8_t* wbuff = reinterpret_cast<const uint8_t*>( buff );
   uint8_t csect;

   bytesWritten = 0; /* Initialize byte counter */

   res = fs->validate( id ); /* Check validity */
   if ( res != OK )
   {
      return res;
   }
   if ( flag & FA__ERROR ) /* Aborted file? */
   {
      return INT_ERR;
   }
   // Check access mode
   if ( !( flag & FA_WRITE ) )
   {
      return DENIED;
   }
   if ( (uint32_t) ( fsize + btw ) < fsize )
   {
      // FILE SIZE CANNOT REACH 4GB
      btw = 0;
   }

   // Repeat until all data written
   for (; btw; wbuff += wcnt, fptr += wcnt, bytesWritten += wcnt, btw -= wcnt )
   {
      if ( ( fptr % fs->sectorSize ) == 0 )
      { /* On the sector boundary? */
         csect = (uint8_t) ( fptr / fs->sectorSize & ( fs->csize - 1 ) ); // Sector offset in the cluster
         if ( !csect )
         { // On the cluster boundary?
            if ( fptr == 0 )
            { /* On the top of the file? */
               clst = sclust; /* Follow from the origin */
               if ( clst == 0 ) /* When no cluster is allocated, */
               {
                  sclust = clst = fs->createChain( 0 ); /* Create a new cluster chain */
               }
            }
            else
            { /* Middle or end of the file */
               clst = fs->createChain( clust ); /* Follow or stretch cluster chain on the FAT */
            }
            if ( clst == 0 )
            {
               // Could not allocate a new cluster (disk full)
               break;
            }
            if ( clst == 1 )
            {
               flag |= FA__ERROR;
               return INT_ERR;
            }
            if ( clst == 0xFFFFFFFF )
            {
               flag |= FA__ERROR;
               return DISK_ERR;
            }
            clust = clst; /* Update current cluster */
         }
         if ( flag & FA__DIRTY )
         { /* Write-back sector cache */
            if ( fs->disk->write( buf, dsect, 1 ) != IDiskIo::OK )
            {
               flag |= FA__ERROR;
               return DISK_ERR;
            }
            flag &= ~FA__DIRTY;
         }
         sect = fs->clust2sect( clust ); /* Get current sector */
         if ( !sect )
         {
            flag |= FA__ERROR;
            return INT_ERR;
         }
         sect += csect;
         cc = btw / fs->sectorSize; /* When remaining bytes >= sector size, */
         if ( cc )
         { /* Write maximum contiguous sectors directly */
            if ( csect + cc > fs->csize ) /* Clip at cluster boundary */
            {
               cc = fs->csize - csect;
            }
            if ( fs->disk->write( wbuff, sect, (uint8_t) cc ) != IDiskIo::OK )
            {
               flag |= FA__ERROR;
               return DISK_ERR;
            }
            if ( dsect - sect < cc )
            { /* Refill sector cache if it gets invalidated by the direct write */
               memcpy( buf, wbuff + ( ( dsect - sect ) * fs->sectorSize ),
                       fs->sectorSize );
               flag &= ~FA__DIRTY;
            }
            wcnt = fs->sectorSize * cc; /* Number of bytes transferred */
            continue;
         }
         if ( dsect != sect )
         { /* Fill sector cache with file data */
            if ( fptr < fsize && fs->disk->read( buf, sect, 1 ) != IDiskIo::OK )
            {
               flag |= FA__ERROR;
               return DISK_ERR;
            }
         }

         dsect = sect;
      }
      wcnt = fs->sectorSize - ( fptr % fs->sectorSize );/* Put partial sector into file I/O buffer */
      if ( wcnt > btw )
      {
         wcnt = btw;
      }
      memcpy( &buf[fptr % fs->sectorSize], wbuff, wcnt ); /* Fit partial sector */
      flag |= FA__DIRTY;
   }

   if ( fptr > fsize )
   {
      fsize = fptr; /* Update file size if needed */
   }
   flag |= FA__WRITTEN; /* Set file change flag */

   return OK;
}

FatSystem::Result FatSystem::File::seek( uint32_t ofs )
{
   Result res;

   // Check validity of the object
   res = fs->validate( id );
   if ( res != OK )
   {
      return res;
   }
   if ( flag & FA__ERROR )
   {
      // Check abort flag
      return INT_ERR;
   }

   // Normal Seek
   {
      uint32_t clst, bcs, nsect, ifptr;
      // In read-only mode, clip offset with the file size */
      if ( ( ofs > fsize ) && !( flag & FA_WRITE ) )
      {
         ofs = fsize;
      }

      ifptr = fptr;
      fptr = nsect = 0;
      if ( ofs )
      {
         bcs = (uint32_t) fs->csize * fs->sectorSize; // Cluster size (byte)
         if ( ifptr > 0 && ( ofs - 1 ) / bcs >= ( ifptr - 1 ) / bcs )
         {
            // When seek to same or following cluster,
            fptr = ( ifptr - 1 ) & ~( bcs - 1 ); // start from the current cluster
            ofs -= fptr;
            clst = clust;
         }
         else
         { /* When seek to back cluster, */
            clst = sclust; /* start from the first cluster */
            if ( clst == 0 )
            { /* If no cluster chain, create a new chain */
               clst = fs->createChain( 0 );
               if ( clst == 1 )
               {
                  flag |= FA__ERROR;
                  return INT_ERR;
               }
               if ( clst == 0xFFFFFFFF )
               {
                  flag |= FA__ERROR;
                  return DISK_ERR;
               }
               sclust = clst;
            }
            clust = clst;
         }
         if ( clst != 0 )
         {
            while ( ofs > bcs )
            {
               // Cluster following loop
               if ( flag & FA_WRITE )
               {
                  // Check if in write mode or not
                  // Force stretch if in write mode
                  clst = fs->createChain( clst );
                  if ( clst == 0 )
                  {
                     // When disk gets full, clip file size
                     ofs = bcs;
                     break;
                  }
               }
               else
               {
                  // Follow cluster chain if not in write mode
                  clst = fs->getFat( clst );
               }
               if ( clst == 0xFFFFFFFF )
               {
                  flag |= FA__ERROR;
                  return DISK_ERR;
               }
               if ( clst <= 1 || clst >= fs->n_fatent )
               {
                  flag |= FA__ERROR;
                  return INT_ERR;
               }
               clust = clst;
               fptr += bcs;
               ofs -= bcs;
            }
            fptr += ofs;
            if ( ofs % fs->sectorSize )
            {
               nsect = fs->clust2sect( clst ); /* Current sector */
               if ( !nsect )
               {
                  flag |= FA__ERROR;
                  return INT_ERR;
               }
               nsect += ofs / fs->sectorSize;
            }
         }
      }
      if ( ( fptr % fs->sectorSize ) && nsect != dsect )
      {
         // Fill sector cache if needed
         if ( flag & FA__DIRTY )
         {
            // Write-back dirty sector cache
            if ( fs->disk->write( buf, dsect, 1 ) != IDiskIo::OK )
            {
               flag |= FA__ERROR;
               return DISK_ERR;
            }
            flag &= ~FA__DIRTY;
         }
         if ( fs->disk->read( buf, nsect, 1 ) != IDiskIo::OK )
         {
            // Fill sector cache
            flag |= FA__ERROR;
            return DISK_ERR;
         }
         dsect = nsect;
      }
      if ( fptr > fsize )
      {
         // Set file change flag if the file size is extended
         fsize = fptr;
         flag |= FA__WRITTEN;
      }
   }

   return res;
}

FatSystem::Result FatSystem::Directory::checkLock( uint8_t accessMode )
{
   uint8_t i, be;

// Search file semaphore table
   for ( i = be = 0; i < MAX_OPEN_FILES; i++ )
   {
      if ( fileSemaphors[i].fs )
      {
         // Check if the file matched with an open file
         if ( ( fileSemaphors[i].fs == fs ) && ( fileSemaphors[i].clu == sclust )
            && fileSemaphors[i].idx == index )
         {
            break;
         }
      }
      else
      {
         // Blank entry
         be++;
      }
   }
   if ( i == MAX_OPEN_FILES )
   {
      // The file is not opened
      return ( be || accessMode == 2 ) ? OK : TOO_MANY_OPEN_FILES;
   }

// The file has been opened. Reject any open against writing file and all write mode open
   return ( accessMode || fileSemaphors[i].ctr == 0x100 ) ? LOCKED : OK;
}

uint8_t FatSystem::Directory::lock( uint8_t accessMode )
{
   uint8_t i;

// Find the file
   for ( i = 0; i < MAX_OPEN_FILES; i++ )
   {
      if ( fileSemaphors[i].fs == fs && fileSemaphors[i].clu == sclust
         && fileSemaphors[i].idx == index )
      {
         break;
      }
   }

   if ( i == MAX_OPEN_FILES )
   {
      // Not opened. Register it as new.
      for ( i = 0; i < MAX_OPEN_FILES && fileSemaphors[i].fs; i++ )
      {
      }
      if ( i == MAX_OPEN_FILES )
      {
         // No space to register (int err)
         return 0;
      }
      fileSemaphors[i].fs = fs;
      fileSemaphors[i].clu = sclust;
      fileSemaphors[i].idx = index;
      fileSemaphors[i].ctr = 0;
   }

   if ( accessMode && fileSemaphors[i].ctr )
   {
      // Access violation (int err)
      return 0;
   }

   if ( accessMode )
   {
      fileSemaphors[i].ctr = 0x100;
   }
   else
   {
      // Set semaphore value
      fileSemaphors[i].ctr += 1;
   }
   return i + 1;
}

FatSystem::Result FatSystem::Directory::next( bool stretch )
{
   uint32_t clst;
   uint16_t i;

   i = index + 1;
   if ( !i || !sect )
   {
      // Report EOT when index has reached 65535 or sector is 0
      return NO_FILE;
   }

   if ( !( i % ( fs->sectorSize / SZ_DIR ) ) )
   {
      // Sector changed go to next sector
      sect++;

      if ( clust == 0 )
      {
         // it is a static table
         if ( i >= fs->n_rootdir )
         {
            // Report EOT when end of table
            return NO_FILE;
         }

      }
      else
      {
         // Dynamic table
         if ( ( ( i / ( fs->sectorSize / SZ_DIR ) ) & ( fs->csize - 1 ) ) == 0 )
         {
            // Cluster changed, get next cluster
            clst = fs->getFat( clust );
            if ( clst <= 1 )
            {
               return INT_ERR;
            }
            if ( clst == 0xFFFFFFFF )
            {
               return DISK_ERR;
            }
            if ( clst >= fs->n_fatent )
            {
               // When it reached end of dynamic table

               uint8_t c;
               if ( !stretch )
               {
                  // When do not stretch, report EOT
                  return NO_FILE;
               }

               clst = fs->createChain( clust );
               if ( clst == 0 )
               {
                  return DENIED;          /* No free cluster */
               }
               if ( clst == 1 )
               {
                  return INT_ERR;
               }
               if ( clst == 0xFFFFFFFF )
               {
                  return DISK_ERR;
               }
               /* Clean-up stretched table */
               if ( fs->moveWindow( 0 ) )
               {
                  return DISK_ERR;                    /* Flush active window */
               }
               memset( fs->win, 0, fs->sectorSize ); /* Clear window buffer */
               fs->winsect = fs->clust2sect( clst ); /* Cluster start sector */
               for ( c = 0; c < fs->csize; c++ )
               {
                  // Fill the new cluster with 0
                  fs->wflag = 1;
                  if ( fs->moveWindow( 0 ) )
                  {
                     return DISK_ERR;
                  }
                  fs->winsect++;
               }
               fs->winsect -= c; /* Rewind window address */
            }
            clust = clst; /* Initialize data for new cluster */
            sect = fs->clust2sect( clst );
         }
      }
   }

   index = i;
   dir = fs->win + ( i % ( fs->sectorSize / SZ_DIR ) ) * SZ_DIR;

   return OK;
}

FatSystem::Result FatSystem::Directory::registerObject()
{
   Result res;
   uint8_t c, * dir;
   res = setIndex( 0 );
   if ( res == OK )
   {
      do
      { // Find a blank entry for the SFN
         res = fs->moveWindow( sect );
         if ( res != OK )
         {
            break;
         }
         c = *this->dir;

         if ( c == DDE || c == 0 )
         {
            // Is it a blank entry
            break;
         }

         // Next entry with table stretch
         res = next( true );
      }
      while ( res == OK );
   }

   if ( res == OK )
   {
      // Initialize the SFN entry
      res = fs->moveWindow( sect );
      if ( res == OK )
      {
         dir = this->dir;
         memset( dir, 0, SZ_DIR ); // Clean the entry
         memcpy( dir, fn, 11 ); // Put SFN
         fs->wflag = 1;
      }
   }

   return res;
}

FatSystem::Result FatSystem::Directory::createName( const char** path )
{

   uint8_t b, c, d, * sfn;
   uint8_t ni, si, i;
   const char* p;

// Create file name in directory form
   for ( p = *path; *p == '/' || *p == '\\'; p++ )
   {
      // Strip duplicated separator
   }
   sfn = fn;
   memset( sfn, ' ', 11 );
   si = i = b = 0;
   ni = 8;

   for (;; )
   {
      c = (uint8_t) p[si++];
      if ( c <= ' ' || c == '/' || c == '\\' )
      {
         // Break on end of segment
         break;
      }
      if ( c == '.' || i >= ni )
      {
         if ( ni != 8 || c != '.' )
         {
            return INVALID_NAME;
         }
         i = 8;
         ni = 11;
         b <<= 2;
         continue;
      }
      if ( c >= 0x80 )
      { /* Extended char? */
         b |= 3; /* Eliminate NT flag */
         return INVALID_NAME;
      }
      if ( IsDBCS1( c ) )
      { /* Check if it is a DBC 1st byte (always false on SBCS cfg) */
         d = (uint8_t) p[si++]; /* Get 2nd byte */
         if ( !IsDBCS2( d ) || i >= ni - 1 ) /* Reject invalid DBC */
         {
            return INVALID_NAME;
         }
         sfn[i++] = c;
         sfn[i++] = d;
      }
      else
      { // Single byte code
         if ( strchr( "\"*+,:;<=>\?[]|\x7F", c ) ) // Reject illegal chrs for SFN
         {
            return INVALID_NAME;
         }
         if ( IsUpper( c ) )
         { /* ASCII large capital? */
            b |= 2;
         }
         else
         {
            if ( IsLower( c ) )
            { /* ASCII small capital? */
               b |= 1;
               c -= 0x20;
            }
         }
         sfn[i++] = c;
      }
   }
   *path = &p[si]; /* Return pointer to the next segment */
   c = ( c <= ' ' ) ? NS_LAST : 0; /* Set last segment flag if end of path */

   if ( !i )
   {
      return INVALID_NAME;       /* Reject nul string */
   }
   if ( sfn[0] == DDE )
   {
      sfn[0] = NDDE;                  /* When first char collides with DDE, replace it with 0x05 */

   }
   if ( ni == 8 )
   {
      b <<= 2;
   }
   if ( ( b & 0x03 ) == 0x01 )
   {
      c |= NS_EXT;                       /* NT flag (Name extension has only small capital) */
   }
   if ( ( b & 0x0C ) == 0x04 )
   {
      c |= NS_BODY;                       /* NT flag (Name body has only small capital) */

   }
   sfn[NS] = c; /* Store NT flag, File name is created */

   return OK;
}

FatSystem::Result FatSystem::Directory::find()
{
   Result res;
   uint8_t c, * dir;

// Rewind directory object
   res = setIndex( 0 );
   if ( res != OK )
   {
      return res;
   }

   do
   {
      res = fs->moveWindow( sect );
      if ( res != OK )
      {
         break;
      }
      dir = this->dir; /* Ptr to the directory entry of current index */
      c = dir[DIR_Name];
      if ( c == 0 )
      {
         res = NO_FILE;
         break;
      } /* Reached to end of table */
      if ( !( dir[DIR_Attr] & VOL ) && !memcmp( dir, fn, 11 ) ) /* Is it a valid entry? */
      {
         break;
      }
      res = next( 0 ); /* Next entry */
   }
   while ( res == OK );

   return res;
}

FatSystem::Result FatSystem::Directory::create( const char* path )
{
   Result res;
   Directory dj;
   uint8_t* dir, n;
   uint32_t dsc, dcl, pcl, tim = Calender::now.toFatSystemTimestamp();
   uint8_t sfn[12];

   res = isMounted( &path, &dj.fs, 0 );
   if ( res == OK )
   {
      dj.fn = sfn;
      res = dj.followPath( path ); // Follow the file path
      if ( res == OK )
      {
         res = EXIST; // Any object with same name is already existing
      }
      if ( RELATIVE_PATH && res == NO_FILE && ( dj.fn[NS] & NS_DOT ) )
      {
         res = INVALID_NAME;
      }

      if ( res == NO_FILE )
      {
         // Can create a new directory
         // Allocate a cluster for the new directory table
         dcl = dj.fs->createChain( 0 );
         res = OK;
         if ( dcl == 0 )
         {
            // No space to allocate a new cluster
            res = DENIED;
         }
         if ( dcl == 1 )
         {
            res = INT_ERR;
         }
         if ( dcl == 0xFFFFFFFF )
         {
            res = DISK_ERR;
         }
         if ( res == OK )
         {
            // Flush FAT
            res = dj.fs->moveWindow( 0 );
         }
         if ( res == OK )
         {
            // Initialize the new directory table
            dsc = dj.fs->clust2sect( dcl );
            dir = dj.fs->win;
            memset( dir, 0, dj.fs->sectorSize );
            memset( dir + DIR_Name, ' ', 8 + 3 ); // Create "." entry
            dir[DIR_Name] = '.';
            dir[DIR_Attr] = DIR;
            storeDwordLittle( tim, dir + DIR_WrtTime );
            ST_CLUST( dir, dcl );
            memcpy( dir + SZ_DIR, dir, SZ_DIR ); // Create ".." entry
            dir[33] = '.';
            pcl = dj.sclust;
            if ( dj.fs->fs_type == FileSystem::FAT32 && pcl == dj.fs->dirbase )
            {
               pcl = 0;
            }

            ST_CLUST( dir + SZ_DIR, pcl );
            for ( n = dj.fs->csize; n; n-- )
            {
               // Write dot entries and clear following sectors
               dj.fs->winsect = dsc++;
               dj.fs->wflag = 1;
               res = dj.fs->moveWindow( 0 );
               if ( res != OK )
               {
                  break;
               }
               memset( dir, 0, dj.fs->sectorSize );
            }
         }

         if ( res == OK )
         {
            // Register the object to the directory
            res = dj.registerObject();
         }
         if ( res != OK )
         {
            // Could not register, remove cluster chain
            dj.fs->removeChain( dcl );
         }
         else
         {
            dir = dj.dir;
            dir[DIR_Attr] = DIR; // Attribute
            storeDwordLittle( tim, dir + DIR_WrtTime ); // Created time
            ST_CLUST( dir, dcl ); // Table start cluster
            dj.fs->wflag = 1;
            res = dj.fs->sync();
         }
      }
   }

   return res;
}

FatSystem::Result FatSystem::Directory::open( const char* path )
{
   Result res;
   uint8_t sfn[12];

   res = isMounted( &path, &fs, 0 );
   if ( res == OK )
   {
      fn = sfn;
      res = followPath( path ); /* Follow the path to the directory */
      if ( res == OK )
      { /* Follow completed */
         if ( dir )
         { /* It is not the root dir */
            if ( dir[DIR_Attr] & DIR )
            { /* The object is a directory */
               sclust = LD_CLUST( dir );
            }
            else
            { /* The object is not a directory */
               res = NO_PATH;
            }
         }
         if ( res == OK )
         {
            id = fs->id;
            res = setIndex( 0 ); /* Rewind dir */
         }
      }
      if ( res == NO_FILE )
      {
         res = NO_PATH;
      }
   }

   return res;
}

FatSystem::Result FatSystem::Directory::followPath( const char* path )
{
   Result res;

// Strip heading separator if exist
   if ( *path == '/' || *path == '\\' )
   {
      path++;
   }

// Start from the root dir
   sclust = 0;

   if ( (uint8_t) *path < ' ' )
   {
      // Null path means the start directory itself
      res = setIndex( 0 );
      dir = 0;

   }
   else
   {
      // Follow path
      for (;; )
      {
         res = createName( &path );
         if ( res != OK )
         {
            break;
         }
         res = find();
         uint8_t ns = *( fn + NS );
         if ( res != OK )
         {
            // Could not find the object
            if ( !( ns & NS_LAST ) )
            {
               res = NO_PATH;
            }
            break;
         }
         if ( ns & NS_LAST )
         {
            // Last segment match. Function completed.
            break;
         }

         // There is next segment. Follow the sub directory
         if ( !( dir[DIR_Attr] & Directory::DIR ) )
         {
            // Cannot follow because it is a file
            res = NO_PATH;
            break;
         }
         sclust = LD_CLUST( dir );
      }
   }

   return res;
}

FatSystem::Result FatSystem::Directory::setIndex( uint16_t idx )
{
   uint32_t clst = sclust;
   index = idx;

// Check start cluster range
   if ( clst == 1 || clst >= fs->n_fatent )
   {
      return INT_ERR;
   }

// Replace cluster# 0 with root cluster# if in FAT32
   if ( !clst && fs->fs_type == FileSystem::FAT32 )
   {
      clst = fs->dirbase;
   }

// Static table (root-dir in FAT12/16)
   if ( clst == 0 )
   {
      clust = clst;
      if ( idx >= fs->n_rootdir )
      {
         // Index is out of range
         return INT_ERR;
      }

      sect = fs->dirbase + idx / ( SECTOR_SIZE / SZ_DIR ); // Sector#
   }
   else
   {
      // Dynamic table (sub-dirs or root-dir in FAT32)
      uint16_t ic = SECTOR_SIZE / SZ_DIR * fs->csize; // Entries per cluster

      while ( idx >= ic )
      {
         // Follow cluster chain
         clst = fs->getFat( clst );    // Get next cluster
         if ( clst == 0xFFFFFFFF )
         {
            return DISK_ERR;
         }
         if ( clst < 2 || clst >= fs->n_fatent )
         {
            // Reached to end of table or int error
            return INT_ERR;
         }

         idx -= ic;
      }
      clust = clst;
      sect = fs->convertToSector( clst ) + idx / ( SECTOR_SIZE / SZ_DIR );
   }

// Ptr to the entry in the sector
   dir = fs->win + ( idx % ( SECTOR_SIZE / SZ_DIR ) ) * SZ_DIR;

   return OK;
}

FatSystem::Result FatSystem::isMounted( const char** pPath,
                                        FileSystem** pFileSystem,
                                        bool checkWriteProtection )
{
   uint8_t fmt, b, pi, * tbl;
   uint8_t vol;
   IDiskIo::Status stat;
   uint32_t bsect, fasize, tsect, sysect, nclst, szbfat;
   uint16_t nrsv;
   const char* p = *pPath;
   FileSystem* fs;

// Get logical drive number from the path name
   vol = p[0] - '0'; // Is there a drive number?
   if ( vol <= 9 && p[1] == ':' )
   { // Found a drive number, get and strip it
      p += 2;
      *pPath = p; // Return pointer to the path name
   }
   else
   {
      // No drive number is given, use drive 0
      vol = 0;
   }

// Check if the file system object is valid or not
   if ( vol >= MAX_DRIVES ) // Is the drive number valid?
   {
      return INVALID_DRIVE;
   }

   *pFileSystem = fs = fileSystems[vol]; /// Return pointer to the corresponding file system object
   if ( !fs ) // Is the file system object available?
   {
      return NOT_ENABLED;
   }

   fs->lock();

   if ( fs->fs_type )
   { // If the logical drive has been mounted
      stat = fs->disk->getStatus();
      if ( stat.isInitialized() )
      { // and the physical drive is kept initialized (has not been changed),
         if ( checkWriteProtection && stat.isProtected() ) // Check write protection if needed
         {
            return WRITE_PROTECTED;
         }
         return OK; // The file system object is valid
      }
   }

// The file system object is not valid.
// Following code attempts to mount the volume. (analyze BPB and initialize the fs object)

   fs->fs_type = 0; // Clear the file system object
   stat = fs->disk->init(); // Initialize low level disk I/O layer

// Check if the initialization succeeded
   if ( !stat.isInitialized() )
   {
      // Failed to initialize due to no media or hard error
      return NOT_READY;
   }

// Check disk write protection if needed
   if ( checkWriteProtection && stat.isProtected() )
   {
      return WRITE_PROTECTED;
   }

   if ( fs->disk->ioctl( IDiskIo::GET_SECTOR_SIZE, &fs->sectorSize )
        != IDiskIo::OK )
   {
      return DISK_ERR;
   }

// Search FAT partition on the drive. Supports only generic partitionings, FDISK and SFD.
// Load sector 0 and check if it is an FAT-VBR (in SFD)
   fmt = fs->check( bsect = 0 );
   if ( LD2PT( vol ) && !fmt )
   {
      // Force non-SFD if the volume is forced partition
      fmt = 1;
   }
   if ( fmt == 1 )
   {
      // Not an FAT-VBR, the physical drive can be partitioned
      // Check the partition listed in the partition table
      pi = LD2PT( vol );
      if ( pi )
      {
         pi--;
      }
      tbl = &fs->win[MBR_Table + pi * SZ_PTE];  // Partition table
      if ( tbl[4] )
      { // Is the partition existing?
         bsect = *(uint32_t*) &tbl[8]; // Partition offset in LBA
         fmt = fs->check( bsect ); // Check the partition
      }
   }
   if ( fmt == 3 )
   {
      return DISK_ERR;
   }
   if ( fmt )
   {
      return NO_FILESYSTEM;        // No FAT volume is found

   }
// An FAT volume is found. Following code initializes the file system object

   if ( getWordLittle( fs->win + BPB_BytsPerSec ) != fs->sectorSize ) // (BPB_BytsPerSec must be equal to the physical sector size)
   {
      return NO_FILESYSTEM;
   }

   fasize = getWordLittle( fs->win + BPB_FATSz16 ); // Number of sectors per FAT
   if ( !fasize )
   {
      fasize = getDwordLittle( fs->win + BPB_FATSz32 );
   }
   fs->fsize = fasize;

   fs->n_fats = b = fs->win[BPB_NumFATs]; // Number of FAT copies
   if ( b != 1 && b != 2 )
   {
      return NO_FILESYSTEM;                     // (Must be 1 or 2)
   }
   fasize *= b; // Number of sectors for FAT area

   fs->csize = b = fs->win[BPB_SecPerClus]; // Number of sectors per cluster
   if ( !b || ( b & ( b - 1 ) ) )
   {
      return NO_FILESYSTEM;                        // (Must be power of 2)

   }
   fs->n_rootdir = getWordLittle( fs->win + BPB_RootEntCnt ); // Number of root directory entries
   if ( fs->n_rootdir % ( fs->sectorSize / SZ_DIR ) )
   {
      return NO_FILESYSTEM;                                              // (BPB_RootEntCnt must be sector aligned)

   }
   tsect = getWordLittle( fs->win + BPB_TotSec16 ); // Number of sectors on the volume
   if ( !tsect )
   {
      tsect = getDwordLittle( fs->win + BPB_TotSec32 );
   }

   nrsv = getWordLittle( fs->win + BPB_RsvdSecCnt ); // Number of reserved sectors
   if ( !nrsv )
   {
      return NO_FILESYSTEM;          // (BPB_RsvdSecCnt must not be 0)

   }
// Determine the FAT sub type
   sysect = nrsv + fasize + fs->n_rootdir / ( fs->sectorSize / SZ_DIR ); // RSV+FAT+DIR
   if ( tsect < sysect )
   {
      return NO_FILESYSTEM;                   // (Invalid volume size)
   }
   nclst = ( tsect - sysect ) / fs->csize; // Number of clusters
   if ( !nclst )
   {
      return NO_FILESYSTEM;           // (Invalid volume size)
   }
   fmt = FileSystem::FAT12;
   if ( nclst >= MIN_FAT16 )
   {
      fmt = FileSystem::FAT16;
   }
   if ( nclst >= MIN_FAT32 )
   {
      fmt = FileSystem::FAT32;
   }

// Boundaries and Limits
   fs->n_fatent = nclst + 2; // Number of FAT entries
   fs->database = bsect + sysect; // Data start sector
   fs->fatbase = bsect + nrsv; // FAT start sector
   if ( fmt == FileSystem::FAT32 )
   {
      if ( fs->n_rootdir )
      {
         return NO_FILESYSTEM;                 // (BPB_RootEntCnt must be 0)
      }
      fs->dirbase = getDwordLittle( fs->win + BPB_RootClus ); // Root directory start cluster
      szbfat = fs->n_fatent * 4; // (Required FAT size)
   }
   else
   {
      if ( !fs->n_rootdir )
      {
         return NO_FILESYSTEM;                  // (BPB_RootEntCnt must not be 0)
      }
      fs->dirbase = fs->fatbase + fasize; // Root directory start sector
      szbfat = ( fmt == FileSystem::FAT16 ) ? // (Required FAT size)
               fs->n_fatent * 2 : fs->n_fatent * 3 / 2 + ( fs->n_fatent & 1 );
   }
   if ( fs->fsize < ( szbfat + ( fs->sectorSize - 1 ) ) / fs->sectorSize ) // (BPB_FATSz must not be less than required)
   {
      return NO_FILESYSTEM;
   }

// Initialize cluster allocation information
   fs->free_clust = 0xFFFFFFFF;
   fs->last_clust = 0;

// Get fsinfo if available
   if ( fmt == FileSystem::FAT32 )
   {
      fs->fsi_flag = 0;
      fs->fsi_sector = bsect + getWordLittle( fs->win + BPB_FSInfo );
      if ( fs->disk->read( fs->win, fs->fsi_sector, 1 ) == IDiskIo::OK
         && getWordLittle( fs->win + BS_55AA ) == 0xAA55
         && getDwordLittle( fs->win + FSI_LeadSig ) == 0x41615252
         && getDwordLittle( fs->win + FSI_StrucSig ) == 0x61417272 )
      {
         fs->last_clust = getDwordLittle( fs->win + FSI_Nxt_Free );
         fs->free_clust = getDwordLittle( fs->win + FSI_Free_Count );
      }
   }

   fs->fs_type = fmt; // FAT sub-type
   fs->id = ++Fsid; // File system mount ID
   fs->winsect = 0; // Invalidate sector cache
   fs->wflag = 0;
   fs->clearLockEntries();

   return OK;
}

uint8_t FatSystem::FileSystem::check( uint32_t sect )
{
// Load boot record
   if ( disk->read( win, sect, 1 ) != IDiskIo::OK )
   {
      return 3;
   }

// Check record signature (always placed at offset 510 even if the sector size is >512)
   if ( getWordLittle( &win[BS_55AA] ) != 0xAA55 )
   {
      return 2;
   }

// Check "FAT" string
   if ( ( getDwordLittle( &win[BS_FilSysType] ) & 0xFFFFFF ) == 0x544146 )
   {
      return 0;
   }

   if ( ( getDwordLittle( &win[BS_FilSysType32] ) & 0xFFFFFF ) == 0x544146 )
   {
      return 0;
   }

   return 1;
}

