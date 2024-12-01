#pragma once

#include <cstdint>
#include <cstddef>

#define PORT_COM1 0x3f8 // COM1
#define PORT_COM2 0x2f8 // COM2

/**
 * 指定されたIOポートに8bit値を出力
 */
void outb(uint16_t port, uint8_t data);

/**
 * 指定されたIOポートから8bit値を入力
 */
uint8_t inb(uint16_t port);

/**
 * シリアルポートの準備
 */
int serialInit(uint16_t port);

/**
 * 送信バッファが空かどうかを返す
 * (Line status register(PORT+5)の5ビット目)
 */
int isTransmitEmpty(const uint16_t port);

/**
 * シリアルポートに1バイト書き込む
 */
void serialPutChar(const char data, const uint16_t port);

/**
 * シリアルポートに文字列をヌル文字に達するまで書き込む
 */
int serialPutString(const char *str, const uint16_t port);

/**
 * 読み取れるデータがないか確認
 * (Line status register(PORT+5)の0ビット目)
 */
int serialReceived(const uint16_t port);

/**
 * シリアルポートから1バイト読み取る
 */
char serialGetChar(const uint16_t port);
