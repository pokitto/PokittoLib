#pragma once

#include <Pokitto.h>
#include <type_traits>

namespace Schedule {

    using u32 = std::uint32_t;

    namespace internal {
        struct EventData {
            void *data;
            void *callback;
            void (*wrapper)(void *, void*);
            void (*nextHook)(bool);
            u32 interval;
            u32 startTime;
        };

        inline u32 pauseTime = 0;
        inline u32 timerCount = 0;
        inline u32 resumeCount = 0;

        inline void updateHook(EventData &d, bool update){
            u32 now = Pokitto::Core::getTime();

            if(internal::pauseTime){
                if(internal::resumeCount){
                    d.startTime += now - internal::pauseTime;
                    internal::resumeCount--;
                    if(!internal::resumeCount){
                        internal::pauseTime = 0;
                    }
                }else return;
            }

            if(d.callback){
                u32 delta = now - d.startTime;
                if(delta >= d.interval){
                    if(d.interval)
                        delta %= d.interval;
                    d.startTime = now - delta;
                    d.wrapper(d.callback, d.data);
                }
            }
            d.nextHook(update);
        }

        template <u32 id>
        EventData& getEventData(){
            static EventData d;
            static bool wasInit = false;
            if(!wasInit){
                wasInit = true;
                d.nextHook = nullptr;
                d.callback = nullptr;
            }
            return d;
        }

        template<u32 id>
        void _updateHook(bool update){
            updateHook(getEventData<id>(), update);
        }

    }

    inline void pause(){
        if(internal::pauseTime)
            return;
        internal::pauseTime = Pokitto::Core::getTime();
        internal::resumeCount = 0;
    }

    inline void resume(){
        if(!internal::pauseTime)
            return;
        internal::resumeCount = internal::timerCount;
        if(internal::timerCount == 0)
            internal::pauseTime = 0;
    }

    template<u32 id>
    void cancel(){
        internal::getEventData<id>().callback = nullptr;
    }

    template<u32 id, typename EventData, typename EventData2>
    void repeat(u32 interval, void (*cb)(EventData), EventData2& data, bool once){
        using CBType = decltype(cb);

        auto& d = internal::getEventData<id>();
        d.startTime = Pokitto::Core::getTime();
        d.interval = interval;
        d.callback = reinterpret_cast<void*>(cb);

        if constexpr (!std::is_reference<EventData>::value && sizeof(data) == sizeof(void*)){
            d.data = reinterpret_cast<void*>(data);
            if(once){
                d.wrapper = +[](void *vcb, void *vdata){
                    cancel<id>();
                    reinterpret_cast<CBType>(vcb)(*reinterpret_cast<EventData2>(vdata));
                };
            }else{
                d.wrapper = +[](void *vcb, void *vdata){
                    reinterpret_cast<CBType>(vcb)(*reinterpret_cast<EventData2>(vdata));
                };
            }
        } else {
            d.data = const_cast<void*>(reinterpret_cast<const void*>(&data));
            if(once){
                d.wrapper = +[](void *vcb, void *vdata){
                    cancel<id>();
                    reinterpret_cast<CBType>(vcb)(*reinterpret_cast<EventData2*>(vdata));
                };
            }else{
                d.wrapper = +[](void *vcb, void *vdata){
                    reinterpret_cast<CBType>(vcb)(*reinterpret_cast<EventData2*>(vdata));
                };
            }
        }

        if(!d.nextHook){
            d.nextHook = Pokitto::Core::updateHook;
            Pokitto::Core::updateHook = internal::_updateHook<id>;
            internal::timerCount++;
        }
    }

    template<u32 id, typename EventData, typename EventData2>
    void repeat(u32 interval, bool (*cb)(EventData), EventData2& data){
        using CBType = decltype(cb);

        auto& d = internal::getEventData<id>();
        d.startTime = Pokitto::Core::getTime();
        d.interval = interval;
        d.callback = reinterpret_cast<void*>(cb);
        if constexpr (!std::is_reference<EventData>::value && sizeof(data) == sizeof(void*)){
            d.data = reinterpret_cast<void*>(data);
            d.wrapper = +[](void *vcb, void *vdata){
                bool r = reinterpret_cast<CBType>(vcb)(*reinterpret_cast<EventData2>(vdata));
                if(!r)
                    cancel<id>();
            };
        } else {
            d.data = const_cast<void*>(reinterpret_cast<const void*>(&data));
            d.wrapper = +[](void *vcb, void *vdata){
                bool r = reinterpret_cast<CBType>(vcb)(*reinterpret_cast<EventData2*>(vdata));
                if(!r)
                    cancel<id>();
            };
        }

        if(!d.nextHook){
            d.nextHook = Pokitto::Core::updateHook;
            Pokitto::Core::updateHook = internal::_updateHook<id>;
            internal::timerCount++;
        }
    }

    template<u32 id, typename EventData, typename EventData2>
    void repeat(u32 interval, void (*cb)(const EventData&), EventData2& data){
        repeat<id>(interval, cb, data, false);
    }

    template<u32 id>
    void repeat(u32 interval, void (*cb)()){
        using CB = decltype(cb);
        repeat<id>(interval, +[](CB cb){ cb(); }, cb, false);
    }

    template<u32 id>
    void repeat(u32 interval, bool (*cb)()){
        using CB = decltype(cb);
        repeat<id>(interval, +[](CB cb){ return cb(); }, cb);
    }

    template<u32 id, typename EventData>
    void repeat(u32 interval, void (EventData::*cb)(), EventData& data){
        static decltype(cb) scb;
        scb = cb;
        repeat<id>(interval, +[](EventData &obj){
            (obj.*scb)();
        }, data, false);
    }

    template<u32 id>
    void after(u32 interval, void (*cb)()){
        using CB = decltype(cb);
        repeat<id>(interval, +[](CB cb){ cb(); }, cb, true);
    }

    template<u32 id, typename EventData, typename EventData2>
    void after(u32 interval, void (*cb)(const EventData&), EventData2& data){
        repeat<id>(interval, cb, data, true);
    }

    template<u32 id, typename EventData>
    void after(u32 interval, void (EventData::*cb)(), EventData& data){
        static decltype(cb) scb;
        scb = cb;
        repeat<id>(interval,
                   +[](EventData &obj){
                        (obj.*scb)();
                    },
                   data,
                   true);
    }


    template<u32 id, typename EventData, typename EventData2>
    void repeat(u32 interval, bool (*cb)(const EventData&), EventData2& data){
        repeat<id>(interval, cb, data, false);
    }

    template<u32 id, typename EventData>
    void repeat(u32 interval, bool (EventData::*cb)(), EventData& data){
        static decltype(cb) scb;
        scb = cb;
        repeat<id>(interval, +[](EventData &obj){
            return (obj.*scb)();
        }, data);
    }
}
