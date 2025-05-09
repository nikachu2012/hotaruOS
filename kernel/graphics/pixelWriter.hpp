#pragma once

#include <cstddef>
#include "frameBufferConfig.hpp"

/**
 * @brief 24ビットカラーを扱う
 */
struct PixelTrueColor
{
    uint8_t r, g, b;
};

/**
 * @brief アルファチャンネル付き24ビットカラーを扱う
 */
struct PixelRGBA
{
    uint8_t r, g, b, a;
};

/**
 * @brief RGBResv8BitColorで１ピクセル分の色データの作成
 *
 * FORMAT
 * MSB       LSB
 * |Re|B |G |R |
 * |8 |8 |8 |8 |(bit)

 *
 * @param c
 * @return * constexpr uint32_t
 */
constexpr uint32_t RGBResv8BitColorGen(const PixelTrueColor &c);

/**
 * @brief BGRResv8BitColorで１ピクセル分の色データの作成
 *
 * FORMAT
 * MSB       LSB
 * |Re|R |G |B |
 * |8 |8 |8 |8 |(bit)
 *
 * @param c
 * @return * constexpr uint32_t
 */
constexpr uint32_t BGRResv8BitColorGen(const PixelTrueColor &c);

/**
 * @brief 画面の書き換えを行う基底クラス
 */
class PixelWriter
{
public:
    /**
     * @brief Construct a new Pixel Writer object
     *
     * @param config
     */
    PixelWriter(const frameBufferConfig &config) : config_{config} {}

    /**
     * @brief Destroy the Pixel Writer object
     *
     */
    virtual ~PixelWriter() = default;

    /**
     * @brief 指定座標の色を書き換える
     *
     * @param x x座標
     * @param y y座標
     * @param c 色
     */
    virtual void drawPixel(int x, int y, const PixelTrueColor &c) = 0;

    /**
     * @brief 枠線のみの四角形を書く関数
     *
     * @param x 四角形左上のx座標
     * @param y 四角形左上のy座標
     * @param width 四角形の横幅
     * @param height 四角形の縦幅
     * @param c 色
     */
    virtual void drawRect(int x, int y, int width, int height, const PixelTrueColor &c) = 0;

    /**
     * @brief 中が埋められた四角形を書く関数
     *
     * @param x 四角形左上のx座標
     * @param y 四角形左上のy座標
     * @param width 四角形の横幅
     * @param height 四角形の縦幅
     * @param c 色
     */
    virtual void drawRectWithFill(int x, int y, int width, int height, const PixelTrueColor &c) = 0;

    /**
     * @brief RGBAの画像を出力する関数
     *
     * @param x 出力する座標
     * @param y 出力する座標
     * @param i 画像ピクセルの１次元配列
     * @param width 画像の横幅
     * @param height 画像の縦幅
     */
    virtual void drawImageRGBA(int x, int y, const PixelRGBA *i, int width, int height) = 0;

protected:
    const frameBufferConfig &config_;
};

/**
 * @brief RGBResv8Bit用の画面書き換え用クラス
 */
class RGBResv8BitPerColorPixelWriter : public PixelWriter
{
public:
    using PixelWriter::PixelWriter;
    void drawPixel(int x, int y, const PixelTrueColor &c) override;
    void drawRect(int x, int y, int width, int height, const PixelTrueColor &c) override;
    void drawRectWithFill(int x, int y, int width, int height, const PixelTrueColor &c) override;
    void drawImageRGBA(int x, int y, const PixelRGBA *i, int width, int height) override;
};

/**
 * @brief BGRResv8Bit用の画面書き換え用クラス
 */
class BGRResv8BitPerColorPixelWriter : public PixelWriter
{
public:
    using PixelWriter::PixelWriter;
    void drawPixel(int x, int y, const PixelTrueColor &c) override;
    void drawRect(int x, int y, int width, int height, const PixelTrueColor &c) override;
    void drawRectWithFill(int x, int y, int width, int height, const PixelTrueColor &c) override;
    void drawImageRGBA(int x, int y, const PixelRGBA *i, int width, int height) override;
};
