section .text

global LoadIDT
global GetCodeSegment

LoadIDT: ; void LoadIDT(IDTR *idtr);
    lidt [rdi]   ; IDTをロード
    ret

GetCodeSegment:
    xor eax, eax ; raxの下位32ビットを0埋め
    mov ax, cs
    ret

