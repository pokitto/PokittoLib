#pragma once

#include <LibSchedule>

#define SIMPLE_TUNE(x...)                                               \
    []{                                                                 \
        struct  constexpr_string_type { const char * chars = #x; };     \
        using result = Audio::internal::apply_range<sizeof(#x)-1, Audio::internal::count_elements<constexpr_string_type>::produce >::result; \
        return Audio::internal::genSimpleTune<std::array<Audio::u8, (result::value+1)*2 >>( constexpr_string_type{}.chars ); \
    }()


namespace Audio {

    template<u32 channel, u32 timerId>
    class SimpleTuneSource {
        Note _note;
        Note _bass;
        Note _percussion[4];
        u32 _tempo;
        u32 position;
        u32 length;
        const u8 *data;

    public:
        Note& note(){ return _note; }
        Note& bass(){ return _bass; }
        Note& percussion(u32 c){ return _percussion[c%4]; }

        void tempo(u32 tempo){
            _tempo = 60000 / 4 / tempo;
        }

        void setup(u32 tempo, const u8 *data, u32 length){
            _note = Note().osc(0).volume(64);
            _bass = Note().osc(1).volume(128);

            constexpr u32 percDuration = (60000/4/120) * (PROJ_AUD_FREQ / 1000) / 16;

            _percussion[0] = Note()
                .osc(2)
                .wave(WSAW)
                .noteNumber(12)
                .volume(174)
                .attack(0)
                .decay(216)
                .sustain(27)
                .release(13)
                .overdrive(1)
                .duration(percDuration);

            _percussion[1] = Note()
                .osc(2)
                .wave(WNOISE)
                .noteNumber(73)
                .volume(255)
                .attack(0)
                .decay(234)
                .sustain(43)
                .release(32)
                .duration(percDuration);

            _percussion[2] = Note()
                .osc(2)
                .wave(5)
                .volume(255)
                .noteNumber(85)
                .attack(0)
                .decay(233)
                .sustain(27)
                .release(13)
                .duration(percDuration);

            _percussion[3] = Note() // reserved
                .osc(2)
                .wave(WTRI)
                .volume(255)
                .noteNumber(88)
                .duration(percDuration);

            _tempo = tempo;
            this->data = data;
            this->length = length;
            this->position = 0;
            Schedule::after<timerId>(
                0,
                &SimpleTuneSource::play,
                *this);
        }

    private:
        void play(){
            bool isBass;
            bool isPercussion;
            do{
                if(position >= length)
                    return;
                auto noteNumber = data[position++];
                isPercussion = noteNumber == 90;
                if(isPercussion){
                    _percussion[
                        data[position++]%4
                        ].play<channel>();
                }else{
                    isBass = noteNumber >> 7;
                    noteNumber &= 0x7F;

                    signed char noteDuration = data[position++];
                    uint32_t duration = _tempo;

                    if( noteDuration < 0 )
                        duration /= -noteDuration;
                    else if(noteDuration > 0)
                        duration *= noteDuration;

                    if(!isBass){
                        if(noteNumber <= 88){
                            _note.duration(duration * (PROJ_AUD_FREQ / 1000))
                                .noteNumber(noteNumber)
                                .play<channel>();
                        }

                        Schedule::after<timerId>(
                            duration,
                            &SimpleTuneSource::play,
                            *this);
                    }else if(noteNumber <= 88){
                        _bass.duration(duration * (PROJ_AUD_FREQ / 1000))
                            .noteNumber(noteNumber)
                            .play<channel>();
                    }
                }
            }while(isBass || isPercussion);
        }
    };

    namespace internal {
        template<unsigned  count,
                 template<unsigned...> class  meta_functor, unsigned...  indices>
        struct  apply_range
        {
            typedef  typename apply_range<count-1, meta_functor, count-1, indices...>::result  result;
        };

        template<template<unsigned...> class  meta_functor, unsigned...  indices>
        struct  apply_range<0, meta_functor, indices...>
        {
            typedef  typename meta_functor<indices...>::result  result;
        };

        template<typename lambda_str_type>
        struct count_elements
        {
            template<unsigned... indices>
            struct produce {
                struct result {
                    static const u32 value = ((lambda_str_type{}.chars[indices] == ',') + ...);
                };
            };
        };

        constexpr u32 countWhitespace(const char *str){
            u32 c = 0;
            while(str[c] && str[c] <= 32)
                c++;
            return c;
        }

        struct TuneData {
            u8 _note;
            u8 _duration;

            constexpr TuneData(u8 note, u8 duration) :
                _note(note),
                _duration(duration) {}

            constexpr u8 note() const { return _note; }
            constexpr u32 duration() const { return _duration * 128U; }
        };

        template<typename Array>
        constexpr auto genSimpleTune(const char *str){
            constexpr const u8 noteIndex[] = {
                12, // a
                14, // b
                3, // c
                5, // d
                7, // e
                8, // f
                10 // g
            };

            u32 pos = 0;

            struct Extended : Array {
                u32 _tempo = 4 * 60000 / 120;
                constexpr auto& tempo(u32 tempo){
                    _tempo = 4 * 60000 / tempo;
                    return *this;
                }

                constexpr u32 tempo() const {
                    return _tempo;
                }
            } array = {};

            for(u32 i=0; str[i];){
                u32 octave = 4;
                u32 note = 0;
                s8 duration = 0;
                bool isBass = false;
                bool isPercussion = false;

                i += countWhitespace(str+i);
                switch(str[i]){
                case 'K': // kick
                    isPercussion = true;
                    note = 90;
                    duration = 0;
                    break;
                case 'S': // snare
                    isPercussion = true;
                    note = 90;
                    duration = 1;
                    break;
                case 'H': // hat
                    isPercussion = true;
                    note = 90;
                    duration = 2;
                    break;
                case 'R': // reserved
                    isPercussion = true;
                    note = 90;
                    duration = 3;
                    break;
                }

                if(isPercussion){
                    i++;
                }else{
                    if(str[i] == '!'){
                        isBass = true;
                        i++;
                    }

                    i += countWhitespace(str+i);
                    if(str[i] >= 'A' && str[i] <= 'G'){
                        note = noteIndex[ str[i++] - 'A' ];

                        i += countWhitespace(str+i);
                        if(str[i] == '#'){
                            note++;
                            i++;
                        } else if(str[i] == '-')
                            i++;

                        i += countWhitespace(str+i);
                        if(str[i] >= '0' && str[i] <= '9')
                            octave = str[i++] - '0';
                        note = (note + octave * 12) - 14;
                        if(isBass)
                            note |= 0x80;
                    } else {
                        note = 89;
                        i++;
                    }

                    i += countWhitespace(str+i);
                    if(str[i] == '/'){
                        i++;
                        i += countWhitespace(str+i);
                        while(str[i] >= '0' && str[i] <= '9')
                            duration = (duration * 10) + str[i++] - '0';
                        duration = -duration;
                    } else if( str[i] == '*' ){
                        i++;
                        i += countWhitespace(str+i);
                        while(str[i] >= '0' && str[i] <= '9')
                            duration = (duration * 10) + str[i++] - '0';
                    }
                }

                array[pos++] = note;
                array[pos++] = duration;

                i += countWhitespace(str+i);
                if(str[i] == ',') i++;
            }
            return array;
        }

        template <u32 channel, u32 timerId>
        inline SimpleTuneSource<channel, timerId> simpleTuneSource;
    }

    template<u32 channel=0, u32 timerId=~0U - channel, typename TuneType>
    auto& play(const TuneType& tune){
        auto& source = internal::simpleTuneSource<channel, timerId>;
        source.setup(tune.tempo(), &tune[0], tune.size());
        return source;
    }


}  // Audio
