#include "pci.hpp"

PciError Pci::scanAllBus()
{
    m_deviceCount = 0;

    for (int busIndex = 0; busIndex < 256; busIndex++)
    {
        for (int deviceIndex = 0; deviceIndex < 32; deviceIndex++)
        {
            for (int functionIndex = 0; functionIndex < 8; functionIndex++)
            {
                outdw(PORT_PCI_CONFIG_ADDRESS, makeAddress(busIndex, deviceIndex, functionIndex, 0x00));
                uint32_t pciConfig_0 = indw(PORT_PCI_CONFIG_DATA);

                if ((pciConfig_0 & 0xffffu) == 0xffffu)
                {
                    // Vendor ID == 0xffffならデバイスが存在しない
                    continue;
                }
                else
                {
                    if (m_deviceCount >= 32)
                    {
                        return PciError::Pci_FullArray;
                    }

                    uint16_t deviceID = (pciConfig_0 & 0xffff0000u) >> 16;
                    uint16_t vendorID = pciConfig_0 & 0x0000ffffu;

                    // Header Typeを取得する
                    outdw(PORT_PCI_CONFIG_ADDRESS, makeAddress(busIndex, deviceIndex, functionIndex, 4 * 3));
                    uint8_t headerType = (indw(PORT_PCI_CONFIG_DATA) & 0x00ff0000u) >> 16;

                    // Base / Sub Class と Interfaceを取得する
                    outdw(PORT_PCI_CONFIG_ADDRESS, makeAddress(busIndex, deviceIndex, functionIndex, 4 * 2));
                    uint32_t pciConfig_8 = indw(PORT_PCI_CONFIG_DATA);

                    uint8_t baseClass = (pciConfig_8 & 0xff000000u) >> 24;
                    uint8_t subClass = (pciConfig_8 & 0x00ff0000u) >> 16;
                    uint8_t interface = (pciConfig_8 & 0x0000ff00u) >> 8;

                    m_devices[m_deviceCount] = {static_cast<uint8_t>(busIndex),
                                                static_cast<uint8_t>(deviceIndex),
                                                static_cast<uint8_t>(functionIndex),
                                                deviceID,
                                                vendorID,
                                                headerType,
                                                {baseClass, subClass, interface}};

                    m_deviceCount++;
                }
            }
        }
    }

    return PciError::Pci_Success;
}
