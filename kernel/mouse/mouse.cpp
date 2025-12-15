#include "mouse.hpp"
#include "../native/io.hpp"
#include "cursor.hpp"
#include <cstdint>

int printk(const char *c, ...);

void MousePS2::process(MouseCursor &c)
{
    // マウスのデータが来ているかをチェック
    uint8_t s = inb(STATUS_REGISTER);

    if (!(s & 0b1))
        return;

    if (!(s & 0x20))
    {
        // キーボード入力の時はバッファを破棄
        while (inb(STATUS_REGISTER) & 0b1)
            inb(IO_REGISTER);
        return;
    }

    // マウスのデータが来ている場合
    uint8_t flg = inb(IO_REGISTER);
    uint8_t xMove = inb(IO_REGISTER);
    uint8_t yMove = inb(IO_REGISTER);

    // 符号拡張
    int dx = flg & 0b0001'0000 ? (int)(0xFFFF'FF00 | xMove) : (int)(xMove);
    // y座標は画面上から下へ増えていくので差分の符号を逆にする
    int dy = flg & 0b0010'0000 ? -(int)(0xFFFF'FF00 | yMove) : -(int)(yMove);

    if (flg & 0b00000001)
        printk("clicked LEFT button");
    if (flg & 0b00000010)
        printk("clicked RIGHT button");
    if (flg & 0b00000100)
        printk("clicked MIDDLE button");

    c.moveRelative(dx, dy);
}

inline void waitToWrite()
{
    while (inb(MousePS2::STATUS_REGISTER) & 0b10)
        ;
}

inline void waitToRead()
{
    while (!(inb(MousePS2::STATUS_REGISTER) & 0b1))
        ;
}

inline void sendToMouse(uint8_t d)
{
    // wait allowed to send
    waitToWrite();
    // send to mouse
    outb(MousePS2::STATUS_REGISTER, 0xd4);

    // wait allowed to send
    waitToWrite();
    // send
    outb(MousePS2::IO_REGISTER, d);
}

void MousePS2::reset()
{
    printk("Resetting mouse...");
    // mouse reset
    sendToMouse(0xff);
    // wait recv 0xaa
    while (inb(IO_REGISTER) != 0xaa)
        ;

    // Get Compaq Status Byte
    waitToWrite();
    // send to mouse
    outb(MousePS2::STATUS_REGISTER, 0x20);

    waitToRead();
    auto statusByte = inb(IO_REGISTER);
    statusByte |= 0b10;
    statusByte &= ~(1 << 5);

    // Set Compaq Status
    waitToWrite();
    outb(MousePS2::STATUS_REGISTER, 0x60);
    waitToWrite();
    outb(MousePS2::IO_REGISTER, statusByte);

    printk("Enabling packet streaming...");
    // enable packet streaming
    sendToMouse(0xf4);
    printk("Waiting ACK...");

    // wait ACK
    waitToWrite();
    while (inb(IO_REGISTER) != ACK)
        ;

    return;
}

void MousePS2::setSampleRate()
{
    sendToMouse(0xF3);
    // wait ACK
    waitToWrite();
    while (inb(IO_REGISTER) != ACK)
        ;

    // set sample rate is 40
    sendToMouse(40);
    // wait ACK
    waitToWrite();
    while (inb(IO_REGISTER) != ACK)
        ;

    return;
}
