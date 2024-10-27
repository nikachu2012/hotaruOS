extern "C" void kernelMain()
{
    while (true)
    {
        __asm__("hlt");
    }
}
