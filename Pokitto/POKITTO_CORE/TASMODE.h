#pragma once

#include <cstdint>

namespace TAS {

    using LineFiller = void (*)(std::uint8_t *line, std::uint32_t y, bool skip);

    void BGTileFiller(std::uint8_t *line, std::uint32_t y, bool skip);
    void SpriteFiller(std::uint8_t *line, std::uint32_t y, bool skip);
    void ColorFiller(std::uint8_t* line, std::uint32_t y, bool skip);
    void DitherFiller(std::uint8_t* line, std::uint32_t y, bool skip);
}
