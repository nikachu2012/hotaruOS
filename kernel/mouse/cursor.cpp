#include "cursor.hpp"

constexpr void drawCursor(PixelWriter *writer, int x, int y)
{
    writer->drawImageRGBA(x, y, cursorData, cursorDataWidth, cursorDataHeight);
}

MouseCursor::MouseCursor(PixelWriter *writer, PixelTrueColor eraseColor, int initX, int initY)
    : pixelwriter_{writer}, erasecolor_{eraseColor}, x_{initX}, y_{initY}
{
    drawCursor(pixelwriter_, x_, y_);
}

void MouseCursor::moveAbsolute(int x, int y)
{
    // erasecolorで塗りつぶしてカーソルを消す
    pixelwriter_->drawRectWithFill(x_, y_, cursorDataWidth, cursorDataHeight, erasecolor_);

    // カーソル位置の書き換え
    x_ = x;
    y_ = y;

    if (x < 0)
        x_ = 0;
    if (y > pixelwriter_->getDisplayWidth())
        x_ = pixelwriter_->getDisplayWidth();

    if (y < 0 )
        y_ = 0;
    if (y > pixelwriter_->getDisplayHeight())
        y_ = pixelwriter_->getDisplayHeight();

    // 新規位置にカーソルの描画
    drawCursor(pixelwriter_, x_, y_);
}

void MouseCursor::moveRelative(int dx, int dy)
{
    // erasecolorで塗りつぶしてカーソルを消す
    pixelwriter_->drawRectWithFill(x_, y_, cursorDataWidth, cursorDataHeight, erasecolor_);

    // カーソル位置の書き換え
    x_ = x_ + dx;
    y_ = y_ + dy;

    if (x_ < 0)
        x_ = 0;
    if (x_ > pixelwriter_->getDisplayWidth())
        x_ = pixelwriter_->getDisplayWidth();

    if (y_ < 0 )
        y_ = 0;
    if (y_ > pixelwriter_->getDisplayHeight())
        y_ = pixelwriter_->getDisplayHeight();

    // 新規位置にカーソルの描画
    drawCursor(pixelwriter_, x_, y_);
}
