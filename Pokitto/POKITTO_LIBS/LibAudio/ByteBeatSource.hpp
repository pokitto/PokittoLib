#pragma once

namespace Audio {    
    class ByteBeatSource {
        u8 (*cb)(u32);
        u32 t;

        static void copy(u8 *buffer, void *ptr){
            auto& state = *reinterpret_cast<ByteBeatSource*>(ptr);
            for(u32 i = 0; i < 512; ++i){
                buffer[i] = state.cb(state.t++);
            }
        }

        static void mix(u8 *buffer, void *ptr){
            auto& state = *reinterpret_cast<ByteBeatSource*>(ptr);
            for(u32 i = 0; i < 512; ++i){
                buffer[i] = Audio::mix(buffer[i], state.cb(state.t++));
            }
        }

    public:
        template<u32 channel = 0>
        static ByteBeatSource& play(u8 (*cb)(u32 t)){
            static ByteBeatSource state;
            state.t = 0;
            state.cb = cb;
            Audio::connect(
                channel,
                &state,
                channel == 0 ? copy : mix);
            return state;
        }
    };

    template<u32 channel = 0>
    inline ByteBeatSource& play(u8 (*cb)(u32 t)){
        return ByteBeatSource::play<channel>(cb);
    }
}
    
