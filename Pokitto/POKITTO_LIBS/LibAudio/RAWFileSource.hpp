#pragma once

#include <new>
#include <File>

#define ALIGNED __attribute__ ((aligned))
#define NAKED __attribute__((naked))

namespace Audio {


    class RAWFileSource {
        File* file;
        u32 start;
        u32 offset;
        u32 length;
        bool paused;
        bool loop;

        static void copy(u8 *buffer, void*ptr){
            auto& sd = *reinterpret_cast<RAWFileSource*>(ptr);
            if(sd.paused || (sd.offset - sd.start) >= sd.length){
                MemOps::set(buffer, 127, 512);
                return;
            }
            s32 rem = 512;
            while(rem){
                sd.file->seek(sd.offset);
                u32 count = std::min<u32>(
                    rem,
                    sd.length - (sd.offset - sd.start));
                count = sd.file->read(buffer, count);
                buffer += count;
                sd.offset += count;
                rem -= count;
                if((sd.offset - sd.start) >= sd.length && sd.loop){
                    sd.offset = sd.start;
                } else {
                    return;
                }
            }
        }

        static void mix(u8 *buffer, void*ptr){
            u8 tmpbuf[512];
            u8 *tmp = tmpbuf;
            auto& sd = *reinterpret_cast<RAWFileSource*>(ptr);
            if(sd.paused || (sd.offset - sd.start) >= sd.length){
                return;
            }
            s32 rem = 512;
            while(rem){
                sd.file->seek(sd.offset);
                u32 count = std::min<u32>(
                    rem,
                    sd.length - (sd.offset - sd.start));
                count = sd.file->read(tmp, count);
                tmp += count;
                sd.offset += count;
                rem -= count;
                if((sd.offset - sd.start) >= sd.length && sd.loop){
                    sd.offset = sd.start;
                } else {
                    MemOps::set(tmp, 127, rem);
                    rem = 0;
                }
            }
            Audio::mix(buffer, tmpbuf, 512);
        }

    public:
        template<u32 channel = 0>
        static RAWFileSource& play(File& fileRef, u32 length){
            static RAWFileSource sd;
            sd.file = &fileRef;
            sd.offset = fileRef.tell();
            sd.start = sd.offset;
            sd.length = length;
            sd.loop = true;
            Audio::connect(channel, &sd, channel == 0 ? copy : mix);
            return sd;
        }
        
        template<u32 channel = 0>
        static RAWFileSource* play(const char *name){
            static ALIGNED u8 ram[sizeof(File)];
            static bool init = false;
            if(!init){
                new (static_cast<void*>(ram)) File();
                init = true;
            }

            File& file = *reinterpret_cast<File*>(ram);
            if(file.openRO(name)){
                return &play<channel>(file, file.size());
            } else {
                Audio::stop<channel>();
            }
            return nullptr;
        }

        RAWFileSource& setPaused(bool isPaused){
            paused = isPaused;
            return *this;
        }
        
        RAWFileSource& setLoop(bool doesLoop){
            loop = doesLoop;
            return *this;
        }

        bool ended(){
            return (offset - start) >= length;
        }
    };

    template<u32 channel = 0>
    RAWFileSource& play(File& fileRef, u32 length = 0){
        return RAWFileSource::play<channel>(fileRef, length ?: fileRef.size());
    }

    template<u32 channel = 0>
    RAWFileSource *play(const char *name){
        return RAWFileSource::play<channel>(name);
    }
}
