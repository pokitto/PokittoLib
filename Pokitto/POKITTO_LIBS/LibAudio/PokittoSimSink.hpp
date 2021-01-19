#pragma once
#include <MemOps>

extern SDL_AudioDeviceID audioDevice;

namespace Audio {
    inline void setVolume(u32 v){
        v = v * v * 256 / 65536;
        audio_volume = v;
    }

    inline void mix(void *dst, const void* src, std::size_t count) {
        u8 *buffer = static_cast<u8*>(dst);
        const u8 *tmp = static_cast<const u8*>(src);
        for(u32 i=0; i<count; ++i){
            s32 v = s32(buffer[i]) + s32(tmp[i]) - 128;
            if(v < 0) v = 0;
            else if(v > 0xFF) v = 0xFF;
            buffer[i] = v;
        }
    }

    template <u32 channelCount, u32 sampleRate>
    class Sink : public BaseSink<Sink<channelCount, sampleRate>, channelCount> {
        static u8 IRQ(void){
            static u8 lastByte = 128;
            auto currentBuffer = audio_playHead >> 9;
            if(!audio_state[currentBuffer])
                return lastByte;

            lastByte = audio_buffer[audio_playHead];
            audio_playHead++;

            auto nextBuffer = audio_playHead >> 9;

            if(currentBuffer != nextBuffer){
                audio_state[currentBuffer] = 0;
                if(nextBuffer == bufferCount){
                    audio_playHead = 0;
                }
            }

            return lastByte * audio_volume >> 8;
        }

    public:
        void init(){
            if(this->wasInit)
                return;
            this->wasInit = true;

            Audio::setVolume(Pokitto::Sound::globalVolume);

            this->channels[0].source =
                +[](u8 *buffer, void *ptr){
                     MemOps::set(buffer, 128, 512);
                 };

            for(int i=1; i<channelCount; ++i){
                this->channels[i].source = nullptr;
            }

            for(int i=0; i<bufferCount; ++i){
                audio_state[i] = 0;
            }
            audio_playHead = 0;

            // SDL_PauseAudioDevice(audioDevice, 1);
            // SDL_CloseAudioDevice(audioDevice);

            SDL_AudioSpec wanted;
            SDL_memset(&wanted, 0, sizeof(wanted)); /* or SDL_zero(want) */
            wanted.freq = sampleRate;
            wanted.format = AUDIO_U8;
            wanted.channels = 1;
            wanted.samples = 512;
            wanted.callback = +[](void*, u8* stream, int len){
                for(int i=0; i<len; ++i)
                    stream[i] = IRQ();
            };

            audioDevice = SDL_OpenAudioDevice(NULL, 0, &wanted, NULL, 0);
            MemOps::set(audio_buffer, 127, 512 * bufferCount);
            SDL_PauseAudioDevice(audioDevice, 0);
        }

        Sink() = default;
    };
}
