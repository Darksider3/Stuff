;; http://3zanders.co.uk/2017/10/16/writing-a-bootloader2/
bits 16 ; 16 bit mode @nasm
org 0x7c00; Put this whole thing beyond 0x7c00

boot: ;boot label
  mov ax, 0x2401 ; HELP: What is 0x2401?
  int 0x15       ; HELP: Enable A20bit - WHAT IS THAT
  mov ax, 0x3    ; move static 3 into ax
  int 0x10       ; VGA-Text-Mode interrupt: Loads value from AX and sets its mode equivalent; Is this how most interrupts work???
  lgdt [gdt_pointer] ; HELP: Where the fuck is the gdt_pointer coming from?? What is this doing???
  mov eax, cr0 ; why are we loading cr0 into eax?
  or eax, 0x1 ; set protected mode bit on special CPU reg cr0 - wat
  mov cr0, eax ; Are we moving eax into cr0 now? Why dont just use a static here? Or do we need cr0's value and must 'OR' it with 1 to enable protected mode?
  jmp CODE_SEG:boot2 ; long jump to the code segment
;;;;;;Global Descriptor Table
;; It is just a struct.
;; db -> define byte        -> 1 byte
;; dw -> define word        -> 2 byte
;; dd -> define doubleword  -> 4 byte
;; dq -> define quawword    -> 8 byte
;; dt -> define ten bytes   -> 10 byte
;; [variable Name] - [DefineLength] [initial value] (e.g. dq 0x0 means "define quadword with initial value 0")
gdt_start:
  dq 0x0
gdt_code:
  dw 0xFFFF
  dw 0x0
  db 0x0
  db 10011010b
  db 11001111b
  db 0x0
gdt_data:
  dw 0xFFFF
  dw 0x0
  db 0x0
  db 10010010b
  db 11001111b
  db 0x0
gdt_end:
gdt_pointer:
  dw gdt_end - gdt_start
  dd gdt_start ; dd?
CODE_SEG equ gdt_code - gdt_start ; CODE_SEG = gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start ; DATA_SEG = gdt_data - gdt_start

bits 32
boot2:
  mov ax, DATA_SEG ; Moves DATA_SEG onto ax?! WHY
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax ; why are we doing this?
  mov esi, hello ; Move hello into esi?!
  mov ebx, 0xb8000 ; Move 0xb8000 into ebx?! WHY

.loop:
  lodsb ; ????
  or al,al ; al == 0?
  jz halt ; yes => Halt.
  or eax, 0x0200 ; ORs 0x0100 on EAX
  mov word [ebx], ax ; move word(2 byte) from ax to ebx?
  add ebx, 2 ; add 2 to ebx
  jmp .loop ; jump back to beginning?

halt:
  cli
  hlt

hello: db "Hello World!", 0
times 510 - ($-$$) db 0 ; ???
dw 0xaa55 ; ?????
