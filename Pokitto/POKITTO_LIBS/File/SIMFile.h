#pragma once

#include <cstddef>
#include <cstdio>
#include <LibLog>

struct FileInfo {
    const char *_name;
    const char *name() const {
        return _name;
    }
};

class File {
    FILE *handle;
public:
    inline static int error;

    File(){
        handle = nullptr;
        initYAPFS();
    }

    File(const File&) = delete;

    File(File&& other) : handle(other.handle) {
        other.handle.fs = nullptr;
    }

    ~File(){
        close();
    }

    void close(){
        if(handle){
            fclose(handle);
            handle = nullptr;
        }
    }

    operator bool(){
        return handle;
    }

    static FileInfo stat(const char *name) {
        FileInfo info;
        // error = fstat(name, &info);
        return info;
    }

    File& openRO(const char *name) {
        close();
        handle = fopen(name, "rb");
        return *this;
    }

    File& openRW(const char *name, bool create, bool append) {
        close();
        handle = fopen(name, "rb+");
        if(!handle && create)
            handle = fopen(name, "wb+");
        if(handle && append)
            fseek(handle, 0, SEEK_END);
        return *this;
    }

    uint32_t size(){
        if(!*this)
            return 0;
        fpos_t pos;
        fgetpos(handle, &pos);
        fseek(handle, 0, SEEK_END);
        auto len = ftell(handle);
        fsetpos(handle, pos);

        return len;
    }

    uint32_t tell(){
        return ftell(handle);
    }

    File& seek(uint32_t offset){
        fseek(handle, offset, SEEK_SET);
        return *this;
    }

    uint32_t read( void *ptr, uint32_t count ){
        if(!*this) return 0;
        return fread(ptr, 1, count, handle);
    }

    uint32_t write( const void *ptr, uint32_t count ){
        if(!*this) return 0;
        return fwrite(ptr, 1, count, handle);
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

