#include "interrupt.hpp"

InterruptDescriptor idt[IDT_SIZE];

void SetIDTEntry(InterruptDescriptor &desc, InterruptDescriptorAttribute attr, uint64_t offset,
                 uint16_t segmentSelector)
{
    desc.attr = attr;
    desc.offsetLow = offset & 0xffffu;
    desc.offsetMiddle = (offset >> 16) & 0xffffu;
    desc.offsetHigh = (offset >> 32);
    desc.segment_selector = segmentSelector;
    desc.reserved = 0;
}

InterruptDescriptorAttribute CreateIDTAttr(DescriptorType type, uint8_t descriptorPrivilegeLevel, bool present,
                                           uint8_t interruptStackTable)
{
    auto attr = InterruptDescriptorAttribute();
    attr.bits.type = type;
    attr.bits.descriptorPrivilegeLevel = descriptorPrivilegeLevel;
    attr.bits.present = present;
    attr.bits.interruptStackTable = interruptStackTable;
    return attr;
}

