/*
* c_sdfile.cpp
*
* Created: 11/14/2019 7:27:19 PM
* Author: jeff_d
*/


#include "c_sdfile.h"
#include "FatStructs.h"
#include "..\..\..\..\..\Coordinator\Processing\Main\Main_Process.h"

// default constructor
c_sdfile::c_sdfile()
{
} //c_sdfile

// default destructor
c_sdfile::~c_sdfile()
{
} //~c_sdfile

uint8_t c_sdfile::openRoot(c_sdvolume * volume)
{
	// error if file is already open
	//if (isOpen()) return false;
	Talos::Coordinator::Main_Process::host_serial.print_string("open root\r\n");
	Talos::Coordinator::Main_Process::host_serial.print_string("FAT Type ");
	Talos::Coordinator::Main_Process::host_serial.print_int32(volume->fatType_);
	Talos::Coordinator::Main_Process::host_serial.print_string("\r\n");
	if (volume->fatType_ == 16)
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("fat16\r\n");
		this->type_ = FAT_FILE_TYPE_ROOT16;
		this->firstCluster_ = 0;
		this->fileSize_ = 32 * volume->rootDirEntryCount_;
	}
	else if (volume->fatType_ == 32)
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("fat32\r\n");
		this->type_ = FAT_FILE_TYPE_ROOT32;
		this->firstCluster_ = volume->rootDirStart_;
		if (!volume->chainSize(firstCluster_, &fileSize_))
		{
			return false;
		}
	}
	else
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("not fat 32\r\n");
		// volume is not initialized or FAT12
		return false;
	}
	//this->vol_ = vol;
	// read only
	this->flags_ = O_READ;

	// set to start of file
	this->curCluster_ = 0;
	this->curPosition_ = 0;

	// root has no directory entry
	this->dirBlock_ = 0;
	this->dirIndex_ = 0;
	return true;
}

void c_sdfile::ls(uint8_t flags, uint8_t indent)
 {
	//dir_t* p;
//
	//rewind();
	//while ((p = readDirCache())) {
		//// done if past last used entry
		//if (p->name[0] == DIR_NAME_FREE) break;
//
		//// skip deleted entry and entries for . and  ..
		//if (p->name[0] == DIR_NAME_DELETED || p->name[0] == '.') continue;
//
		//// only list subdirectories and files
		//if (!DIR_IS_FILE_OR_SUBDIR(p)) continue;
//
		//// print any indent spaces
		//for (int8_t i = 0; i < indent; i++) Serial.print(' ');
//
		//// print file name with possible blank fill
		//printDirName(*p, flags & (LS_DATE | LS_SIZE) ? 14 : 0);
//
		//// print modify date/time if requested
		//if (flags & LS_DATE) {
			//printFatDate(p->lastWriteDate);
			//Serial.print(' ');
			//printFatTime(p->lastWriteTime);
		//}
		//// print size if requested
		//if (!DIR_IS_SUBDIR(p) && (flags & LS_SIZE)) {
			//Serial.print(' ');
			//Serial.print(p->fileSize);
		//}
		//Serial.println();
//
		//// list subdirectory content if requested
		//if ((flags & LS_R) && DIR_IS_SUBDIR(p)) {
			//uint16_t index = curPosition()/32 - 1;
			//SdFile s;
			//if (s.open(this, index, O_READ)) s.ls(flags, indent + 2);
			//seekSet(32 * (index + 1));
		//}
	//}
}