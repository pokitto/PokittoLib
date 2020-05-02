#pragma once

#include "SDFileSystem.h"

class File {
    FileHandle *handle;

    static void initSDFS(){
        if( !FATFileSystem::_ffs[0] ){
            printf("Init\n");
            auto sdfs = new SDFileSystem(P0_9, P0_8, P0_6, P0_7, "sd", NC, SDFileSystem::SWITCH_NONE, 24000000 );
            sdfs->crc(false);
        }
    }

public:
    inline static bool error;
    
    File(){
        handle = nullptr;
        initSDFS();
    }

    File(const File&) = delete;

    File(File&& other) : handle(other.handle) {
        other.handle = nullptr;
    }

    ~File(){
        close();
    }

    void close(){
        if(handle){
            handle->close();
            handle = nullptr;
        }
    }

    operator bool(){
        return handle;
    }

    File& openRO(const char *name){
        close();
        handle = ((SDFileSystem *) FATFileSystem::_ffs[0])->open( name, 0 );
        return *this;
    }

    File& openRW(const char *name, bool create, bool append) {
        close();

        handle = ((SDFileSystem *) FATFileSystem::_ffs[0])->open(
            name,
            O_RDWR | (create?O_CREAT:0)
            );

        if( handle && append ){
            seek(size());
        }

        return *this;
    }

    uint32_t size(){
        while(!handle);
        return handle->flen();
    }

    uint32_t tell(){
        while(!handle);
        return handle->lseek(0, SEEK_CUR);
    }

    File& seek(uint32_t offset){
        while(!handle);
        handle->lseek(offset, SEEK_SET);
        return *this;
    }

    uint32_t read(void *ptr, uint32_t count){
        while(!handle);
        return handle->read(ptr, count);
    }

    uint32_t write(const void *ptr, uint32_t count){
        while(!handle);
        return handle->write(ptr, count);
    }

    template< typename T, size_t S > uint32_t read( T (&data)[S] ){
	    return read( data, sizeof(data) );
    }

    template< typename T, size_t S > uint32_t write( const T (&data)[S] ){
	    return write( data, sizeof(data) );
    }

    template< typename T >
    T read(){
        T tmp = {};
        read( (void*) &tmp, sizeof(T) );
        return tmp;
    }

    template< typename T >
    File & operator >> (T &i){
    	read(&i, sizeof(T));
    	return *this;
    }

    template< typename T >
    File & operator << (const T& i){
    	write(&i, sizeof(T));
    	return *this;
    }

    File & operator << (const char *str ){
        uint32_t len = 0;
        while(str[len]) len++;
        write(str, len);
    	return *this;
    }

};
