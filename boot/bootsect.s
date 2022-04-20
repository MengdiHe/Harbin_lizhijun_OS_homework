SETUPLEN=4
SETUPSEG=0x07e0 !setup's segment addr = 0x07c0 + 0x20(512byte) = 0x07e0
entry _start
_start:
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov cx,#23
	mov bx,#0x0007
	mov bp,#msg1
	mov ax,#0x07c0
	mov es,ax
	mov ax,#0x1301
	int 0x10

load_setup:
	mov	dx,#0x0000		! drive 0, head 0
	mov	cx,#0x0002		! sector 2, track 0
	mov	bx,#0x0200		! address = 512, in INITSEG
	mov	ax,#0x0200+SETUPLEN	! service 2, nr of sectors
	int	0x13			! read it
	jnc	ok_load_setup		! ok - continue
	mov	dx,#0x0000
	mov	ax,#0x0000		! reset the diskette
	int	0x13
	jmp	load_setup

ok_load_setup:
	jmpi	0,SETUPSEG

msg1:
	.byte 13,10
	.ascii "Welcome to HMDOS!"
	.byte 13,10,13,10
.org 510

boot_flag:
	.word 0xAA55
	
