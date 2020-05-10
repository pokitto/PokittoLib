#pragma once

namespace Audio {

    inline void mix(void *dst, const void* src, std::size_t count) {
        char *buffer = static_cast<char*>(dst);
        const char *tmp = static_cast<const char*>(src);
        for(u32 i=0; i<count; ++i){
            s32 v = s32(buffer[i]) + s32(tmp[i]) - 127;
            if(v < 0) v = 0;
            else if(v > 0xFF) v = 0xFF;
            buffer[i] = v;
        }
    }

}
