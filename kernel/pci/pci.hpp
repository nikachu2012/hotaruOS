#pragma once

#include <cstdint>
#include <array>

#include "pciError.hpp"
#include "../native/io.hpp"

constexpr uint16_t PORT_PCI_CONFIG_ADDRESS = 0x0cf8;
constexpr uint16_t PORT_PCI_CONFIG_DATA = 0x0cfc;

uint32_t makeAddress(uint8_t bus, uint8_t device, uint8_t funct, uint8_t offset);

namespace Pci
{
    struct ClassCode
    {
        uint8_t baseClass, subClass, interface;

        bool operator==(const ClassCode &rhs) const
        {
            return (baseClass == rhs.baseClass &&
                    subClass == rhs.subClass &&
                    interface == rhs.interface);
        }
    };

    struct Device
    {
        uint8_t bus, device, function;
        uint16_t deviceID, vendorID;
        uint8_t headerType;
        ClassCode classCode;
    };

    inline std::array<Device, 32> m_devices;
    inline size_t m_deviceCount;

    PciError scanAllBus();
};
