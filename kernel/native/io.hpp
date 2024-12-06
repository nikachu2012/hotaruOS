#pragma once

#include <stdint.h>

// Intel記法を推進しています

/**
 * @brief 指定されたIOポートに8bit値を出力 (out binary)
 *
 * @param port ポート
 * @param data 出力するデータ
 */
inline void outb(uint16_t port, uint8_t data)
{
    // out %al, %dx
    // al -> dx
    asm("out %b0, %w1"
        :
        : "a"(data), "d"(port));
}

/**
 * @brief 指定されたIOポートに16bit値を出力 (out word)
 *
 * @param port ポート
 * @param data 出力するデータ
 */
inline void outw(uint16_t port, uint16_t data)
{
    // out %ax, %dx
    // ax -> dx
    asm("out %w0, %w1"
        :
        : "a"(data), "d"(port));
}

/**
 * @brief 指定されたIOポートに32bit値を出力 (out double word)
 *
 * @param port ポート
 * @param data 出力するデータ
 */
inline void outdw(uint16_t port, uint32_t data)
{
    // out %eax, %dx
    // eax -> dx
    asm("out %k0, %w1"
        :
        : "a"(data), "d"(port));
}

/**
 * @brief 指定されたIOポートから8bit値を入力 (in byte)
 *
 * @param port ポート
 * @return uint8_t 入力されたデータ
 */
inline uint8_t inb(uint16_t port)
{
    uint8_t data;

    // in %dx, %al
    // dx -> al
    asm("in %w1, %b0"
        : "=a"(data)
        : "d"(port));

    return data;
}

/**
 * @brief 指定されたIOポートから16bit値を入力 (in word)
 *
 * @param port ポート
 * @return uint16_t 入力されたデータ
 */
inline uint16_t inw(uint16_t port)
{
    uint16_t data;

    // in %dx, %ax
    // dx -> ax
    asm("in %w1, %w0"
        : "=a"(data)
        : "d"(port));

    return data;
}

/**
 * @brief 指定されたIOポートから32bit値を入力 (in double word)
 *
 * @param port ポート
 * @return uint32_t 入力されたデータ
 */
inline uint32_t indw(uint16_t port)
{
    uint32_t data;

    // in %dx, %eax
    // dx -> eax
    asm("in %w1, %k0"
        : "=a"(data)
        : "d"(port));

    return data;
}
