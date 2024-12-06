#include "pci.hpp"

/**
 * @brief PCIのCONFIG_ADDRESSを生成します
 *
 * @param bus バス番号
 * @param device デバイス番号
 * @param funct ファンクション番号
 * @param offset レジスタオフセット
 * @return uint32_t
 */
uint32_t makeAddress(uint8_t bus, uint8_t device, uint8_t funct, uint8_t offset)
{
    uint32_t addr = 0x80000000 | (bus & 0xff) << 16 | (device & 0x1f) << 11 | (funct & 0b111) << 8 | (offset & 0xff);

    return addr;
}
