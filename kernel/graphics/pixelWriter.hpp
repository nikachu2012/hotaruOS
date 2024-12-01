#pragma once

#include "frameBufferConfig.hpp"

/**
 * @brief 24ビットカラーの色を扱う
 */
struct PixelTrueColor
{
    uint8_t r, g, b;
};

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
    virtual void write(int x, int y, const PixelTrueColor &c) = 0;

protected:
    /**
     * @brief 座標から実際のメモリ上のアドレスを計算
     *
     * @param x x座標
     * @param y y座標
     * @return uint8_t* 座標に対応するフレームバッファ上のアドレス
     */
    virtual uint8_t *getPixelIndex(int x, int y) = 0;
    const frameBufferConfig &config_;
};

/**
 * @brief RGBResv8Bit用の画面書き換え用クラス
 */
class RGBResv8BitPerColorPixelWriter : public PixelWriter
{
public:
    using PixelWriter::PixelWriter;
    void write(int x, int y, const PixelTrueColor &c) override;

protected:
    uint8_t *getPixelIndex(int x, int y) override;
};

/**
 * @brief BGRResv8Bit用の画面書き換え用クラス
 */
class BGRResv8BitPerColorPixelWriter : public PixelWriter
{
public:
    using PixelWriter::PixelWriter;
    void write(int x, int y, const PixelTrueColor &c) override;

protected:
    uint8_t *getPixelIndex(int x, int y) override;
};
