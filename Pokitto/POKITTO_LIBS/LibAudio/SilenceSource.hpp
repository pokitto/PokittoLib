#pragma once

namespace Audio {

template<u32 channel = 0>
void stop(){
    auto copy =
        +[](u8 *buffer, void *ptr){
             MemOps::set(buffer, 127, 512);
         };

    auto mix = +[](u8 *buffer, void *ptr){};
    
    Audio::connect(
        channel,
        nullptr,
        channel == 0 ? copy : mix
        );
}

}
