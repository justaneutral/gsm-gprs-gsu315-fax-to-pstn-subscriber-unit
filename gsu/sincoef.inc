;****************************************************************************
;* Filename: sincoef.inc
;* Date: 01-29-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: 1.25 cycles (256+64 samples) of sin() function.
;****************************************************************************

	.sect	"vcoefs"

 .if $isdefed("XDAIS_API")
	.global _VCOEF_MESI_sinTable
 .else
	.global _sin_table
 .endif										;* "XDAIS_API endif

_sin_table:				
_VCOEF_MESI_sinTable:
      .word 00000h, 00324h, 00647h, 0096ah, 00c8bh, 00fabh, 012c8h, 015e2h
      .word 018f8h, 01c0bh, 01f19h, 02223h, 02528h, 02826h, 02b1fh, 02e11h
      .word 030fbh, 033deh, 036bah, 0398ch, 03c56h, 03f17h, 041ceh, 0447ah
      .word 0471ch, 049b4h, 04c3fh, 04ebfh, 05133h, 0539bh, 055f5h, 05842h
      .word 05a82h, 05cb4h, 05ed7h, 060ech, 062f2h, 064e8h, 066cfh, 068a6h
      .word 06a6dh, 06c24h, 06dcah, 06f5fh, 070e2h, 07255h, 073b5h, 07504h
      .word 07641h, 0776ch, 07884h, 0798ah, 07a7dh, 07b5dh, 07c29h, 07ce3h
      .word 07d8ah, 07e1dh, 07e9dh, 07f09h, 07f62h, 07fa7h, 07fd8h, 07ff6h
      .word 07fffh, 07ff6h, 07fd8h, 07fa7h, 07f62h, 07f09h, 07e9dh, 07e1dh
      .word 07d8ah, 07ce3h, 07c29h, 07b5dh, 07a7dh, 0798ah, 07884h, 0776ch
      .word 07641h, 07504h, 073b5h, 07255h, 070e2h, 06f5fh, 06dcah, 06c24h
      .word 06a6dh, 068a6h, 066cfh, 064e8h, 062f2h, 060ech, 05ed7h, 05cb4h
      .word 05a82h, 05842h, 055f5h, 0539bh, 05133h, 04ebfh, 04c3fh, 049b4h
      .word 0471ch, 0447ah, 041ceh, 03f17h, 03c56h, 0398ch, 036bah, 033deh
      .word 030fbh, 02e11h, 02b1fh, 02826h, 02528h, 02223h, 01f19h, 01c0bh
      .word 018f8h, 015e2h, 012c8h, 00fabh, 00c8bh, 0096ah, 00647h, 00324h
      .word 00000h, 0fcdch, 0f9b9h, 0f696h, 0f375h, 0f055h, 0ed38h, 0ea1eh
      .word 0e708h, 0e3f5h, 0e0e7h, 0ddddh, 0dad8h, 0d7dah, 0d4e1h, 0d1efh
      .word 0cf05h, 0cc22h, 0c946h, 0c674h, 0c3aah, 0c0e9h, 0be32h, 0bb86h
      .word 0b8e4h, 0b64ch, 0b3c1h, 0b141h, 0aecdh, 0ac65h, 0aa0bh, 0a7beh
      .word 0a57eh, 0a34ch, 0a129h, 09f14h, 09d0eh, 09b18h, 09931h, 0975ah
      .word 09593h, 093dch, 09236h, 090a1h, 08f1eh, 08dabh, 08c4bh, 08afch
      .word 089bfh, 08894h, 0877ch, 08676h, 08583h, 084a3h, 083d7h, 0831dh
      .word 08276h, 081e3h, 08163h, 080f7h, 0809eh, 08059h, 08028h, 0800ah
      .word 08001h, 0800ah, 08028h, 08059h, 0809eh, 080f7h, 08163h, 081e3h
      .word 08276h, 0831dh, 083d7h, 084a3h, 08583h, 08676h, 0877ch, 08894h
      .word 089bfh, 08afch, 08c4bh, 08dabh, 08f1eh, 090a1h, 09236h, 093dch
      .word 09593h, 0975ah, 09931h, 09b18h, 09d0eh, 09f14h, 0a129h, 0a34ch
      .word 0a57eh, 0a7beh, 0aa0bh, 0ac65h, 0aecdh, 0b141h, 0b3c1h, 0b64ch
      .word 0b8e4h, 0bb86h, 0be32h, 0c0e9h, 0c3aah, 0c674h, 0c946h, 0cc22h
      .word 0cf05h, 0d1efh, 0d4e1h, 0d7dah, 0dad8h, 0ddddh, 0e0e7h, 0e3f5h
      .word 0e708h, 0ea1eh, 0ed38h, 0f055h, 0f375h, 0f696h, 0f9b9h, 0fcdch
      .word 00000h, 00324h, 00647h, 0096ah, 00c8bh, 00fabh, 012c8h, 015e2h
      .word 018f8h, 01c0bh, 01f19h, 02223h, 02528h, 02826h, 02b1fh, 02e11h
      .word 030fbh, 033deh, 036bah, 0398ch, 03c56h, 03f17h, 041ceh, 0447ah
      .word 0471ch, 049b4h, 04c3fh, 04ebfh, 05133h, 0539bh, 055f5h, 05842h
      .word 05a82h, 05cb4h, 05ed7h, 060ech, 062f2h, 064e8h, 066cfh, 068a6h
      .word 06a6dh, 06c24h, 06dcah, 06f5fh, 070e2h, 07255h, 073b5h, 07504h
      .word 07641h, 0776ch, 07884h, 0798ah, 07a7dh, 07b5dh, 07c29h, 07ce3h
      .word 07d8ah, 07e1dh, 07e9dh, 07f09h, 07f62h, 07fa7h, 07fd8h, 07ff6h

;****************************************************************************
