# Define file sets
C_FILES := kernel.c console/console.c console/cmds.c util/math.c util/string.c util/random.c util/time.c device/portmap/portmap.c device/keyboard/keyboard.c device/screen/graphics.c games/snake.c games/space_invader.c
ASM_FILES := boot.asm kernel-entry.elf
O_FILES := $(C_FILES:.c=.o)

# Targets
all: qemu_launch

# Launch qemu with the final binary
qemu_launch: os.bin
	qemu-system-i386 -drive format=raw,file=$<

# Create the os.bin by concatenating boot.bin and kernel.bin
os.bin: boot.bin kernel.bin
	cat $^ > $@

# Assemble boot.asm to boot.bin
boot.bin: boot.asm
	nasm $< -f bin -o $@

# Link kernel-entry.o and object files to create kernel.bin
kernel.bin: kernel-entry.o $(O_FILES)
	ld -m elf_i386 -s -Ttext 0x8000 $^ --oformat binary -o $@

# Assemble kernel-entry.elf to kernel-entry.o
kernel-entry.o: kernel-entry.elf
	nasm $< -f elf32 -o $@

# Pattern rule to compile .c files to .o files
%.o: %.c
	gcc -Iinclude -fno-pie -m32 -ffreestanding -c $< -o $@

# Clean up old binaries and object files
clean:
	find . -name \*.o | xargs --no-run-if-empty rm