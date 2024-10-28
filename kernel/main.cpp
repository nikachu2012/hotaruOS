#include <cstdint>

extern "C" void kernelMain(uint64_t framebufferBase, uint64_t framebufferSize)
{
    uint8_t *framebuffer = reinterpret_cast<uint8_t *>(framebufferBase);
    for (uint64_t i = 0; i < framebufferSize; i++)
    {
        // index | 0 | 1 | 2 | 3  | 4 | 5 | 6 | 7  | ... (Re = reserved)
        // data  | B | G | R | Re | B | G | R | Re | ...
        framebuffer[i] = i % 4 == 1 ? 0xff : 0x00;
    }

    while (true)
    {
        __asm__("hlt");
    }
}
