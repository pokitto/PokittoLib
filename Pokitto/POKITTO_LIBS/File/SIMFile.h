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
            fclose(handle);
            handle = nullptr;
        }
    }

    operator bool(){
        return handle;
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
        fsetpos(handle, &pos);

        return static_cast<uint32_t>(len);
    }

    uint32_t tell(){
        return static_cast<uint32_t>(ftell(handle));
    }

    File& seek(uint32_t offset){
        fseek(handle, offset, SEEK_SET);
        return *this;
    }

    uint32_t read( void *ptr, uint32_t count ){
        if(!*this) return 0;
        return static_cast<uint32_t>(fread(ptr, 1, count, handle));
    }

    uint32_t write( const void *ptr, uint32_t count ){
        if(!*this) return 0;
        return static_cast<uint32_t>(fwrite(ptr, 1, count, handle));
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

