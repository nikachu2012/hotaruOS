TARGET = kernel.elf

SRCS = $(wildcard *.cpp **/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))

CXXFLAGS += -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -std=c++17
LDFLAGS += --entry kernelMain -z norelro --image-base 0x100000 --static

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean: 
	rm -rf **/*.o

kernel.elf: $(OBJS)
	ld.lld $(LDFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	clang++ $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

