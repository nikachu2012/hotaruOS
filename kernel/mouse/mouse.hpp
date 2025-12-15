#pragma once

#include "cursor.hpp"
#include <cstdint>
namespace MousePS2
{
constexpr uint16_t STATUS_REGISTER = 0x64;
constexpr uint16_t IO_REGISTER = 0x60;
constexpr uint8_t ACK = 0xfa;

/**
 * @brief マウスが動いたか確認し動いていたらカーソルを移動させます
 */
void process(MouseCursor &c);

/**
 * @brief マウスをリセットします
 */
void reset();

void setSampleRate();

} // namespace MousePS2
