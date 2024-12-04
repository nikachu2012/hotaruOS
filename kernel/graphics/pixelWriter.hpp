#pragma once

#include <cstddef>
#include "frameBufferConfig.hpp"

/**
 * @brief 24ビットカラーの色を扱う
 */
struct PixelTrueColor
{
    uint8_t r, g, b;
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
    virtual void writePixel(int x, int y, const PixelTrueColor &c) = 0;
    virtual void writeRect(int x, int y, int width, int height, const PixelTrueColor &c) = 0;

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
    void writePixel(int x, int y, const PixelTrueColor &c) override;
    void writeRect(int x, int y, int width, int height, const PixelTrueColor &c) override;
};

/**
 * @brief BGRResv8Bit用の画面書き換え用クラス
 */
class BGRResv8BitPerColorPixelWriter : public PixelWriter
{
public:
    using PixelWriter::PixelWriter;
    void writePixel(int x, int y, const PixelTrueColor &c) override;
    void writeRect(int x, int y, int width, int height, const PixelTrueColor &c) override;
};
