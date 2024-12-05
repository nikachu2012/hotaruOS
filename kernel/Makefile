TARGET = kernel.elf

SRCS = $(wildcard *.cpp **/*.cpp) 
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) ProggyClean.o
DEPS = $(patsubst %.cpp, %.d, $(SRCS))

CXXFLAGS += -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone \
			-fno-exceptions -fno-rtti -std=c++17
LDFLAGS += --entry kernelMain -z norelro --image-base 0x100000 --static -lc

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean: 
	rm -rf **/*.o *.o **/*.d *.d

-include $(DEPS)

$(TARGET): $(OBJS)
	ld.lld $(LDFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	clang++ $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

ProggyClean.o: ProggyClean.bin
	objcopy -I binary -O elf64-x86-64 -B i386:x86-64 $< $@
