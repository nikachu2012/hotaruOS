#pragma once

#include "../graphics/pixelWriter.hpp"
#include "cursorImage.hpp"

class MouseCursor
{
  private:
    PixelWriter *pixelwriter_;
    PixelTrueColor erasecolor_;
    /**
     * @brief カーソルの位置
     */
    int x_, y_;

  public:
    MouseCursor(PixelWriter *writer_, PixelTrueColor erasecolor_, int initX, int initY);

    void moveAbsolute(int x, int y);
    void moveRelative(int dx, int dy);
};
