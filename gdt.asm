gdt_start_segment:
    dq 0x00

gdt_code_segment:
    dw 0xFFFF 
    dw 0x0000
    db 0x00
    db 0x9A
    db 0xCF
    db 0x00

gdt_data_segment:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00

gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start_segment - 1
    dd gdt_start_segment