/*
* c_sdvolume.h
*
* Created: 11/14/2019 4:58:06 PM
* Author: jeff_d
*/


#ifndef __C_SDVOLUME_H__
#define __C_SDVOLUME_H__

#include <stdint.h>
#include "c_sdcard.h"


class c_sdvolume
{
	//variables
	public:
	#include "FatStructs.h"
	union cache_t {
		/** Used to access cached file data blocks. */
		uint8_t  data[512];
		/** Used to access cached FAT16 entries. */
		uint16_t fat16[256];
		/** Used to access cached FAT32 entries. */
		uint32_t fat32[128];
		/** Used to access cached directory entries. */
		dir_t    dir[16];
		/** Used to access a cached MasterBoot Record. */
		mbr_t    mbr;
		/** Used to access to a cached FAT boot sector. */
		fbs_t    fbs;
	};

	uint8_t const CACHE_FOR_READ = 0;
	// value for action argument in cacheRawBlock to indicate cache dirty
	uint8_t const CACHE_FOR_WRITE = 1;

	uint32_t allocSearchStart_;   // start cluster for alloc search
	uint8_t blocksPerCluster_;    // cluster size in blocks
	uint32_t blocksPerFat_;       // FAT size in blocks
	uint32_t clusterCount_;       // clusters in one FAT
	uint8_t clusterSizeShift_;    // shift to convert cluster count to block count
	uint32_t dataStartBlock_;     // first data block number
	uint8_t fatCount_;            // number of FATs on volume
	uint32_t fatStartBlock_;      // start block for first FAT
	uint8_t fatType_;             // volume type (12, 16, OR 32)
	uint16_t rootDirEntryCount_;  // number of entries in FAT16 root dir
	uint32_t rootDirStart_;       // root start block for FAT16, cluster for FAT32

	cache_t  cacheBuffer_;     // 512 byte cache for Sd2Card
	uint8_t  cacheDirty_ = 0;  // cacheFlush() will write block if true
	uint32_t cacheMirrorBlock_ = 0;  // mirror  block for second FAT
	uint32_t cacheBlockNumber_ = 0XFFFFFFFF;


	protected:
	private:

	//functions
	public:
	uint8_t initialize( uint8_t partition);
	uint8_t cacheRawBlock(uint32_t blockNumber, uint8_t action);
	uint8_t cacheFlush();
	uint8_t chainSize(uint32_t cluster, uint32_t* size);
	uint8_t fatGet(uint32_t cluster, uint32_t* value);
	uint8_t isEOC(uint32_t cluster);

	c_sdvolume();
	~c_sdvolume();
	c_sdvolume( const c_sdvolume &c );
	c_sdvolume& operator=( const c_sdvolume &c );

	protected:
	private:
	

}; //c_sdvolume

#endif //__C_SDVOLUME_H__
