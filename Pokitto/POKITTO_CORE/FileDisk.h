#pragma once

#include <File>

// declared in PokittoDisk.cpp

#define CURRENT_FILE_SIZE 15
extern char currentfile[CURRENT_FILE_SIZE]; // holds current file's name

// declared in SDFileSystem.cpp
inline File streamFile;

#define pokInitSD ffs_pokInitSD
#define fileOpen ffs_fileOpen
#define fileReadBytes ffs_fileReadBytes
#define fileClose ffs_fileClose
#define fileRewind ffs_fileRewind
#define isThisFileOpen ffs_isThisFileOpen
#define getFileLength ffs_getFileLength

#ifdef POK_SIM
inline struct {
    uint32_t fsize;
} fs;
#endif

inline bool ffs_pokInitSD(){
    return true;
}

inline bool ffs_isThisFileOpen( const char *buffer ){
    int a=0;
    if( !streamFile ) return false;

    a = strcmp((const char*)buffer,(const char*)currentfile);
    return a == 0;
}

inline void ffs_fileRewind(){
    if( !streamFile ) return;
    streamFile.seek(0);
}

inline uint8_t ffs_fileOpen(const char *buffer, char fmode){
    streamFile.openRO(buffer);
    if( !streamFile )
	return 1;

    uint8_t i=0;
    while(buffer[i] && i < CURRENT_FILE_SIZE-1 ){
        currentfile[i]=buffer[i];
        i++;
    }
    currentfile[i]=0;

    fs.fsize = streamFile.size();
    return 0;
}

inline uint32_t ffs_getFileLength(){
    return streamFile.size();
}

inline uint16_t ffs_fileReadBytes(uint8_t * b, uint16_t n) {
    if( !streamFile ) return 0;
    return streamFile.read( b, n );
}

inline void ffs_fileClose(){}
