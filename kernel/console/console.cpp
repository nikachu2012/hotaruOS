#include "console.hpp"

Console::Console(PixelWriter &writer, const PixelTrueColor &textColor, const PixelTrueColor &bgColor, const int x, const int y)
    : writer_{writer}, m_textColor{textColor}, m_bgColor{bgColor}, m_buf{}, m_cursorRow{0}, m_cursorColumn{0}, m_startPosX{x}, m_startPosY{y}
{
}

void Console::puts(const char *s)
{
    int index = 0;
    while (s[index] != '\0')
    {
        if (s[index] == '\n')
        {
            Console::lineBreak();
        }
        else
        {
            writeChar(writer_, m_startPosX + m_cursorRow * FONT_WIDTH, m_startPosY + m_cursorColumn * FONT_HEIGHT, s[index], m_textColor);
            m_buf[m_cursorColumn][m_cursorRow] = s[index];
            m_cursorRow++;
        }
        index++;
    }
    Console::lineBreak();
}

void Console::lineBreak()
{
    m_buf[m_cursorRow][m_cursorColumn] = '\n';
    m_cursorRow = 0;

    if (m_cursorColumn < s_consoleColumn - 1)
    {
        m_cursorColumn++;
    }
    else
    {
        // バッファメモリをコピーして先頭に移動
        memcpy(m_buf, &m_buf[1], s_consoleRow * (s_consoleColumn - 1));
        // 空いた最下行をヌル文字で埋める
        memset(m_buf[s_consoleColumn - 1], 0, s_consoleRow + 1);

        // 画面をきれいにする
        writer_.drawRectWithFill(m_startPosX, m_startPosY, s_consoleRow * FONT_WIDTH, s_consoleColumn * FONT_HEIGHT, m_bgColor);

        // 再描画
        for (size_t y = 0; y < s_consoleColumn - 1; y++)
        {
            /* code */
            writeString(writer_, m_startPosX, m_startPosY + y * FONT_HEIGHT, m_buf[y], m_textColor);
        }
    }
}
