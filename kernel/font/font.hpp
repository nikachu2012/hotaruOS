#pragma once

#include <cstdint>
#include <cstddef>
#include "../graphics/pixelWriter.hpp"

#define FONT_WIDTH 7
#define FONT_HEIGHT 13

/**
 * @brief 指定文字のフォントを取得
 *
 * @param c 文字
 * @return const uint8_t* フォントデータ (FONT_WIDTH x FONT_HEIGHTサイズ)
 */
const uint8_t *getFont(char c);

/**
 * @brief 1文字を指定座標に出力
 *
 * @param writer 出力用クラス
 * @param x x座標(左上を0,0とする)
 * @param y y座標
 * @param c 出力する文字
 * @param color 24ビットカラー
 */
void writeChar(PixelWriter &writer, int x, int y, char c, const PixelTrueColor &color);

/**
 * @brief 文字列を指定座標に出力
 *
 * @param writer
 * @param x
 * @param y
 * @param s
 * @param color
 */
void writeString(PixelWriter &writer, int x, int y, const char *s, const PixelTrueColor &color);
