#pragma once
#include <MemOps>

extern SDL_AudioDeviceID audioDevice;

namespace Audio {
    inline void setVolume(u32 v){
        v = v * v * 256 / 36864;
        audio_volume = v;
    }

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

    template <u32 channelCount, u32 sampleRate>
    class Sink {
        internal::Channel channels[channelCount];
        u8 wasInit = false;

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

        void (*nextHook)(bool);

        void init(){
            if(wasInit)
                return;
            wasInit = true;
            // set volume
            Audio::setVolume(Pokitto::Sound::globalVolume);

            for(int i=0; i<channelCount; ++i){
                channels[i].source = nullptr;
            }

            for(int i=0; i<bufferCount; ++i){
                audio_state[i] = 0;
            }
            audio_playHead = 0;

            SDL_PauseAudioDevice(audioDevice, 1);
            SDL_CloseAudioDevice(audioDevice);

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

    public:

        Sink(){
            nextHook = Pokitto::Core::updateHook;
            Pokitto::Core::updateHook =
                +[](bool isFrame){
                     auto self = reinterpret_cast<Sink*>(Audio::internal::sinkInstance);
                     self->update();
                     self->nextHook(isFrame);
                 };
            Audio::internal::sinkInstance = this;
            Audio::connect =
                +[](u32 channelNumber, void *data, Source source){
                     auto self = reinterpret_cast<Sink*>(Audio::internal::sinkInstance);
                     self->connect(channelNumber, data, source);
                 };
        }

        void connect(u32 channelNumber, void *data, Source source){
            init();
            if(channelNumber >= channelCount)
                channelNumber = channelCount-1;
            channels[channelNumber].source = source;
            channels[channelNumber].data = data;
        }

        void update(){
            init();
            for(u32 i = 0; i < bufferCount; ++i){
                if(audio_state[i])
                    continue;
                u8 *buffer = audio_buffer + i * 512;
                for(u32 c = 0; c < channelCount; ++c){
                    auto source = channels[c].source;
                    if(source){
                        source(buffer, channels[c].data);
                    }
                }
                audio_state[i] = 1;
            }
        }

        template <u32 channel>
        void stop(){
            channels[channel].source = nullptr;
        }

    };
}
