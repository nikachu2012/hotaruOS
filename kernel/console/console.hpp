#pragma once

#include <cstring>

#include "../graphics/pixelWriter.hpp"
#include "../font/font.hpp"
#include "../serial/serial.hpp"

class Console
{
public:
    static const int s_consoleRow = 80, s_consoleColumn = 25;
    static const int s_startPosX = 100, s_startPosY = 100;
    Console(PixelWriter &writer, const PixelTrueColor &textColor, const PixelTrueColor &bgColor);
    void puts(const char *s);

private:
    void lineBreak();

    PixelWriter &writer_;
    const PixelTrueColor m_textColor, m_bgColor;
    char m_buf[s_consoleColumn][s_consoleRow + 1];
    int m_cursorRow, m_cursorColumn;
};
