#pragma once

namespace Audio {
    class SFX8Source {
        u32 len;
        const u8 *head;

        static void copy(u8 *buffer, void *ptr){
            auto& state = *reinterpret_cast<SFX8Source*>(ptr);
            u32 len = std::min<u32>(512, state.len);
            MemOps::copy(buffer, state.head, len);
            MemOps::set(buffer + len, 127, 512 - len);
            state.head += len;
            state.len -= len;
        }

        static void mix(u8 *buffer, void *ptr){
            auto& state = *reinterpret_cast<SFX8Source*>(ptr);
            u32 len = std::min<u32>(512, state.len);
            auto head = state.head;
            Audio::mix(buffer, head, len);
            state.head += len;
            state.len -= len;
        }

        #ifndef POK_SIM
        static void lowLatencyMix(SFX8Source& state, u32 channel){
            u32 len = state.len;
            u32 out = audio_playHead + 10;
            bool inPlace = out <= 512;
            u8 tmpbuf[512];
            u8 *tmp = inPlace ? audio_buffer + out : tmpbuf;

            if(inPlace && channel != 0){
                mix(tmp, &state);
            } else {
                copy(tmp, &state);
            }

            if(!inPlace){
                u32 m = std::min<u32>(len, 512);
                if(channel == 0){
                    for(u32 i=0; i<m; ++i){
                        audio_buffer[(out + i) & 0x3FF] = tmpbuf[i];
                    }
                } else {
                    for(u32 i=0; i<m; ++i){
                        u32 o = (out + i) & 0x3FF;
                        audio_buffer[o] = Audio::mix(audio_buffer[o], tmpbuf[i]);
                    }
                }
            }
        }
        #endif

    public:
        
        template<u32 channel = 1, bool lowLatency = true>
        static SFX8Source& play(const u8 *data, u32 len){
            static SFX8Source state;
            state.len = len;
            state.head = data;
            #ifndef POK_SIM
            if(lowLatency)
                lowLatencyMix(state, channel);
            #endif
            connect(channel, &state, channel == 0 ? copy : mix);
            return state;
        }

        template<u32 channel = 1, bool lowLatency = true, u32 len = 0>
        static SFX8Source& play(const u8 (&data)[len]){
            return play<channel, lowLatency>(data, len);
        }
    };

    template<u32 channel = 1, u32 len = 0>
    inline SFX8Source& play(const u8 (&data)[len]){
        return SFX8Source::play<channel>(data, len);
    }
}
