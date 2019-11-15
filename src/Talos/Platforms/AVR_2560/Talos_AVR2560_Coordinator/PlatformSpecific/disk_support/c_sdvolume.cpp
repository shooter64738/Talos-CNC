/*
* c_sdvolume.cpp
*
* Created: 11/14/2019 4:58:06 PM
* Author: jeff_d
*/


#include "c_sdvolume.h"
#include "FatStructs.h"
#include "..\..\..\..\..\Coordinator\Processing\Main\Main_Process.h"

// default constructor
c_sdvolume::c_sdvolume()
{
} //c_sdvolume

// default destructor
c_sdvolume::~c_sdvolume()
{
} //~c_sdvolume



uint8_t c_sdvolume::initialize(uint8_t partition)
{
	uint32_t volumeStartBlock = 0;
	
	//sdCard_ = dev;
	// if part == 0 assume super floppy with FAT boot sector in block zero
	// if part > 0 assume mbr volume with partition table
	if (partition) {
		if (partition > 4)return false;
		if (!cacheRawBlock(volumeStartBlock, CACHE_FOR_READ)) return false;
		part_t* p = &cacheBuffer_.mbr.part[partition-1];
		if ((p->boot & 0X7F) !=0  ||
		p->totalSectors < 100 ||
		p->firstSector == 0) {
			// not a valid partition
			return false;
		}
		volumeStartBlock = p->firstSector;
	}
	
	if (!cacheRawBlock(volumeStartBlock, CACHE_FOR_READ)) return false;
	bpb_t* bpb = &cacheBuffer_.fbs.bpb;
	if (bpb->bytesPerSector != 512 ||
	bpb->fatCount == 0 ||
	bpb->reservedSectorCount == 0 ||
	bpb->sectorsPerCluster == 0) {
		// not valid FAT volume
		return false;
	}
	fatCount_ = bpb->fatCount;
	blocksPerCluster_ = bpb->sectorsPerCluster;

	// determine shift that is same as multiply by blocksPerCluster_
	clusterSizeShift_ = 0;
	while (blocksPerCluster_ != (1 << clusterSizeShift_)) {
		// error if not power of 2
		if (clusterSizeShift_++ > 7) return false;
	}
	blocksPerFat_ = bpb->sectorsPerFat16 ?
	bpb->sectorsPerFat16 : bpb->sectorsPerFat32;

	fatStartBlock_ = volumeStartBlock + bpb->reservedSectorCount;

	// count for FAT16 zero for FAT32
	rootDirEntryCount_ = bpb->rootDirEntryCount;

	// directory start for FAT16 dataStart for FAT32
	rootDirStart_ = fatStartBlock_ + bpb->fatCount * blocksPerFat_;

	// data start for FAT16 and FAT32
	dataStartBlock_ = rootDirStart_ + ((32 * bpb->rootDirEntryCount + 511)/512);
	Talos::Coordinator::Main_Process::host_serial.print_string("vol 3");
	// total blocks for FAT16 or FAT32
	uint32_t totalBlocks = bpb->totalSectors16 ?
	bpb->totalSectors16 : bpb->totalSectors32;
	// total data blocks
	clusterCount_ = totalBlocks - (dataStartBlock_ - volumeStartBlock);

	// divide by cluster size to get cluster count
	clusterCount_ >>= clusterSizeShift_;

	// FAT type is determined by cluster count
	if (clusterCount_ < 4085)
	{
		this->fatType_ = 12;
	}
	else if (clusterCount_ < 65525)
	{
		this->fatType_ = 16;
	}
	else
	{
		rootDirStart_ = bpb->fat32RootCluster;
		this->fatType_ = 32;
	}
	Talos::Coordinator::Main_Process::host_serial.print_string("FAT Type ");
	Talos::Coordinator::Main_Process::host_serial.print_int32(this->fatType_);
	Talos::Coordinator::Main_Process::host_serial.print_string("\r\n");
	uint32_t size = blocksPerCluster_;
	size *= clusterCount_;
	size *= 512;
	Talos::Coordinator::Main_Process::host_serial.print_string("\r\nSize ");
	Talos::Coordinator::Main_Process::host_serial.print_int32(size);
	Talos::Coordinator::Main_Process::host_serial.print_string("\r\n");
	return true;
}

uint8_t c_sdvolume::cacheRawBlock(uint32_t blockNumber, uint8_t action) {
	if (cacheBlockNumber_ != blockNumber)
	{
		if (!cacheFlush()) return false;
		if (!c_sdcard::__readBlock__(blockNumber, cacheBuffer_.data)) return false;
		cacheBlockNumber_ = blockNumber;
	}
	cacheDirty_ |= action;
	return true;
}

uint8_t c_sdvolume::cacheFlush() {
	if (cacheDirty_)
	{
		if (!c_sdcard::__writeBlock__(cacheBlockNumber_, cacheBuffer_.data)) {
			return false;
		}
		// mirror FAT tables
		if (cacheMirrorBlock_)
		{
			if (!c_sdcard::__writeBlock__(cacheMirrorBlock_, cacheBuffer_.data)) {
				return false;
			}
			cacheMirrorBlock_ = 0;
		}
		cacheDirty_ = 0;
	}
	return true;
}

uint8_t c_sdvolume::chainSize(uint32_t cluster, uint32_t* size)
{
	uint32_t s = 0;
	do
	{
		if (!fatGet(cluster, &cluster)) return false;
		s += 512UL << clusterSizeShift_;
	} while (!isEOC(cluster));
	*size = s;
	return true;
}

uint8_t c_sdvolume::fatGet(uint32_t cluster, uint32_t* value)
{
	if (cluster > (clusterCount_ + 1)) return false;
	uint32_t lba = fatStartBlock_;
	lba += fatType_ == 16 ? cluster >> 8 : cluster >> 7;
	if (lba != cacheBlockNumber_) {
		if (!cacheRawBlock(lba, CACHE_FOR_READ)) return false;
	}
	if (fatType_ == 16) {
		*value = cacheBuffer_.fat16[cluster & 0XFF];
		} else {
		*value = cacheBuffer_.fat32[cluster & 0X7F] & FAT32MASK;
	}
	return true;
}

uint8_t c_sdvolume::isEOC(uint32_t cluster)
{
	return  cluster >= (fatType_ == 16 ? FAT16EOC_MIN : FAT32EOC_MIN);
}