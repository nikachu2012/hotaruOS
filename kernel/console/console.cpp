#include "console.hpp"

Console::Console(PixelWriter &writer, const PixelTrueColor &textColor, const PixelTrueColor &bgColor)
    : writer_{writer}, textColor_{textColor}, bgColor_{bgColor}, buf_{}, cursorRow_{0}, cursorColumn_{0}
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
            writeChar(writer_, s_startPosX + cursorRow_ * FONT_WIDTH, s_startPosY + cursorColumn_ * FONT_HEIGHT, s[index], textColor_);
            buf_[cursorColumn_][cursorRow_] = s[index];
            cursorRow_++;
        }
        index++;
    }
    Console::lineBreak();
}

void Console::lineBreak()
{
    buf_[cursorRow_][cursorColumn_] = '\n';
    cursorRow_ = 0;

    if (cursorColumn_ < s_consoleColumn - 1)
    {
        cursorColumn_++;
    }
    else
    {
        serialPutString("max column \r\n", PORT_COM1);

        // バッファメモリをコピーして先頭に移動
        memcpy(buf_, &buf_[1], s_consoleRow * (s_consoleColumn - 1));
        // 空いた最下行をヌル文字で埋める
        memset(buf_[s_consoleColumn - 1], 0, s_consoleRow + 1);

        // 画面をきれいにする
        for (size_t y = 0; y < s_startPosY + s_consoleColumn * FONT_HEIGHT; y++)
        {
            for (size_t x = 0; x < s_startPosX + s_consoleRow * FONT_WIDTH; x++)
            {
                writer_.write(x, y, bgColor_);
            }
        }

        // 再描画
        for (size_t y = 0; y < s_consoleColumn - 1; y++)
        {
            /* code */
            writeString(writer_, s_startPosX, s_startPosY + y * FONT_HEIGHT, buf_[y], textColor_);
        }
    }
}
