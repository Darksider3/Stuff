SOURCES = boot1.asm
OUT = boot1.bin


ASS = nasm
ASSFLAGS = -f bin

all:
	$(ASS) $(ASSFLAGS) $(SOURCES) -o $(OUT)

run:
	qemu-system-x86_64 -fda boot1.bin

run2:
	nasm -f bin boot2.asm && qemu-system-x86_64 -fda boot2
