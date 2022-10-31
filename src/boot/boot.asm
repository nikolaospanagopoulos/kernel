ORG 0x7c00
BITS 16


CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data_segment - gdt_start

jmp short start
nop
;FAT 16 Header BIOS PARAMETER BLOCK
OEMIdentifier    db 'PANOS   '
BytesPerSector   dw 0x200 ;512 bytes
SectorPerCluster db 0x80  ;Logical sectors per cluster -> 128
ReservedSectors  dw 200   ;The number of logical sectors before the first FAT. At least one (holds boot sector)
FatCopies        db 0x02  ;2 FAT systems . 1 main and 1 backup
RootDirEntries   dw 0x40  ;Maximum number of FAT16 root directory entries. 
NumSectors       dw 0x00  ;Total logical sectors (If zero, use 4 byte value at offset 0x020)
MediaType        db 0xF8  ;Media descriptor
SectorsPerFat    dw 0x100 ;Logical sectors per File Allocation Table
SectorsPerTrack  dw 0x20  ;Physical sectors per track for disks with INT 13h CHS geometry
NumberOfHeads    dw 0x40  ;Number of heads for disks with INT 13h CHS geometry
HiddenSectors    dd 0x00  ;Count of hidden sectors preceding the partition that contains this FAT volume. This field should always be zero on media that are not partitioned.
SectorsBig       dd 0x773594

; Extended Bios parameter block

DriveNumber      db 0x80 ;0x80 for (first) fixed disk as per INT 13h
WindowsNtBit     db 0x00 ;Should be set to 0 by formatting tools.
Signature        db 0x29 ;indicates that an EBPB with the following 3 entries exists
VolumeID         dd 0xD105 ;Serial Number
PartitionIDString db "NO NAME    ";Partition Volume Label
SystemIDString    db "FAT16   ";File system type, padded with blanks

start:
    jmp 0:step2
step2:
   cli
   mov ax, 0x00
   mov ds, ax
   mov es, ax
   mov ss, ax
   mov sp, 0x7c00
   sti
.load_protected:
   cli
   lgdt[gdt_descriptor]
   mov eax, cr0
   or eax, 0x1
   mov cr0, eax
   jmp CODE_SEG:load32
gdt_start:
gdt_null:
; Null Descriptor, is never referenced by the processor, and should always contain no data. 8 bytes
   dd 0x0
   dd 0x0
gdt_code:   ; offset 0x8 (8 bytes)
;Kernel Mode Code Segment 
    dw 0xffff		; segment limit first 0-15 bits
	dw 0x0			; base first 0-15 bits
	db 0x0			; base 16-23 bits
	db 0x9a			; access byte
	db 0b11001111	; high 4 bits (flags) low 4 bits (limit 4 last bits)(limit is 20 bit wide)
	db 0x0			; base 24-31 bits

gdt_data_segment: ;offset 0x10 (16 bytes)
;Kernel Mode Data Segment 
    dw 0xffff		; segment limit first 0-15 bits
	dw 0x0			; base first 0-15 bits
	db 0x0			; base 16-23 bits
	db 0x92			; access byte
	db 0b11001111	; high 4 bits (flags) low 4 bits (limit 4 last bits)(limit is 20 bit wide)
	db 0x0			; base 24-31 bits


gdt_end:
;get Descriptor size
gdt_descriptor:
  dw gdt_end - gdt_start -1
  dd gdt_start

[BITS 32]
load32:
    mov eax, 1
	mov ecx, 100
	mov edi, 0x0100000
	call ata_lba_read 
	jmp CODE_SEG:0x0100000

ata_lba_read:
    mov ebx, eax ;backup lba
	;send the highest 8 bits of the lba to the hard disc controller
	shr eax, 24
	or eax, 0xE0
	mov dx, 0x1F6
	out dx, al

	;send the total sectors to read
	mov eax, ecx
	mov dx, 0x1F2
	out dx, al

	;send more bits of the lba
	mov eax, ebx
	mov dx, 0x1F3
	out dx, al

	;send more bits of the lba
	mov dx, 0x1F4
	mov eax, ebx
	shr eax, 8
	out dx, al
	
	;send upper 16 bits to the lba
	mov dx, 0x1F5
	mov eax, ebx
	shr eax, 16
	out dx, al

	mov dx, 0x1f7
	mov al, 0x20
	out dx, al

;read all sectors in memory
.next_sector:
    push ecx

; checking if we need to read because of delay
.try_again:
   mov dx, 0x1f7
   in al, dx
   test al, 8
   jz .try_again

;read 256 words at a time
   mov ecx, 256
   mov dx, 0x1F0
   rep insw
   pop ecx
   loop .next_sector
   ret

    





times 510- ($ - $$ ) db 0 ;we have to fill 510 bytes and we pad with 0 
dw 0xAA55


