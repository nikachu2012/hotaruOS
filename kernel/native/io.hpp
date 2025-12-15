#pragma once

#include <stdint.h>

// Intel記法を推進しています

/**
 * @brief 指定されたIOポートに8bit値を出力 (out binary)
 *
 * @param port ポート
 * @param data 出力するデータ
 */
void outb(uint16_t port, uint8_t data);

/**
 * @brief 指定されたIOポートに16bit値を出力 (out word)
 *
 * @param port ポート
 * @param data 出力するデータ
 */
void outw(uint16_t port, uint16_t data);

/**
 * @brief 指定されたIOポートに32bit値を出力 (out double word)
 *
 * @param port ポート
 * @param data 出力するデータ
 */
void outdw(uint16_t port, uint32_t data);
/**
 * @brief 指定されたIOポートから8bit値を入力 (in byte)
 *
 * @param port ポート
 * @return uint8_t 入力されたデータ
 */
uint8_t inb(uint16_t port);

/**
 * @brief 指定されたIOポートから16bit値を入力 (in word)
 *
 * @param port ポート
 * @return uint16_t 入力されたデータ
 */
uint16_t inw(uint16_t port);

/**
 * @brief 指定されたIOポートから32bit値を入力 (in double word)
 *
 * @param port ポート
 * @return uint32_t 入力されたデータ
 */
uint32_t indw(uint16_t port);

/**
 * @brief 1ms~4msほど待機
 */
void io_wait(void);
