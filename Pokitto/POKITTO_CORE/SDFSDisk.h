#ifndef SDFSDISK_H
#define SDFSDISK_H

#include "SDFileSystem.h"

// declared in PokittoDisk.cpp
extern char currentfile[15]; // holds current file's name

// declared in SDFileSystem.cpp
extern FileHandle *sdfsdisk_fileptr;

#define pokInitSD fs_pokInitSD
#define fileOpen fs_fileOpen
#define fileReadBytes fs_fileReadBytes
#define fileClose fs_fileClose
#define fileRewind fs_fileRewind
#define isThisFileOpen fs_isThisFileOpen

/*
#define fs fs_fs

struct fs_fs_t {
    off_t fsize;
} fs_fs;
*/

inline bool fs_pokInitSD(){
    if( !FATFileSystem::_ffs[0] )
	new SDFileSystem(P0_9, P0_8, P0_6, P0_7, "streaming", NC, SDFileSystem::SWITCH_NONE, 25000000 );
    return true;
}

inline bool fs_isThisFileOpen( const char *buffer ){
    int a=0;
    if( !sdfsdisk_fileptr ) return false;
    
    a = strcmp((const char*)buffer,(const char*)currentfile); // returns 0 if strings are identical
    
    return a == 0;
    
}

inline void fs_fileRewind(){
    if( !sdfsdisk_fileptr ) return;
    sdfsdisk_fileptr->lseek( 0, SEEK_SET );
}

inline uint8_t fs_fileOpen(const char *buffer, char fmode){
    
    if( sdfsdisk_fileptr )
	sdfsdisk_fileptr->close();

    sdfsdisk_fileptr = ((SDFileSystem *) FATFileSystem::_ffs[0])->open( buffer, fmode );

    if( !sdfsdisk_fileptr )
	return 1;

    strcpy(currentfile,buffer);
    fs.fsize = sdfsdisk_fileptr->flen();
    return 0;
}

inline uint16_t fs_fileReadBytes(uint8_t * b, uint16_t n) {
    if( !sdfsdisk_fileptr ) return 0;
    return sdfsdisk_fileptr->read( b, n );
}

inline void fs_fileClose(){
    if( sdfsdisk_fileptr )
	sdfsdisk_fileptr->close();
    sdfsdisk_fileptr = NULL;
}

#endif
