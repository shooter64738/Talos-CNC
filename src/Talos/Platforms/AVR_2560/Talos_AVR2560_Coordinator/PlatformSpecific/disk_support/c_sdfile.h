/*
* c_sdfile.h
*
* Created: 11/14/2019 7:27:19 PM
* Author: jeff_d
*/


#ifndef __C_SDFILE_H__
#define __C_SDFILE_H__

#include <stdint.h>
#include "c_sdvolume.h"

// flags for ls()
/** ls() flag to print modify date */
uint8_t const LS_DATE = 1;
/** ls() flag to print file size */
uint8_t const LS_SIZE = 2;
/** ls() flag for recursive list of subdirectories */
uint8_t const LS_R = 4;

// use the gnu style oflag in open()
/** open() oflag for reading */
uint8_t const O_READ = 0X01;
/** open() oflag - same as O_READ */
uint8_t const O_RDONLY = O_READ;
/** open() oflag for write */
uint8_t const O_WRITE = 0X02;
/** open() oflag - same as O_WRITE */
uint8_t const O_WRONLY = O_WRITE;
/** open() oflag for reading and writing */
uint8_t const O_RDWR = (O_READ | O_WRITE);
/** open() oflag mask for access modes */
uint8_t const O_ACCMODE = (O_READ | O_WRITE);
/** The file offset shall be set to the end of the file prior to each write. */
uint8_t const O_APPEND = 0X04;
/** synchronous writes - call sync() after each write */
uint8_t const O_SYNC = 0X08;
/** create the file if nonexistent */
uint8_t const O_CREAT = 0X10;
/** If O_CREAT and O_EXCL are set, open() shall fail if the file exists */
uint8_t const O_EXCL = 0X20;
/** truncate the file to zero length */
uint8_t const O_TRUNC = 0X40;

// flags for timestamp
/** set the file's last access date */
uint8_t const T_ACCESS = 1;
/** set the file's creation date and time */
uint8_t const T_CREATE = 2;
/** Set the file's write date and time */
uint8_t const T_WRITE = 4;
// values for type_
/** This SdFile has not been opened. */
uint8_t const FAT_FILE_TYPE_CLOSED = 0;
/** SdFile for a file */
uint8_t const FAT_FILE_TYPE_NORMAL = 1;
/** SdFile for a FAT16 root directory */
uint8_t const FAT_FILE_TYPE_ROOT16 = 2;
/** SdFile for a FAT32 root directory */
uint8_t const FAT_FILE_TYPE_ROOT32 = 3;
/** SdFile for a subdirectory */
uint8_t const FAT_FILE_TYPE_SUBDIR = 4;
/** Test value for directory type */
uint8_t const FAT_FILE_TYPE_MIN_DIR = FAT_FILE_TYPE_ROOT16;

class c_sdfile
{
	//variables
	public:
	uint8_t type_;
	uint32_t firstCluster_ = 0;
	uint32_t fileSize_ = 0;
	uint8_t flags_ = 0;
	uint32_t curCluster_ = 0;
	uint32_t curPosition_ = 0;

	// root has no directory entry
	uint32_t dirBlock_ = 0;
	uint8_t dirIndex_ = 0;
	protected:
	private:

	//functions
	public:
	uint8_t openRoot(c_sdvolume * volume);

	c_sdfile();
	~c_sdfile();
	c_sdfile( const c_sdfile &c );
	c_sdfile& operator=( const c_sdfile &c );
	protected:
	private:
	

}; //c_sdfile

#endif //__C_SDFILE_H__
