; Real Mode 16 Bit Directive
[bits 16]

; Set origin of bootloader -> last KB of minimum 32KB needed
org 0x7C00

; Move bootdrive number from DL to variable
mov [BOOT_DRIVE], DL

; Setup stack and base pointers // use scratch space beyond bootloader
mov bp, 0x7E00
mov sp, bp

; Call disk_read
call disk_read
call load_gdt
jmp $

%include "gdt.asm"

; ============================================================ ;

disk_read:
    pusha
    mov AH, 0x02           ; mode for interrupt, read request
    ; for now AL will hold size of bootloader 512 bytes (0x01) (1 SECTOR = 512 bytes)
    mov AL, [num_sectors]           ; sectors to read (size kernal rounded to next 512 bytes), should calculate with (ceiling (((File size) – 512)/512) sectors))
    mov BX, KERNEL_ADDRESS ; move Kernal address into BX register
    mov CH, 0x00           ; cylinder of disk -> set to zero
    mov CL, 0x02           ; sector of disk -> set to one (why? cause) will be 0x02 in future to skip bootloader
    mov DH, 0x00           ; head of disk -> set to zero
    mov DL, [BOOT_DRIVE]   ; move boot drive back into DL
    int 0x13               ; disk read interrupt

    jc disk_unknown_read_error ; jump if carry flag is 1

    ; AL stores actual number of read sectors
    cmp byte AL, [num_sectors]      ; compare num sectors

    jne disk_sectors_read_error ; jump if compare resulted in false

    ; call load_gdt

    popa
    ret

; ============================================================ ;

boot_signatures_do_not_match_error:
    mov BX, BOOT_SIGNATURES_DO_NOT_MATCH_ERROR_MSG
    call print_string
    jmp $ ; jump to current instruction to keep IP value the same (basically halt)
 
disk_sectors_read_error:
    mov BX, DISK_SECTORS_READ_ERROR_MSG
    call print_string
    jmp $ ; jump to current instruction to keep IP value the same (basically halt)

disk_unknown_read_error:
    mov BX, DISK_UNKNOWN_READ_ERROR_MSG
    call print_string
    jmp $ ; jump to current instruction to keep IP value the same (basically halt)

; ============================================================ ;

print_string:
    pusha
    mov AH, 0x0E
    ; loop through characters of message
    print_string_loop:
        ; contents of BX into AL
        mov AL, [BX]
        cmp AL, 0
        je print_string_done
        int 0x10
        add BX, 1 ; increment address in BX
        jmp print_string_loop

print_string_done:
    popa
    ret ; return to line after 'call print_string'

; ============================================================ ;

load_gdt:
    cli ; Turn off interrupts
    lgdt [gdt_descriptor] ; load the gdt descriptor
    mov eax, cr0 ; move control register contents to 32 bit register
    or eax, 0x1 ; Turn on the first bit
    mov cr0, eax ; Move contents back to control register
    jmp CODE_SEGMENT_INDEX:begin_32bit_mode ; Code Index = 0x08

; ============================================================ ;

; Protected 32 Bit Directive
[bits 32]

begin_32bit_mode:
    mov ebp, 0x90000
    mov esp, ebp

    mov ax, DATA_SEGMENT_INDEX ; 0x10 for data selector
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ; jmp CODE_SEGMENT_INDEX:print_letter_in_32_bit_mode
    jmp KERNEL_ADDRESS

print_letter_in_32_bit_mode:
    mov byte [0xb8000], 'X'
    mov byte [0xb8001], 0x01
    ret

; ============================================================ ;
; Data
KERNEL_ADDRESS equ 0x8000
BOOT_DRIVE db 0
DISK_UNKNOWN_READ_ERROR_MSG db 'Unknown error while reading disk.', 0
DISK_SECTORS_READ_ERROR_MSG db 'Size of data requested != Size of data read.', 0
BOOT_SIGNATURES_DO_NOT_MATCH_ERROR_MSG db 'Boot signature in Kernal memory space does not match Boot signature at origin 0x7C00.', 0
DISK_READ_SUCCESS_MSG db 'Disk Read Success!', 0
BOOT_SIGNATURE equ 0xAA55
BOOT_LOADER_FINAL_BYTE equ 0xAA
CODE_SEGMENT_INDEX equ 0x08 ; hard code for now
DATA_SEGMENT_INDEX equ 0x10 ; hard code for now

; TODO: calculate size of kernal file and store in variable
num_sectors db 0x59

; Pad the remaining bootloader with 0s to produce 510 total bytes
times 510 - ($-$$) db 0

; Add the magic number to the output (boot signature -> remaining 2 bytes)
dw BOOT_SIGNATURE