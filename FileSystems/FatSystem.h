/*
 * FatSystem.h
 *
 *  Created on: 08.04.2016
 *      Author: viktor.pankraz
 */

#ifndef FILESYSTEMS_FATSYSTEM_H_
#define FILESYSTEMS_FATSYSTEM_H_

#include "SwFramework.h"

#ifndef SECTOR_SIZE
#define SECTOR_SIZE 512
#endif

class IDiskIo;

class FatSystem
{
   public:

      enum Result
      {
         OK = 0,          // (0) Succeeded
         DISK_ERR,        // (1) A hard error occured in the low level disk I/O layer
         INT_ERR,         // (2) Assertion failed
         NOT_READY,       // (3) The physical drive cannot work
         NO_FILE,         // (4) Could not find the file
         NO_PATH,         // (5) Could not find the path
         INVALID_NAME,    // (6) The path name format is invalid
         DENIED,          // (7) Acces denied due to prohibited access or directory full
         EXIST,           // (8) Acces denied due to prohibited access
         INVALID_OBJECT,  // (9) The file/directory object is invalid
         WRITE_PROTECTED, // (10) The physical drive is write protected
         INVALID_DRIVE,   // (11) The logical drive number is invalid
         NOT_ENABLED,     // (12) The volume has no work area
         NO_FILESYSTEM,   // (13) There is no valid FAT volume
         MKFS_ABORTED,    // (14) The f_mkfs() aborted due to any parameter error
         TIMEOUT,         // (15) Could not get a grant to access the volume within defined period
         LOCKED,          // (16) The operation is rejected according to the file shareing policy
         NOT_ENOUGH_CORE, // (17) LFN working buffer could not be allocated
         TOO_MANY_OPEN_FILES, // (18) Number of open files > _FS_SHARE
         INVALID_PARAMETER // (19) Given parameter is invalid
      };

      class Partition
      {
         uint8_t physicalDrive;
         uint8_t index;
      };

      class FileSystem
      {

         public:

            enum Types
            {
               FAT12 = 1,
               FAT16,
               FAT32,
            };

            ////    Operations    ////

            FileSystem( IDiskIo* _disk );

            ~FileSystem();

            Result mount( uint8_t driveNumber );

            Result removeChain( uint32_t cluster );

            Result putFat( uint32_t cluster, uint32_t value );

            uint32_t getFat( uint32_t cluster );

            uint32_t createChain( uint32_t cluster );

            uint32_t clust2sect( uint32_t cluster );

            uint8_t check( uint32_t sect );

            Result validate( uint16_t id );

            Result sync();

            inline uint32_t convertToSector( uint32_t cluster )
            {
               cluster -= 2;
               if ( cluster >= ( n_fatent - 2 ) )
               {
                  // Invalid cluster#
                  return 0;
               }
               return cluster * csize + database;
            }

            inline bool lock()
            {
               // if reentrant is needed this function must be correct implemented
               return true;
            }

            inline bool unlock()
            {
               // if reentrant is needed this function must be correct implemented
               return true;
            }

            void clearLockEntries();

            Result moveWindow( uint32_t sector );

            uint8_t fs_type; // FAT sub-type (0:Not mounted)
            uint8_t csize; // Sectors per cluster (1,2,4...128)
            uint8_t n_fats; // Number of FAT copies (1,2)
            uint8_t wflag; // win[] dirty flag (1:must be written back)
            uint8_t fsi_flag; // fsinfo dirty flag (1:must be written back)
            IDiskIo* disk;
            uint16_t id; // File system mount ID
            uint16_t sectorSize;
            uint16_t n_rootdir; // Number of root directory entries (FAT12/16)
            uint32_t last_clust; // Last allocated cluster
            uint32_t free_clust; // Number of free clusters
            uint32_t fsi_sector; // fsinfo sector (FAT32)
            uint32_t n_fatent; // Number of FAT entries (= number of clusters + 2)
            uint32_t fsize; // Sectors per FAT
            uint32_t fatbase; // FAT start sector
            uint32_t dirbase; // Root directory start sector (FAT32:Cluster#)
            uint32_t database; // Data start sector
            uint32_t winsect; // Current sector appearing in the win[]
            uint8_t win[SECTOR_SIZE]; // Disk access window for Directory, FAT (and Data on tiny cfg)
      };

      class File
      {
         public:

            enum Status
            {
               FA_OPEN_EXISTING = 0x00,
               FA_READ = 0x01,
               FA_WRITE = 0x02,
               FA_CREATE_NEW = 0x04,
               FA_CREATE_ALWAYS = 0x08,
               FA_OPEN_ALWAYS = 0x10,
               FA__WRITTEN = 0x20,
               FA__DIRTY = 0x40,
               FA__ERROR = 0x80,
            };

            class Semaphor
            {
               public:

                  inline void notifyRemovedFileSystem( FileSystem* _fs )
                  {
                     if ( fs == _fs )
                     {
                        fs = NULL;
                     }
                  }

                  inline bool isFree()
                  {
                     return ( fs == NULL );
                  }

                  inline void decrement()
                  {
                     if ( ctr == 0x100 )
                     {
                        ctr = 0;
                     }
                     if ( ctr )
                     {
                        ctr--;
                     }
                     if ( !ctr )
                     {
                        fs = NULL;
                     }
                  }

                  FileSystem* fs; // File ID 1, volume (NULL:blank entry)
                  uint32_t clu; // File ID 2, directory
                  uint16_t idx; // File ID 3, directory index
                  uint16_t ctr; // File open counter, 0:none, 0x01..0xFF:read open count, 0x100:write mode
            };

            ////    Operations    ////

         public:

            inline File()
            {
               memset( this, 0, sizeof( File ) );
            }

            Result sync();

            Result close();

            Result open( const char* filename, uint8_t mode );

            Result write( const void* buffer, uint16_t len, uint16_t& bytesWritten );

            Result seek( uint32_t offset );

            inline uint32_t getSize()
            {
               return fsize;
            }

            inline const char* getName() const
            {
               return fname;
            }

            inline bool isOpen() const
            {
               return ( fs != NULL );
            }

         private:

            FileSystem* fs; // Pointer to the owner file system object
            char fname[13]; // Short file name (8.3 format)
            uint16_t id;  // Owner file system mount ID
            uint8_t flag; // File status flags
            uint8_t pad1;
            uint32_t fptr; // File read/write pointer (0 on file open)
            uint32_t fsize; // File size
            uint32_t sclust; // File start cluster (0 when fsize==0)
            uint32_t clust; // Current cluster
            uint32_t dsect; // Current data sector
            uint32_t dir_sect; // Sector containing the directory entry
            uint8_t* dir_ptr; // Ponter to the directory entry in the window
            uint8_t lockid; // File lock ID (index of file semaphore table)
            uint8_t buf[SECTOR_SIZE]; // File data read/write buffer
      };

      class Directory
      {
         public:

            enum AttributeBits
            {
               RDO = 0x01, // Read only
               HID = 0x02, // Hidden
               SYS = 0x04, // System
               VOL = 0x08, // Volume label
               LFN = 0x0F, // LFN entry
               DIR = 0x10, // Directory
               ARC = 0x20, // Archive
               MASK = 0x3F, // Mask of defined bits
            };

            ////    Operations    ////

         public:

            inline Directory()
            {
               memset( this, 0, sizeof( Directory ) );
            }

            static Result create( const char* path );

            Result open( const char* path );

            // TODO private:

            Result followPath( const char* path );

            Result checkLock( uint8_t acc );

            uint8_t lock( uint8_t acc );

            Result registerObject();

            Result find();

            Result createName( const char** path );

            Result setIndex( uint16_t idx );

            Result next( bool stretch );

            FileSystem* fs; // Pointer to the owner file system object
            uint16_t id;  // Owner file system mount ID
            uint16_t index; // Current read/write index number
            uint32_t sclust; // Table start cluster (0:Root dir)
            uint32_t clust; // Current cluster
            uint32_t sect; // Current sector
            uint8_t* dir; // Pointer to the current SFN entry in the win[]
            uint8_t* fn; // Pointer to the SFN (in/out) {file[8],ext[3],status[1]}
      };

      class FileInfo
      {
         public:

            enum Attribute
            {
               AM_RDO = 0x01, // Read only
               AM_HID = 0x02, // Hidden
               AM_SYS = 0x04, // System
               AM_VOL = 0x08, // Volume label
               AM_LFN = 0x0F, // LFN entry
               AM_DIR = 0x10, // Directory
               AM_ARC = 0x20, // Archive
               AM_MASK = 0x3F, // Mask of defined bits
            };

            inline FileInfo()
            {
               memset( this, 0, sizeof( FileInfo ) );
            }

            uint32_t fsize; // File size
            uint16_t fdate; // Last modified date
            uint16_t ftime; // Last modified time
            uint8_t fattrib; // Attribute
            char fname[13]; // Short file name (8.3 format)
      };

      ////    Operations    ////

   public:

      static Result isMounted( const char** pPath, FileSystem** pFileSystem,
                               bool checkWriteProtection );
};

#endif /* FILESYSTEMS_FATSYSTEM_H_ */
