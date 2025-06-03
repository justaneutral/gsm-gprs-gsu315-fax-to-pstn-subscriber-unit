;****************************************************************************
;* Filename: rcoscoef.inc
;* Date: 11-24-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: Common baud rate interpolator filter coefficients 
;****************************************************************************

	.sect	"vcoefs"
 
 .if (V22_MODEM|V34_MODEM=ENABLED)
 .if $isdefed("XDAIS_API")
	.global	_VCOEF_MESI_RxTiming600
 .else
	.global	_Rx_timing600
 .endif										;* "XDAIS_API endif
_Rx_timing600:		
_VCOEF_MESI_RxTiming600:
	.word 2,0,1,0, 1,2,0,0, 6,7,7,0, 7,7,6,0
 .endif
 .if (V26_MODEM|TX_V27_MODEM_2400|RX_V27_MODEM_2400|V34_MODEM)=ENABLED
 .if $isdefed("XDAIS_API")
	.global	_VCOEF_MESI_RxTiming1200
 .else
	.global	_Rx_timing1200
 .endif										;* "XDAIS_API endif
_Rx_timing1200:		
_VCOEF_MESI_RxTiming1200:
	.word 1,2,0,0, 2,0,1,0, 3,3,4,0, 4,3,3,0
 .endif
 .if (TX_V27_MODEM_4800=ENABLED)|(RX_V27_MODEM_4800=ENABLED)
 .if $isdefed("XDAIS_API")
	.global	_VCOEF_MESI_RxTiming1600
 .else
	.global	_Rx_timing1600
 .endif										;* "XDAIS_API endif
_Rx_timing1600:		
_VCOEF_MESI_RxTiming1600:
	.word 3,0,0,0, 3,0,0,0, 2,0,0,3, 3,0,0,2
 .endif
 .if (V17_MODEM|V29_MODEM|V32_MODEM|V34_2400)=ENABLED
 .if $isdefed("XDAIS_API")
	.global	_VCOEF_MESI_RxTiming2400
 .else
	.global	_Rx_timing2400
 .endif										;* "XDAIS_API endif
_Rx_timing2400:		
_VCOEF_MESI_RxTiming2400:
	.word 2,0,1,0, 1,2,0,0, 1,2,2,0, 2,2,1,0
 .endif
 .if (V34_3000=ENABLED)
 .if $isdefed("XDAIS_API")
	.global	_VCOEF_MESI_RxTiming3000
 .else
	.global	_Rx_timing3000
 .endif										;* "XDAIS_API endif
_Rx_timing3000:		
_VCOEF_MESI_RxTiming3000:
	.word 1,2,0,0, 2,0,1,0, 1,1,2,0, 2,1,1,0
 .endif
 .if (V34_3200=ENABLED)
 .if $isdefed("XDAIS_API")
	.global	_VCOEF_MESI_RxTiming3200
 .else
	.global	_Rx_timing3200
 .endif										;* "XDAIS_API endif
_Rx_timing3200:		
_VCOEF_MESI_RxTiming3200:
	.word 1,2,3,0, 3,0,1,2, 1,1,1,2, 2,1,1,1
 .endif

 .if (TX_V22C_MODEM|V34_MODEM)=ENABLED
 .if $isdefed("XDAIS_API")
	.global	_VCOEF_MESI_TxRCOS600f1200
 .else
	.global _Tx_RCOS600_f1200
 .endif										;* "XDAIS_API endif
_Tx_RCOS600_f1200:
_VCOEF_MESI_TxRCOS600f1200:
	.word 0015ch, 0ff4fh, 0010ah, 0ffd6h, 0007fh, 00014h, 0ffe9h, 0fff4h
	.word 0ff73h, 0ff73h, 0ff50h, 0fea7h, 0ffa5h, 0fdbch, 0007ch, 0fcedh
	.word 001cch, 0fc7ah, 00367h, 0fc99h, 0050dh, 0fd6dh, 0066eh, 0fefch
	.word 0073bh, 00125h, 00734h, 003abh, 00635h, 00635h, 00443h, 0085eh
	.word 0018ch, 009c8h, 0fe64h, 00a2ch, 0fb36h, 00967h, 0f877h, 00789h
	.word 0f692h, 004ceh, 0f5cdh, 0019dh, 0f642h, 0fe75h, 0f7d1h, 0fbd5h
	.word 0fa26h, 0fa26h, 0fcc3h, 0f9a4h, 0ff1bh, 0fa54h, 000a4h, 0fbf5h
	.word 000feh, 0fe0dh, 00003h, 0fffdh, 0fdddh, 00116h, 0fafbh, 000cbh
	.word 0f814h, 0febfh, 0f604h, 0faeah, 0f5aah, 0f5aah, 0f7b7h, 0efbch
	.word 0fc8bh, 0ea2dh, 00416h, 0e633h, 00dc3h, 0e4fdh, 01881h, 0e77fh
	.word 022d8h, 0ee3fh, 02b1eh, 0f92ch, 02fach, 0078ch, 02f25h, 01805h
	.word 028b3h, 028b3h, 01c3bh, 03768h, 00a71h, 041edh, 0f4dch, 04658h
	.word 0ddb2h, 04355h, 0c799h, 03867h, 0b55ch, 02608h, 0a983h, 00db2h
	.word 0a601h, 0f1bfh, 0abe5h, 0d525h, 0bb25h, 0bb25h, 0d295h, 0a6ddh
	.word 0effdh, 09ae6h, 01052h, 098f5h, 03016h, 0a1a0h, 04bbfh, 0b441h
	.word 0602bh, 0cf00h, 06b01h, 0ef0eh, 06b01h, 010f2h, 0602bh, 03100h
	.word 04bbfh, 04bbfh, 03016h, 05e60h, 01052h, 0670bh, 0effdh, 0651ah
	.word 0d295h, 05923h, 0bb25h, 044dbh, 0abe5h, 02adbh, 0a601h, 00e41h
	.word 0a983h, 0f24eh, 0b55ch, 0d9f8h, 0c799h, 0c799h, 0ddb2h, 0bcabh
	.word 0f4dch, 0b9a8h, 00a71h, 0be13h, 01c3bh, 0c898h, 028b3h, 0d74dh
	.word 02f25h, 0e7fbh, 02fach, 0f874h, 02b1eh, 006d4h, 022d8h, 011c1h
	.word 01881h, 01881h, 00dc3h, 01b03h, 00416h, 019cdh, 0fc8bh, 015d3h
	.word 0f7b7h, 01044h, 0f5aah, 00a56h, 0f604h, 00516h, 0f814h, 00141h
	.word 0fafbh, 0ff35h, 0fdddh, 0feeah, 00003h, 00003h, 000feh, 001f3h
	.word 000a4h, 0040bh, 0ff1bh, 005ach, 0fcc3h, 0065ch, 0fa26h, 005dah
	.word 0f7d1h, 0042bh, 0f642h, 0018bh, 0f5cdh, 0fe63h, 0f692h, 0fb32h
	.word 0f877h, 0f877h, 0fb36h, 0f699h, 0fe64h, 0f5d4h, 0018ch, 0f638h
	.word 00443h, 0f7a2h, 00635h, 0f9cbh, 00734h, 0fc55h, 0073bh, 0fedbh
	.word 0066eh, 00104h, 0050dh, 00293h, 00367h, 00367h, 001cch, 00386h
	.word 0007ch, 00313h, 0ffa5h, 00244h, 0ff50h, 00159h, 0ff73h, 0008dh
	.word 0ffe9h, 0000ch, 0007fh, 0ffech, 0010ah, 0002ah, 0015ch, 000b1h
 .endif
 .if (TX_V22A_MODEM|V34_MODEM)=ENABLED
 .if $isdefed("XDAIS_API")
	.global	_VCOEF_MESI_TxRCOS600f2400
 .else
	.global _Tx_RCOS600_f2400
 .endif										;* "XDAIS_API endif
_Tx_RCOS600_f2400:
_VCOEF_MESI_TxRCOS600f2400:
	.word 000e5h, 0fec4h, 00100h, 0ffadh, 0007ah, 00027h, 0fff1h, 0ffebh
	.word 00000h, 0ff39h, 000e4h, 0fec7h, 0022eh, 0ff4bh, 002f5h, 000f6h
	.word 00253h, 00333h, 00000h, 004d0h, 0fcabh, 00496h, 0f9cfh, 00203h
	.word 0f909h, 0fdbdh, 0fb40h, 0f976h, 00000h, 0f739h, 00585h, 0f867h
	.word 0096bh, 0fcf1h, 009cbh, 0032eh, 00633h, 00889h, 00000h, 00aa8h
	.word 0f9c8h, 00890h, 0f62eh, 00331h, 0f69eh, 0fcf4h, 0fa9ah, 0f892h
	.word 00000h, 0f7b9h, 00432h, 0fa3ah, 00576h, 0fe3ah, 003e5h, 00144h
	.word 00149h, 001c5h, 00000h, 00005h, 00169h, 0fe10h, 004d5h, 0fe6eh
	.word 007a1h, 0027ah, 00696h, 00910h, 00000h, 00e9fh, 0f545h, 00ec4h
	.word 0eafch, 006d4h, 0e728h, 0f7eeh, 0ee2eh, 0e779h, 00000h, 0dd59h
	.word 016fch, 0e05dh, 02984h, 0f283h, 02de7h, 00eeah, 01f19h, 02aceh
	.word 00000h, 0398fh, 0db73h, 0324fh, 0c085h, 014a0h, 0bc44h, 0e9feh
	.word 0d395h, 0c2ddh, 00000h, 0b03ch, 0313dh, 0bc3ah, 05348h, 0e4f1h
	.word 056a9h, 01c28h, 0377ch, 04c5eh, 00000h, 06161h, 0c533h, 050efh
	.word 09ea6h, 01fa1h, 09cc7h, 0dfc3h, 0c1beh, 0aa4fh, 00000h, 094e1h
	.word 03f70h, 0a8afh, 06709h, 0de86h, 06709h, 0217ah, 03f70h, 05751h
	.word 00000h, 06b1fh, 0c1beh, 055b1h, 09cc7h, 0203dh, 09ea6h, 0e05fh
	.word 0c533h, 0af11h, 00000h, 09e9fh, 0377ch, 0b3a2h, 056a9h, 0e3d8h
	.word 05348h, 01b0fh, 0313dh, 043c6h, 00000h, 04fc4h, 0d395h, 03d23h
	.word 0bc44h, 01602h, 0c085h, 0eb60h, 0db73h, 0cdb1h, 00000h, 0c671h
	.word 01f19h, 0d532h, 02de7h, 0f116h, 02984h, 00d7dh, 016fch, 01fa3h
	.word 00000h, 022a7h, 0ee2eh, 01887h, 0e728h, 00812h, 0eafch, 0f92ch
	.word 0f545h, 0f13ch, 00000h, 0f161h, 00696h, 0f6f0h, 007a1h, 0fd86h
	.word 004d5h, 00192h, 00169h, 001f0h, 00000h, 0fffbh, 00149h, 0fe3bh
	.word 003e5h, 0febch, 00576h, 001c6h, 00432h, 005c6h, 00000h, 00847h
	.word 0fa9ah, 0076eh, 0f69eh, 0030ch, 0f62eh, 0fccfh, 0f9c8h, 0f770h
	.word 00000h, 0f558h, 00633h, 0f777h, 009cbh, 0fcd2h, 0096bh, 0030fh
	.word 00585h, 00799h, 00000h, 008c7h, 0fb40h, 0068ah, 0f909h, 00243h
	.word 0f9cfh, 0fdfdh, 0fcabh, 0fb6ah, 00000h, 0fb30h, 00253h, 0fccdh
	.word 002f5h, 0ff0ah, 0022eh, 000b5h, 000e4h, 00139h, 00000h, 000c7h
	.word 0fff1h, 00015h, 0007ah, 0ffd9h, 00100h, 00053h, 000e5h, 0013ch
 .endif
 .if (RX_V22A_MODEM|V34_MODEM)=ENABLED
 .if $isdefed("XDAIS_API")
	.global	_VCOEF_MESI_RxRCOS600f1200
 .else
	.global _Rx_RCOS600_f1200
 .endif										;* "XDAIS_API endif
_Rx_RCOS600_f1200:
_VCOEF_MESI_RxRCOS600f1200:
	.word 0ffd0h, 0fff9h, 0ffcbh, 0ffe5h, 0ffceh, 0ffceh, 0ffdbh, 0ffb7h
	.word 0fff2h, 0ffa6h, 0000fh, 0ff9eh, 00030h, 0ffa1h, 0004fh, 0ffb1h
	.word 00067h, 0ffcch, 00075h, 0ffeeh, 00075h, 00012h, 00068h, 00035h
	.word 0004fh, 0004fh, 00030h, 0005eh, 0000fh, 00061h, 0fff3h, 00057h
	.word 0ffdeh, 00044h, 0ffd4h, 0002ch, 0ffd6h, 00015h, 0ffe2h, 00004h
	.word 0fff3h, 0fffeh, 00004h, 00002h, 00010h, 00010h, 00013h, 00025h
	.word 00009h, 0003ch, 0fff4h, 0004eh, 0ffd5h, 00055h, 0ffb2h, 0004eh
	.word 0ff91h, 00038h, 0ff7ah, 00015h, 0ff72h, 0ffeah, 0ff7bh, 0ffbch
	.word 0ff95h, 0ff95h, 0ffbch, 0ff7ah, 0ffeah, 0ff71h, 00015h, 0ff79h
	.word 00037h, 0ff93h, 0004ah, 0ffb6h, 0004ah, 0ffdah, 00039h, 0fff7h
	.word 0001bh, 00004h, 0fffbh, 0fffeh, 0ffe2h, 0ffe2h, 0ffdbh, 0ffb6h
	.word 0ffedh, 0ff84h, 0001ah, 0ff58h, 00062h, 0ff3fh, 000bah, 0ff46h
	.word 00115h, 0ff73h, 00160h, 0ffc9h, 0018ch, 0003eh, 0018bh, 000c9h
	.word 00154h, 00154h, 000e9h, 001cbh, 00054h, 00218h, 0ffa8h, 0022eh
	.word 0fefah, 00203h, 0fe63h, 0019dh, 0fdfbh, 00107h, 0fdd1h, 00058h
	.word 0fdeah, 0ffach, 0fe40h, 0ff1ch, 0febfh, 0febfh, 0ff4fh, 0fea4h
	.word 0ffcfh, 0fec9h, 00023h, 0ff23h, 00036h, 0ff95h, 00000h, 00000h
	.word 0ff8bh, 0003bh, 0feedh, 0002bh, 0fe4eh, 0ffbch, 0fdddh, 0fee9h
	.word 0fdc9h, 0fdc9h, 0fe3ah, 0fc84h, 0ff43h, 0fb53h, 000e0h, 0fa79h
	.word 002f3h, 0fa37h, 00540h, 0fac0h, 00777h, 0fc33h, 0093dh, 0fe8ah
	.word 00a37h, 0019eh, 00a1ah, 00525h, 008b8h, 008b8h, 0060ch, 00bdfh
	.word 0023ch, 00e20h, 0fd9dh, 00f12h, 0f8a7h, 00e6dh, 0f3ebh, 00c15h
	.word 0f002h, 00826h, 0ed78h, 002efh, 0ecb8h, 0fcf3h, 0edfbh, 0f6d2h
	.word 0f13fh, 0f13fh, 0f645h, 0ece7h, 0fc92h, 0ea56h, 0037fh, 0e9ech
	.word 00a4dh, 0ebc7h, 0103bh, 0efc5h, 0149bh, 0f581h, 016edh, 0fc5fh
	.word 016edh, 003a1h, 0149bh, 00a7fh, 0103bh, 0103bh, 00a4dh, 01439h
	.word 0037fh, 01614h, 0fc92h, 015aah, 0f645h, 01319h, 0f13fh, 00ec1h
	.word 0edfbh, 0092eh, 0ecb8h, 0030dh, 0ed78h, 0fd11h, 0f002h, 0f7dah
	.word 0f3ebh, 0f3ebh, 0f8a7h, 0f193h, 0fd9dh, 0f0eeh, 0023ch, 0f1e0h
	.word 0060ch, 0f421h, 008b8h, 0f748h, 00a1ah, 0fadbh, 00a37h, 0fe62h
	.word 0093dh, 00176h, 00777h, 003cdh, 00540h, 00540h, 002f3h, 005c9h
	.word 000e0h, 00587h, 0ff43h, 004adh, 0fe3ah, 0037ch, 0fdc9h, 00237h
	.word 0fdddh, 00117h, 0fe4eh, 00044h, 0feedh, 0ffd5h, 0ff8bh, 0ffc5h
	.word 00000h, 00000h, 00036h, 0006bh, 00023h, 000ddh, 0ffcfh, 00137h
	.word 0ff4fh, 0015ch, 0febfh, 00141h, 0fe40h, 000e4h, 0fdeah, 00054h
	.word 0fdd1h, 0ffa8h, 0fdfbh, 0fef9h, 0fe63h, 0fe63h, 0fefah, 0fdfdh
	.word 0ffa8h, 0fdd2h, 00054h, 0fde8h, 000e9h, 0fe35h, 00154h, 0feach
	.word 0018bh, 0ff37h, 0018ch, 0ffc2h, 00160h, 00037h, 00115h, 0008dh
	.word 000bah, 000bah, 00062h, 000c1h, 0001ah, 000a8h, 0ffedh, 0007ch
	.word 0ffdbh, 0004ah, 0ffe2h, 0001eh, 0fffbh, 00002h, 0001bh, 0fffch
	.word 00039h, 00009h, 0004ah, 00026h, 0004ah, 0004ah, 00037h, 0006dh
	.word 00015h, 00087h, 0ffeah, 0008fh, 0ffbch, 00086h, 0ff95h, 0006bh
	.word 0ff7bh, 00044h, 0ff72h, 00016h, 0ff7ah, 0ffebh, 0ff91h, 0ffc8h
	.word 0ffb2h, 0ffb2h, 0ffd5h, 0ffabh, 0fff4h, 0ffb2h, 00009h, 0ffc4h
	.word 00013h, 0ffdbh, 00010h, 0fff0h, 00004h, 0fffeh, 0fff3h, 00002h
	.word 0ffe2h, 0fffch, 0ffd6h, 0ffebh, 0ffd4h, 0ffd4h, 0ffdeh, 0ffbch
	.word 0fff3h, 0ffa9h, 0000fh, 0ff9fh, 00030h, 0ffa2h, 0004fh, 0ffb1h
	.word 00068h, 0ffcbh, 00075h, 0ffeeh, 00075h, 00012h, 00067h, 00034h
	.word 0004fh, 0004fh, 00030h, 0005fh, 0000fh, 00062h, 0fff2h, 0005ah
	.word 0ffdbh, 00049h, 0ffceh, 00032h, 0ffcbh, 0001bh, 0ffd0h, 00007h
 .endif
 .if (RX_V22C_MODEM|V34_MODEM)=ENABLED
 .if $isdefed("XDAIS_API")
	.global	_VCOEF_MESI_RxRCOS600f2400
 .else
	.global _Rx_RCOS600_f2400
 .endif										;* "XDAIS_API endif
_Rx_RCOS600_f2400:
_VCOEF_MESI_RxRCOS600f2400:
	.word 0ffd2h, 0fff1h, 0ffddh, 0ffd0h, 00000h, 0ffb9h, 00030h, 0ffbeh
	.word 00057h, 0ffe4h, 0005fh, 0001eh, 0003fh, 00056h, 00000h, 00070h
	.word 0ffbch, 0005eh, 0ff90h, 00024h, 0ff90h, 0ffdch, 0ffbch, 0ffa2h
	.word 00000h, 0ff90h, 0003eh, 0ffaah, 0005dh, 0ffe2h, 00054h, 0001bh
	.word 0002ch, 0003dh, 00000h, 0003eh, 0ffe5h, 00026h, 0ffe3h, 00009h
	.word 0fff4h, 0fffch, 00002h, 00003h, 00000h, 00017h, 0ffe8h, 00022h
	.word 0ffc6h, 00012h, 0ffb5h, 0ffe8h, 0ffc8h, 0ffb3h, 00000h, 0ff91h
	.word 00049h, 0ff9ch, 00081h, 0ffd6h, 00089h, 0002ch, 00058h, 00079h
	.word 00000h, 00098h, 0ffa8h, 0007ah, 0ff76h, 0002dh, 0ff7eh, 0ffd6h
	.word 0ffb8h, 0ff9dh, 00000h, 0ff97h, 00031h, 0ffbdh, 00036h, 0ffefh
	.word 0001ah, 00008h, 0fffdh, 0fffch, 00000h, 0ffd6h, 00030h, 0ffbdh
	.word 00077h, 0ffdah, 000a2h, 00034h, 0007fh, 000afh, 00000h, 00108h
	.word 0ff4ah, 000fbh, 0feadh, 0006eh, 0fe82h, 0ff84h, 0fefch, 0fe9ah
	.word 00000h, 0fe1fh, 0012eh, 0fe60h, 00204h, 0ff59h, 00219h, 000aeh
	.word 00154h, 001d4h, 00000h, 00248h, 0feabh, 001d5h, 0fde6h, 000afh
	.word 0fdfeh, 0ff59h, 0fed8h, 0fe69h, 00000h, 0fe3ah, 000e6h, 0fec4h
	.word 0012bh, 0ff9fh, 000d5h, 00045h, 00046h, 00061h, 00000h, 00001h
	.word 0004dh, 0ff96h, 00109h, 0ffaah, 001a2h, 00087h, 00169h, 001f1h
	.word 00000h, 00322h, 0fdb4h, 0032ah, 0fb80h, 00176h, 0faaeh, 0fe46h
	.word 0fc2fh, 0fabfh, 00000h, 0f893h, 004ech, 0f939h, 008e5h, 0fd1dh
	.word 009d6h, 00332h, 006aah, 0092ch, 00000h, 00c55h, 0f82bh, 00ac7h
	.word 0f266h, 0046bh, 0f17dh, 0fb49h, 0f67ch, 0f2e7h, 00000h, 0eee9h
	.word 00a8dh, 0f17bh, 011d8h, 0fa34h, 01291h, 00608h, 00be3h, 0105dh
	.word 00000h, 014ddh, 0f367h, 01157h, 0eb24h, 006c7h, 0eabeh, 0f918h
	.word 0f2a9h, 0eda4h, 00000h, 0e90ch, 00d98h, 0ed4bh, 01613h, 0f8d4h
	.word 01613h, 0072ch, 00d98h, 012b5h, 00000h, 016f4h, 0f2a9h, 0125ch
	.word 0eabeh, 006e8h, 0eb24h, 0f939h, 0f367h, 0eea9h, 00000h, 0eb23h
	.word 00be3h, 0efa3h, 01291h, 0f9f8h, 011d8h, 005cch, 00a8dh, 00e85h
	.word 00000h, 01117h, 0f67ch, 00d19h, 0f17dh, 004b7h, 0f266h, 0fb95h
	.word 0f82bh, 0f539h, 00000h, 0f3abh, 006aah, 0f6d4h, 009d6h, 0fcceh
	.word 008e5h, 002e3h, 004ech, 006c7h, 00000h, 0076dh, 0fc2fh, 00541h
	.word 0faaeh, 001bah, 0fb80h, 0fe8ah, 0fdb4h, 0fcd6h, 00000h, 0fcdeh
	.word 00169h, 0fe0fh, 001a2h, 0ff79h, 00109h, 00056h, 0004dh, 0006ah
	.word 00000h, 0ffffh, 00046h, 0ff9fh, 000d5h, 0ffbbh, 0012bh, 00061h
	.word 000e6h, 0013ch, 00000h, 001c6h, 0fed8h, 00197h, 0fdfeh, 000a7h
	.word 0fde6h, 0ff51h, 0feabh, 0fe2bh, 00000h, 0fdb8h, 00154h, 0fe2ch
	.word 00219h, 0ff52h, 00204h, 000a7h, 0012eh, 001a0h, 00000h, 001e1h
	.word 0fefch, 00166h, 0fe82h, 0007ch, 0feadh, 0ff92h, 0ff4ah, 0ff05h
	.word 00000h, 0fef8h, 0007fh, 0ff51h, 000a2h, 0ffcch, 00077h, 00026h
	.word 00030h, 00043h, 00000h, 0002ah, 0fffdh, 00004h, 0001ah, 0fff8h
	.word 00036h, 00011h, 00031h, 00043h, 00000h, 00069h, 0ffb8h, 00063h
	.word 0ff7eh, 0002ah, 0ff76h, 0ffd3h, 0ffa8h, 0ff86h, 00000h, 0ff68h
	.word 00058h, 0ff87h, 00089h, 0ffd4h, 00081h, 0002ah, 00049h, 00064h
	.word 00000h, 0006fh, 0ffc8h, 0004dh, 0ffb5h, 00018h, 0ffc6h, 0ffeeh
	.word 0ffe8h, 0ffdeh, 00000h, 0ffe9h, 00002h, 0fffdh, 0fff4h, 00004h
	.word 0ffe3h, 0fff7h, 0ffe5h, 0ffdah, 00000h, 0ffc2h, 0002ch, 0ffc3h
	.word 00054h, 0ffe5h, 0005dh, 0001eh, 0003eh, 00056h, 00000h, 00070h
	.word 0ffbch, 0005eh, 0ff90h, 00024h, 0ff90h, 0ffdch, 0ffbch, 0ffa2h
	.word 00000h, 0ff90h, 0003fh, 0ffaah, 0005fh, 0ffe2h, 00057h, 0001ch
	.word 00030h, 00042h, 00000h, 00047h, 0ffddh, 00030h, 0ffd2h, 0000fh
 .endif
 .if (V26_MODEM=ENABLED)|(TX_V27_MODEM_2400=ENABLED)|(RX_V27_MODEM_2400=ENABLED)
 .if $isdefed("XDAIS_API")
	.global	_VCOEF_MESI_RCOS1200f1800
 .else
	.global _RCOS1200_f1800
 .endif										;* "XDAIS_API endif
_RCOS1200_f1800:
_VCOEF_MESI_RCOS1200f1800:
      .word 0ff3ch, 0ff60h, 0ff5ch, 0ff55h, 0ff7bh, 0ff51h, 0ff9bh, 0ff53h
      .word 0ffb9h, 0ff5ch, 0ffd3h, 0ff6ch, 0ffeah, 0ff81h, 0fffbh, 0ff9ch
      .word 00004h, 0ffbah, 00006h, 0ffdah, 00001h, 0fffbh, 0fff5h, 0001ah
      .word 0ffdfh, 00039h, 0ffc1h, 00053h, 0ff9ch, 00068h, 0ff70h, 00076h
      .word 0ff3fh, 0007bh, 0ff0ah, 00077h, 0fed4h, 00068h, 0fe9dh, 0004dh
      .word 0fe69h, 00028h, 0fe38h, 0fff8h, 0fe0eh, 0ffbbh, 0fdech, 0ff76h
      .word 0fdd4h, 0ff27h, 0fdc8h, 0fed1h, 0fdcah, 0fe76h, 0fddbh, 0fe18h
      .word 0fdfah, 0fdb9h, 0fe2ah, 0fd5eh, 0fe69h, 0fd06h, 0feb7h, 0fcb6h
      .word 0ff12h, 0fc70h, 0ff7ah, 0fc37h, 0ffedh, 0fc0ch, 00066h, 0fbf2h
      .word 000e5h, 0fbeah, 00167h, 0fbf5h, 001e7h, 0fc13h, 00264h, 0fc44h
      .word 002d9h, 0fc88h, 00344h, 0fcddh, 003a1h, 0fd41h, 003edh, 0fdb3h
      .word 00428h, 0fe2fh, 0044fh, 0feb2h, 00461h, 0ff38h, 0045dh, 0ffbfh
      .word 00444h, 00040h, 00417h, 000bbh, 003d6h, 00129h, 00384h, 0018ah
      .word 00325h, 001d7h, 002bbh, 00211h, 0024bh, 00234h, 001d8h, 00240h
      .word 00168h, 00233h, 000ffh, 0020fh, 000a2h, 001d4h, 00055h, 00185h
      .word 0001ch, 00125h, 0fffdh, 000b6h, 0fff8h, 0003fh, 0000fh, 0ffc6h
      .word 00046h, 0ff4ch, 0009dh, 0fedah, 00112h, 0fe76h, 001a5h, 0fe26h
      .word 00252h, 0fdf0h, 00315h, 0fddah, 003eah, 0fde8h, 004cbh, 0fe20h
      .word 005b0h, 0fe85h, 00691h, 0ff18h, 00768h, 0ffdbh, 0082bh, 000cdh
      .word 008d2h, 001efh, 00953h, 0033bh, 009a8h, 004afh, 009c7h, 00642h
      .word 009abh, 007efh, 0094dh, 009ach, 008a8h, 00b70h, 007bbh, 00d30h
      .word 00682h, 00ee1h, 004feh, 01076h, 00332h, 011e4h, 00120h, 0131fh
      .word 0fed1h, 0141bh, 0fc49h, 014cdh, 0f994h, 0152ch, 0f6bch, 0152eh
      .word 0f3cfh, 014ceh, 0f0d9h, 01404h, 0edeah, 012cfh, 0eb12h, 0112dh
      .word 0e85fh, 00f20h, 0e5e3h, 00caah, 0e3aeh, 009d2h, 0e1cch, 006a0h
      .word 0e04eh, 0031fh, 0df3eh, 0ff5ch, 0dea8h, 0fb64h, 0de94h, 0f748h
      .word 0df0ah, 0f31ah, 0e00ch, 0eeech, 0e19ch, 0ead2h, 0e3b9h, 0e6dfh
      .word 0e65eh, 0e327h, 0e985h, 0dfbeh, 0ed23h, 0dcb4h, 0f12ch, 0da1ch
      .word 0f591h, 0d802h, 0fa42h, 0d676h, 0ff2bh, 0d581h, 00437h, 0d52ch
      .word 00954h, 0d57bh, 00e69h, 0d670h, 01361h, 0d80ah, 01826h, 0da47h
      .word 01ca2h, 0dd1dh, 020c0h, 0e083h, 0246eh, 0e46dh, 0279ah, 0e8cah
      .word 02a37h, 0ed89h, 02c37h, 0f297h, 02d92h, 0f7ddh, 02e42h, 0fd46h
      .word 02e42h, 002bah, 02d92h, 00823h, 02c37h, 00d69h, 02a37h, 01277h
      .word 0279ah, 01736h, 0246eh, 01b93h, 020c0h, 01f7dh, 01ca2h, 022e3h
      .word 01826h, 025b9h, 01361h, 027f6h, 00e69h, 02990h, 00954h, 02a85h
      .word 00437h, 02ad4h, 0ff2bh, 02a7fh, 0fa42h, 0298ah, 0f591h, 027feh
      .word 0f12ch, 025e4h, 0ed23h, 0234ch, 0e985h, 02042h, 0e65eh, 01cd9h
      .word 0e3b9h, 01921h, 0e19ch, 0152eh, 0e00ch, 01114h, 0df0ah, 00ce6h
      .word 0de94h, 008b8h, 0dea8h, 0049ch, 0df3eh, 000a4h, 0e04eh, 0fce1h
      .word 0e1cch, 0f960h, 0e3aeh, 0f62eh, 0e5e3h, 0f356h, 0e85fh, 0f0e0h
      .word 0eb12h, 0eed3h, 0edeah, 0ed31h, 0f0d9h, 0ebfch, 0f3cfh, 0eb32h
      .word 0f6bch, 0ead2h, 0f994h, 0ead4h, 0fc49h, 0eb33h, 0fed1h, 0ebe5h
      .word 00120h, 0ece1h, 00332h, 0ee1ch, 004feh, 0ef8ah, 00682h, 0f11fh
      .word 007bbh, 0f2d0h, 008a8h, 0f490h, 0094dh, 0f654h, 009abh, 0f811h
      .word 009c7h, 0f9beh, 009a8h, 0fb51h, 00953h, 0fcc5h, 008d2h, 0fe11h
      .word 0082bh, 0ff33h, 00768h, 00025h, 00691h, 000e8h, 005b0h, 0017bh
      .word 004cbh, 001e0h, 003eah, 00218h, 00315h, 00226h, 00252h, 00210h
      .word 001a5h, 001dah, 00112h, 0018ah, 0009dh, 00126h, 00046h, 000b4h
      .word 0000fh, 0003ah, 0fff8h, 0ffc1h, 0fffdh, 0ff4ah, 0001ch, 0fedbh
      .word 00055h, 0fe7bh, 000a2h, 0fe2ch, 000ffh, 0fdf1h, 00168h, 0fdcdh
      .word 001d8h, 0fdc0h, 0024bh, 0fdcch, 002bbh, 0fdefh, 00325h, 0fe29h
      .word 00384h, 0fe76h, 003d6h, 0fed7h, 00417h, 0ff45h, 00444h, 0ffc0h
      .word 0045dh, 00041h, 00461h, 000c8h, 0044fh, 0014eh, 00428h, 001d1h
      .word 003edh, 0024dh, 003a1h, 002bfh, 00344h, 00323h, 002d9h, 00378h
      .word 00264h, 003bch, 001e7h, 003edh, 00167h, 0040bh, 000e5h, 00416h
      .word 00066h, 0040eh, 0ffedh, 003f4h, 0ff7ah, 003c9h, 0ff12h, 00390h
      .word 0feb7h, 0034ah, 0fe69h, 002fah, 0fe2ah, 002a2h, 0fdfah, 00247h
      .word 0fddbh, 001e8h, 0fdcah, 0018ah, 0fdc8h, 0012fh, 0fdd4h, 000d9h
      .word 0fdech, 0008ah, 0fe0eh, 00045h, 0fe38h, 00008h, 0fe69h, 0ffd8h
      .word 0fe9dh, 0ffb3h, 0fed4h, 0ff98h, 0ff0ah, 0ff89h, 0ff3fh, 0ff85h
      .word 0ff70h, 0ff8ah, 0ff9ch, 0ff98h, 0ffc1h, 0ffadh, 0ffdfh, 0ffc7h
      .word 0fff5h, 0ffe6h, 00001h, 00005h, 00006h, 00026h, 00004h, 00046h
      .word 0fffbh, 00064h, 0ffeah, 0007fh, 0ffd3h, 00094h, 0ffb9h, 000a4h
      .word 0ff9bh, 000adh, 0ff7bh, 000afh, 0ff5ch, 000abh, 0ff3ch, 000a0h
 .endif
 .if (TX_V27_MODEM_4800=ENABLED)|(RX_V27_MODEM_4800=ENABLED)
 .if $isdefed("XDAIS_API")
	.global	_VCOEF_MESI_RCOS1600f1800
 .else
	.global _RCOS1600_f1800
 .endif										;* "XDAIS_API endif
_RCOS1600_f1800:
_VCOEF_MESI_RCOS1600f1800:
      .word 00026h, 0007eh, 00012h, 00065h, 00004h, 00049h, 0fffeh, 0002ah
      .word 0ffffh, 0000ah, 00005h, 0ffedh, 00015h, 0ffcfh, 0002bh, 0ffb6h
      .word 00047h, 0ffa1h, 00069h, 0ff93h, 0008eh, 0ff8ch, 000b4h, 0ff8dh
      .word 000dch, 0ff96h, 00101h, 0ffa7h, 00123h, 0ffc1h, 0013fh, 0ffe1h
      .word 00156h, 00006h, 00165h, 00031h, 0016bh, 0005eh, 00168h, 0008dh
      .word 0015ch, 000bah, 00147h, 000e4h, 0012ah, 00108h, 00106h, 00127h
      .word 000dch, 0013ch, 000afh, 00147h, 00080h, 00148h, 00053h, 0013fh
      .word 00029h, 0012ah, 00005h, 0010ch, 0ffeah, 000e4h, 0ffd8h, 000b6h
      .word 0ffd3h, 00083h, 0ffdbh, 0004eh, 0fff0h, 00019h, 00013h, 0ffe9h
      .word 00044h, 0ffbeh, 00082h, 0ff9dh, 000cbh, 0ff89h, 0011dh, 0ff84h
      .word 00173h, 0ff90h, 001cch, 0ffaeh, 00223h, 0ffe0h, 00275h, 00025h
      .word 002bdh, 0007dh, 002f8h, 000e6h, 00322h, 0015fh, 00338h, 001e3h
      .word 00338h, 00270h, 0031fh, 00300h, 002ech, 00390h, 002a0h, 00419h
      .word 0023ah, 00499h, 001beh, 00509h, 0012eh, 00564h, 0008eh, 005a8h
      .word 0ffe3h, 005cfh, 0ff31h, 005dah, 0fe7fh, 005c6h, 0fdd2h, 00593h
      .word 0fd31h, 00541h, 0fca3h, 004d4h, 0fc2ch, 0044fh, 0fbd2h, 003b6h
      .word 0fb99h, 00311h, 0fb84h, 00265h, 0fb95h, 001bah, 0fbcch, 00118h
      .word 0fc27h, 00088h, 0fca3h, 00010h, 0fd3ch, 0ffbbh, 0fdech, 0ff8ch
      .word 0fea9h, 0ff8ah, 0ff6dh, 0ffb9h, 0002bh, 0001bh, 000dbh, 000b4h
      .word 00171h, 00180h, 001e2h, 0027dh, 00222h, 003a4h, 00228h, 004eeh
      .word 001eah, 00652h, 00162h, 007c2h, 0008ah, 00933h, 0ff61h, 00a96h
      .word 0fde2h, 00bdbh, 0fc13h, 00cf2h, 0f9f7h, 00dcch, 0f797h, 00e59h
      .word 0f4feh, 00e8ah, 0f239h, 00e54h, 0ef59h, 00dabh, 0ec6eh, 00c87h
      .word 0e98dh, 00ae3h, 0e6cch, 008bdh, 0e43fh, 00617h, 0e1feh, 002f4h
      .word 0e01ch, 0ff60h, 0deafh, 0fb65h, 0ddcah, 0f713h, 0dd7bh, 0f27eh
      .word 0ddd2h, 0edbbh, 0ded7h, 0e8e4h, 0e092h, 0e412h, 0e304h, 0df61h
      .word 0e62ah, 0daeeh, 0e9feh, 0d6d3h, 0ee75h, 0d32ch, 0f37fh, 0d013h
      .word 0f909h, 0cd9fh, 0fefbh, 0cbe3h, 00539h, 0caf2h, 00baah, 0cad5h
      .word 0122ch, 0cb98h, 018a0h, 0cd3bh, 01ee4h, 0cfbfh, 024d8h, 0d31ch
      .word 02a5ch, 0d745h, 02f54h, 0dc2ch, 033a5h, 0e1bbh, 03736h, 0e7dah
      .word 039f5h, 0ee6ch, 03bd2h, 0f552h, 03cc4h, 0fc6bh, 03cc4h, 00395h
      .word 03bd2h, 00aaeh, 039f5h, 01194h, 03736h, 01826h, 033a5h, 01e45h
      .word 02f54h, 023d4h, 02a5ch, 028bbh, 024d8h, 02ce4h, 01ee4h, 03041h
      .word 018a0h, 032c5h, 0122ch, 03468h, 00baah, 0352bh, 00539h, 0350eh
      .word 0fefbh, 0341dh, 0f909h, 03261h, 0f37fh, 02fedh, 0ee75h, 02cd4h
      .word 0e9feh, 0292dh, 0e62ah, 02512h, 0e304h, 0209fh, 0e092h, 01beeh
      .word 0ded7h, 0171ch, 0ddd2h, 01245h, 0dd7bh, 00d82h, 0ddcah, 008edh
      .word 0deafh, 0049bh, 0e01ch, 000a0h, 0e1feh, 0fd0ch, 0e43fh, 0f9e9h
      .word 0e6cch, 0f743h, 0e98dh, 0f51dh, 0ec6eh, 0f379h, 0ef59h, 0f255h
      .word 0f23ah, 0f1ach, 0f4feh, 0f176h, 0f797h, 0f1a7h, 0f9f7h, 0f234h
      .word 0fc13h, 0f30eh, 0fde2h, 0f425h, 0ff61h, 0f56ah, 0008ah, 0f6cdh
      .word 00162h, 0f83eh, 001eah, 0f9aeh, 00228h, 0fb12h, 00222h, 0fc5ch
      .word 001e2h, 0fd83h, 00171h, 0fe80h, 000dbh, 0ff4ch, 0002bh, 0ffe5h
      .word 0ff6dh, 00047h, 0fea9h, 00076h, 0fdech, 00074h, 0fd3ch, 00045h
      .word 0fca3h, 0fff0h, 0fc27h, 0ff78h, 0fbcch, 0fee8h, 0fb95h, 0fe46h
      .word 0fb84h, 0fd9bh, 0fb99h, 0fcefh, 0fbd2h, 0fc4ah, 0fc2ch, 0fbb1h
      .word 0fca3h, 0fb2ch, 0fd31h, 0fabfh, 0fdd2h, 0fa6dh, 0fe7fh, 0fa3ah
      .word 0ff31h, 0fa26h, 0ffe3h, 0fa31h, 0008eh, 0fa58h, 0012eh, 0fa9ch
      .word 001beh, 0faf7h, 0023ah, 0fb67h, 002a0h, 0fbe7h, 002ech, 0fc70h
      .word 0031fh, 0fd00h, 00338h, 0fd90h, 00338h, 0fe1dh, 00322h, 0fea1h
      .word 002f8h, 0ff1ah, 002bdh, 0ff83h, 00275h, 0ffdbh, 00223h, 00020h
      .word 001cch, 00052h, 00173h, 00070h, 0011dh, 0007ch, 000cbh, 00077h
      .word 00082h, 00063h, 00044h, 00042h, 00013h, 00017h, 0fff0h, 0ffe7h
      .word 0ffdbh, 0ffb2h, 0ffd3h, 0ff7dh, 0ffd8h, 0ff4ah, 0ffeah, 0ff1ch
      .word 00005h, 0fef4h, 00029h, 0fed6h, 00053h, 0fec1h, 00080h, 0feb8h
      .word 000afh, 0feb9h, 000dch, 0fec4h, 00106h, 0fed9h, 0012ah, 0fef8h
      .word 00147h, 0ff1ch, 0015ch, 0ff46h, 00168h, 0ff73h, 0016bh, 0ffa2h
      .word 00165h, 0ffcfh, 00156h, 0fffah, 0013fh, 0001fh, 00123h, 0003fh
      .word 00101h, 00059h, 000dch, 0006ah, 000b4h, 00073h, 0008eh, 00074h
      .word 00069h, 0006dh, 00047h, 0005fh, 0002bh, 0004ah, 00015h, 00031h
      .word 00005h, 00013h, 0ffffh, 0fff6h, 0fffeh, 0ffd6h, 00004h, 0ffb7h
      .word 00012h, 0ff9bh, 00026h, 0ff82h
 .endif
 .if (V17_MODEM=ENABLED)|(V29_MODEM=ENABLED)|(V32_MODEM=ENABLED)|(V34_2400=ENABLED)
 .if $isdefed("XDAIS_API")
	.global	_VCOEF_MESI_RCOS2400f1800
 .else
	.global _RCOS2400_f1800
 .endif										;* "XDAIS_API endif
_RCOS2400_f1800:
_VCOEF_MESI_RCOS2400f1800:
      .word 00052h, 00008h, 0003eh, 0000ah, 0002bh, 00009h, 00015h, 00006h
      .word 00002h, 00000h, 0ffefh, 0fff9h, 0ffdbh, 0ffeeh, 0ffc8h, 0ffe0h
      .word 0ffb6h, 0ffd0h, 0ffa6h, 0ffbeh, 0ff98h, 0ffa9h, 0ff8ch, 0ff93h
      .word 0ff82h, 0ff7ah, 0ff7bh, 0ff62h, 0ff77h, 0ff47h, 0ff75h, 0ff2ch
      .word 0ff77h, 0ff11h, 0ff7ch, 0fef6h, 0ff84h, 0fedch, 0ff8eh, 0fec2h
      .word 0ff9ch, 0fea9h, 0ffach, 0fe92h, 0ffbfh, 0fe7dh, 0ffd4h, 0fe6ah
      .word 0ffech, 0fe5bh, 00004h, 0fe4dh, 0001eh, 0fe44h, 00039h, 0fe3dh
      .word 00055h, 0fe3ah, 00071h, 0fe3ah, 0008dh, 0fe3dh, 000a9h, 0fe43h
      .word 000c3h, 0fe4eh, 000dbh, 0fe5dh, 000f1h, 0fe6dh, 00105h, 0fe81h
      .word 00117h, 0fe97h, 00124h, 0feb0h, 0012fh, 0fecbh, 00136h, 0fee7h
      .word 0013ah, 0ff03h, 0013ah, 0ff21h, 00135h, 0ff3fh, 0012dh, 0ff5ch
      .word 00120h, 0ff78h, 0010fh, 0ff93h, 000fbh, 0ffach, 000e3h, 0ffc3h
      .word 000c9h, 0ffd6h, 000ach, 0ffe7h, 0008ch, 0fff4h, 0006bh, 0fffdh
      .word 00047h, 00002h, 00022h, 00003h, 00000h, 00000h, 0ffdch, 0fff9h
      .word 0ffb7h, 0ffedh, 0ff95h, 0ffdch, 0ff75h, 0ffc8h, 0ff55h, 0ffb0h
      .word 0ff3ah, 0ff94h, 0ff22h, 0ff75h, 0ff0dh, 0ff53h, 0fefch, 0ff2fh
      .word 0fef0h, 0ff0ah, 0fee8h, 0fee3h, 0fee6h, 0febch, 0fee8h, 0fe95h
      .word 0feeeh, 0fe6eh, 0fef9h, 0fe49h, 0ff09h, 0fe27h, 0ff1dh, 0fe07h
      .word 0ff36h, 0fdeah, 0ff51h, 0fdd2h, 0ff70h, 0fdbeh, 0ff90h, 0fdaeh
      .word 0ffb3h, 0fda5h, 0ffd7h, 0fda0h, 0fffbh, 0fda1h, 0001dh, 0fda8h
      .word 0003fh, 0fdb5h, 0005fh, 0fdc8h, 0007ch, 0fde1h, 00096h, 0fdffh
      .word 000abh, 0fe22h, 000bah, 0fe49h, 000c4h, 0fe74h, 000c8h, 0fea3h
      .word 000c4h, 0fed4h, 000b9h, 0ff07h, 000a6h, 0ff3ah, 0008ah, 0ff6dh
      .word 00067h, 0ff9fh, 0003ch, 0ffceh, 00008h, 0fffah, 0ffcfh, 00020h
      .word 0ff8dh, 00041h, 0ff44h, 0005dh, 0fef4h, 00072h, 0fea0h, 0007eh
      .word 0fe47h, 00081h, 0fdeah, 0007ah, 0fd8ch, 00069h, 0fd2ch, 0004eh
      .word 0fccbh, 00028h, 0fc6ch, 0fff8h, 0fc11h, 0ffbbh, 0fbb9h, 0ff74h
      .word 0fb67h, 0ff22h, 0fb1bh, 0fec7h, 0fad8h, 0fe62h, 0fa9dh, 0fdf4h
      .word 0fa6dh, 0fd7fh, 0fa4ah, 0fd05h, 0fa31h, 0fc85h, 0fa26h, 0fc02h
      .word 0fa28h, 0fb7ch, 0fa38h, 0faf7h, 0fa56h, 0fa73h, 0fa83h, 0f9f2h
      .word 0fabdh, 0f976h, 0fb06h, 0f901h, 0fb5ah, 0f895h, 0fbbbh, 0f832h
      .word 0fc27h, 0f7dch, 0fc9bh, 0f793h, 0fd18h, 0f759h, 0fd9ch, 0f730h
      .word 0fe23h, 0f717h, 0feadh, 0f710h, 0ff37h, 0f71ch, 0ffbeh, 0f73ch
      .word 00040h, 0f76fh, 000bch, 0f7b3h, 0012eh, 0f80ch, 00193h, 0f876h
      .word 001eah, 0f8f2h, 00230h, 0f97ch, 00262h, 0fa14h, 0027fh, 0fab9h
      .word 00284h, 0fb67h, 0026fh, 0fc1dh, 00240h, 0fcd6h, 001f4h, 0fd92h
      .word 0018ch, 0fe4bh, 00105h, 0ff00h, 00061h, 0ffach, 0ffa0h, 0004ah
      .word 0fec0h, 000dah, 0fdc3h, 00157h, 0fcaah, 001beh, 0fb77h, 00209h
      .word 0fa2eh, 00236h, 0f8ceh, 00242h, 0f75ch, 0022ah, 0f5dbh, 001eah
      .word 0f44fh, 00180h, 0f2bbh, 000e9h, 0f125h, 00025h, 0ef8fh, 0ff32h
      .word 0edffh, 0fe0dh, 0ec7ah, 0fcb7h, 0eb06h, 0fb2fh, 0e9a7h, 0f976h
      .word 0e862h, 0f78dh, 0e73ch, 0f575h, 0e63ch, 0f331h, 0e565h, 0f0c2h
      .word 0e4beh, 0ee2ch, 0e449h, 0eb72h, 0e40dh, 0e899h, 0e40ch, 0e5a6h
      .word 0e44ah, 0e29ch, 0e4cbh, 0df81h, 0e591h, 0dc5ch, 0e69eh, 0d932h
      .word 0e7f4h, 0d609h, 0e995h, 0d2eah, 0eb80h, 0cfd9h, 0edb5h, 0ccdeh
      .word 0f032h, 0c9ffh, 0f2f8h, 0c745h, 0f604h, 0c4b6h, 0f951h, 0c258h
      .word 0fcdeh, 0c032h, 000a5h, 0be49h, 004a2h, 0bca5h, 008d1h, 0bb49h
      .word 00d2ah, 0ba3dh, 011a8h, 0b982h, 01644h, 0b91dh, 01af5h, 0b914h
      .word 01fb6h, 0b966h, 0247dh, 0ba17h, 02944h, 0bb26h, 02e01h, 0bc97h
      .word 032adh, 0be67h, 0373fh, 0c096h, 03bafh, 0c322h, 03ff5h, 0c609h
      .word 04408h, 0c947h, 047e3h, 0ccd8h, 04b7dh, 0d0b7h, 04ecfh, 0d4e0h
      .word 051d4h, 0d94ch, 05485h, 0ddf6h, 056dfh, 0e2d4h, 058d9h, 0e7e2h
      .word 05a75h, 0ed15h, 05babh, 0f267h, 05c7ch, 0f7cfh, 05ce5h, 0fd44h
      .word 05ce5h, 002bch, 05c7ch, 00831h, 05babh, 00d99h, 05a75h, 012ebh
      .word 058d9h, 0181eh, 056dfh, 01d2ch, 05485h, 0220ah, 051d4h, 026b4h
      .word 04ecfh, 02b20h, 04b7dh, 02f49h, 047e3h, 03328h, 04408h, 036b9h
      .word 03ff5h, 039f7h, 03bafh, 03cdeh, 0373fh, 03f6ah, 032adh, 04199h
      .word 02e01h, 04369h, 02944h, 044dah, 0247dh, 045e9h, 01fb6h, 0469ah
      .word 01af5h, 046ech, 01644h, 046e3h, 011a8h, 0467eh, 00d2ah, 045c3h
      .word 008d1h, 044b7h, 004a2h, 0435bh, 000a5h, 041b7h, 0fcdeh, 03fceh
      .word 0f951h, 03da8h, 0f604h, 03b4ah, 0f2f8h, 038bbh, 0f032h, 03601h
      .word 0edb5h, 03322h, 0eb80h, 03027h, 0e995h, 02d16h, 0e7f4h, 029f7h
      .word 0e69eh, 026ceh, 0e591h, 023a4h, 0e4cbh, 0207fh, 0e44ah, 01d64h
      .word 0e40ch, 01a5ah, 0e40dh, 01767h, 0e449h, 0148eh, 0e4beh, 011d4h
      .word 0e565h, 00f3eh, 0e63ch, 00ccfh, 0e73ch, 00a8bh, 0e862h, 00873h
      .word 0e9a7h, 0068ah, 0eb06h, 004d1h, 0ec7ah, 00349h, 0edffh, 001f3h
      .word 0ef8fh, 000ceh, 0f125h, 0ffdbh, 0f2bbh, 0ff17h, 0f44fh, 0fe80h
      .word 0f5dbh, 0fe16h, 0f75ch, 0fdd6h, 0f8ceh, 0fdbeh, 0fa2eh, 0fdcah
      .word 0fb77h, 0fdf7h, 0fcaah, 0fe42h, 0fdc3h, 0fea9h, 0fec0h, 0ff26h
      .word 0ffa0h, 0ffb6h, 00061h, 00054h, 00105h, 00100h, 0018ch, 001b5h
      .word 001f4h, 0026eh, 00240h, 0032ah, 0026fh, 003e3h, 00284h, 00499h
      .word 0027fh, 00547h, 00262h, 005ech, 00230h, 00684h, 001eah, 0070eh
      .word 00193h, 0078ah, 0012eh, 007f4h, 000bch, 0084dh, 00040h, 00891h
      .word 0ffbeh, 008c4h, 0ff37h, 008e4h, 0feadh, 008f0h, 0fe23h, 008e9h
      .word 0fd9ch, 008d0h, 0fd18h, 008a7h, 0fc9bh, 0086dh, 0fc27h, 00824h
      .word 0fbbbh, 007ceh, 0fb5ah, 0076bh, 0fb06h, 006ffh, 0fabdh, 0068ah
      .word 0fa83h, 0060eh, 0fa56h, 0058dh, 0fa38h, 00509h, 0fa28h, 00484h
      .word 0fa26h, 003feh, 0fa31h, 0037bh, 0fa4ah, 002fbh, 0fa6dh, 00281h
      .word 0fa9dh, 0020ch, 0fad8h, 0019eh, 0fb1bh, 00139h, 0fb67h, 000deh
      .word 0fbb9h, 0008ch, 0fc11h, 00045h, 0fc6ch, 00008h, 0fccbh, 0ffd8h
      .word 0fd2ch, 0ffb2h, 0fd8ch, 0ff97h, 0fdeah, 0ff86h, 0fe47h, 0ff7fh
      .word 0fea0h, 0ff82h, 0fef4h, 0ff8eh, 0ff44h, 0ffa3h, 0ff8dh, 0ffbfh
      .word 0ffcfh, 0ffe0h, 00008h, 00006h, 0003ch, 00032h, 00067h, 00061h
      .word 0008ah, 00093h, 000a6h, 000c6h, 000b9h, 000f9h, 000c4h, 0012ch
      .word 000c8h, 0015dh, 000c4h, 0018ch, 000bah, 001b7h, 000abh, 001deh
      .word 00096h, 00201h, 0007ch, 0021fh, 0005fh, 00238h, 0003fh, 0024bh
      .word 0001dh, 00258h, 0fffbh, 0025fh, 0ffd7h, 00260h, 0ffb3h, 0025bh
      .word 0ff90h, 00252h, 0ff70h, 00242h, 0ff51h, 0022eh, 0ff36h, 00216h
      .word 0ff1dh, 001f9h, 0ff09h, 001d9h, 0fef9h, 001b7h, 0feeeh, 00192h
      .word 0fee8h, 0016bh, 0fee6h, 00144h, 0fee8h, 0011dh, 0fef0h, 000f6h
      .word 0fefch, 000d1h, 0ff0dh, 000adh, 0ff22h, 0008bh, 0ff3ah, 0006ch
      .word 0ff55h, 00050h, 0ff75h, 00038h, 0ff95h, 00024h, 0ffb7h, 00013h
      .word 0ffdch, 00007h, 00000h, 00000h, 00022h, 0fffdh, 00047h, 0fffeh
      .word 0006bh, 00003h, 0008ch, 0000ch, 000ach, 00019h, 000c9h, 0002ah
      .word 000e3h, 0003dh, 000fbh, 00054h, 0010fh, 0006dh, 00120h, 00088h
      .word 0012dh, 000a4h, 00135h, 000c1h, 0013ah, 000dfh, 0013ah, 000fdh
      .word 00136h, 00119h, 0012fh, 00135h, 00124h, 00150h, 00117h, 00169h
      .word 00105h, 0017fh, 000f1h, 00193h, 000dbh, 001a3h, 000c3h, 001b2h
      .word 000a9h, 001bdh, 0008dh, 001c3h, 00071h, 001c6h, 00055h, 001c6h
      .word 00039h, 001c3h, 0001eh, 001bch, 00004h, 001b3h, 0ffech, 001a5h
      .word 0ffd4h, 00196h, 0ffbfh, 00183h, 0ffach, 0016eh, 0ff9ch, 00157h
      .word 0ff8eh, 0013eh, 0ff84h, 00124h, 0ff7ch, 0010ah, 0ff77h, 000efh
      .word 0ff75h, 000d4h, 0ff77h, 000b9h, 0ff7bh, 0009eh, 0ff82h, 00086h
      .word 0ff8ch, 0006dh, 0ff98h, 00057h, 0ffa6h, 00042h, 0ffb6h, 00030h
      .word 0ffc8h, 00020h, 0ffdbh, 00012h, 0ffefh, 00007h, 00002h, 00000h
      .word 00015h, 0fffah, 0002bh, 0fff7h, 0003eh, 0fff6h, 00052h, 0fff8h
 .endif
 .if (V34_3000=ENABLED)
 .if $isdefed("XDAIS_API")
	.global	_VCOEF_MESI_RCOS3000f1900
 .else
	.global _RCOS3000_f1900
 .endif										;* "XDAIS_API endif
_RCOS3000_f1900:
_VCOEF_MESI_RCOS3000f1900:
	.word 00119h, 0ff9eh, 0010bh, 0ffb6h, 000f9h, 0ffcbh, 000e5h, 0ffdeh
	.word 000cdh, 0ffefh, 000b3h, 0fffch, 00098h, 00005h, 00079h, 0000ch
	.word 0005ch, 0000fh, 0003ch, 0000dh, 0001eh, 00008h, 00000h, 00000h
	.word 0ffe3h, 0fff4h, 0ffc8h, 0ffe4h, 0ffadh, 0ffcfh, 0ff97h, 0ffb8h
	.word 0ff82h, 0ff9fh, 0ff71h, 0ff82h, 0ff64h, 0ff64h, 0ff5ah, 0ff44h
	.word 0ff54h, 0ff23h, 0ff53h, 0ff03h, 0ff55h, 0fee1h, 0ff5ch, 0fec2h
	.word 0ff67h, 0fea3h, 0ff76h, 0fe87h, 0ff88h, 0fe6dh, 0ff9eh, 0fe57h
	.word 0ffb6h, 0fe44h, 0ffd1h, 0fe34h, 0ffedh, 0fe29h, 0000ah, 0fe23h
	.word 00028h, 0fe21h, 00045h, 0fe25h, 00062h, 0fe2dh, 0007dh, 0fe3ah
	.word 00096h, 0fe4bh, 000adh, 0fe60h, 000bfh, 0fe7bh, 000ceh, 0fe97h
	.word 000d7h, 0feb8h, 000ddh, 0fed9h, 000dch, 0fefdh, 000d6h, 0ff22h
	.word 000cah, 0ff47h, 000b8h, 0ff6ch, 0009fh, 0ff8fh, 00081h, 0ffb0h
	.word 0005eh, 0ffceh, 00035h, 0ffe8h, 00007h, 0fffeh, 0ffd7h, 0000dh
	.word 0ffa1h, 00017h, 0ff69h, 0001bh, 0ff2dh, 00019h, 0fef2h, 0000fh
	.word 0feb6h, 0ffffh, 0fe7bh, 0ffe6h, 0fe41h, 0ffc6h, 0fe09h, 0ff9fh
	.word 0fdd6h, 0ff71h, 0fda8h, 0ff3ch, 0fd7fh, 0ff02h, 0fd5bh, 0fec1h
	.word 0fd3fh, 0fe7dh, 0fd2ah, 0fe35h, 0fd1fh, 0fdeah, 0fd1bh, 0fd9eh
	.word 0fd20h, 0fd51h, 0fd2fh, 0fd06h, 0fd46h, 0fcbch, 0fd67h, 0fc76h
	.word 0fd8fh, 0fc34h, 0fdc0h, 0fbf7h, 0fdf8h, 0fbc2h, 0fe36h, 0fb94h
	.word 0fe7ah, 0fb6fh, 0fec2h, 0fb53h, 0ff0dh, 0fb43h, 0ff5ah, 0fb3ch
	.word 0ffa7h, 0fb40h, 0fff3h, 0fb50h, 0003bh, 0fb6dh, 00080h, 0fb94h
	.word 000bfh, 0fbc5h, 000f7h, 0fc02h, 00126h, 0fc47h, 0014bh, 0fc95h
	.word 00164h, 0fceah, 00171h, 0fd45h, 00171h, 0fda4h, 00162h, 0fe06h
	.word 00144h, 0fe6ah, 00116h, 0fecch, 000d9h, 0ff2ch, 0008dh, 0ff86h
	.word 00032h, 0ffdah, 0ffcbh, 00023h, 0ff54h, 00063h, 0fed1h, 00097h
	.word 0fe42h, 000bdh, 0fdaah, 000d2h, 0fd0ah, 000d7h, 0fc63h, 000c9h
	.word 0fbbbh, 000a7h, 0fb0fh, 00072h, 0fa66h, 00027h, 0f9bfh, 0ffc9h
	.word 0f91fh, 0ff56h, 0f887h, 0feceh, 0f7fbh, 0fe33h, 0f77ch, 0fd86h
	.word 0f70dh, 0fcc8h, 0f6b1h, 0fbfch, 0f668h, 0fb22h, 0f634h, 0fa3dh
	.word 0f619h, 0f950h, 0f617h, 0f85fh, 0f62dh, 0f76ah, 0f65eh, 0f676h
	.word 0f6aah, 0f587h, 0f70eh, 0f49dh, 0f78dh, 0f3bfh, 0f823h, 0f2edh
	.word 0f8d0h, 0f22dh, 0f993h, 0f181h, 0fa68h, 0f0e9h, 0fb4eh, 0f06ch
	.word 0fc41h, 0f00ah, 0fd3dh, 0efc5h, 0fe41h, 0ef9fh, 0ff47h, 0ef99h
	.word 0004bh, 0efb5h, 0014ah, 0eff1h, 00240h, 0f04dh, 00327h, 0f0cch
	.word 003fdh, 0f167h, 004bch, 0f221h, 00560h, 0f2f5h, 005e7h, 0f3e0h
	.word 0064ch, 0f4e2h, 0068ch, 0f5f4h, 006a5h, 0f712h, 00693h, 0f83ah
	.word 00655h, 0f965h, 005eah, 0fa8dh, 00552h, 0fbaeh, 0048bh, 0fcc2h
	.word 00396h, 0fdc3h, 00274h, 0feadh, 00128h, 0ff77h, 0ffb7h, 0001ch
	.word 0fe1eh, 0009ah, 0fc65h, 000e9h, 0fa8eh, 00104h, 0f8a0h, 000e8h
	.word 0f6a0h, 00091h, 0f495h, 0fffch, 0f283h, 0ff24h, 0f074h, 0fe08h
	.word 0ee6eh, 0fca8h, 0ec7ah, 0fb02h, 0ea9eh, 0f918h, 0e8e2h, 0f6e9h
	.word 0e74eh, 0f478h, 0e5eah, 0f1c8h, 0e4bch, 0eedch, 0e3cdh, 0ebb9h
	.word 0e324h, 0e865h, 0e2c6h, 0e4e5h, 0e2b9h, 0e140h, 0e303h, 0dd7eh
	.word 0e3a7h, 0d9a6h, 0e4aah, 0d5c1h, 0e60fh, 0d1dbh, 0e7d6h, 0cdf9h
	.word 0ea01h, 0ca27h, 0ec90h, 0c66dh, 0ef82h, 0c2d8h, 0f2d5h, 0bf72h
	.word 0f685h, 0bc43h, 0fa8eh, 0b955h, 0feech, 0b6b0h, 00397h, 0b45fh
	.word 0088ah, 0b269h, 00dbch, 0b0d7h, 01324h, 0afaeh, 018bah, 0aef7h
	.word 01e72h, 0aeb3h, 02443h, 0aee9h, 02a21h, 0af9bh, 03001h, 0b0cah
	.word 035d8h, 0b27ah, 03b98h, 0b4a9h, 04138h, 0b754h, 046abh, 0ba7bh
	.word 04be4h, 0be1ah, 050dah, 0c22bh, 05583h, 0c6a9h, 059d3h, 0cb8dh
	.word 05dc4h, 0d0ceh, 06147h, 0d665h, 0645ch, 0dc47h, 066f6h, 0e26bh
	.word 06913h, 0e8c4h, 06aadh, 0ef49h, 06bbfh, 0f5ech, 06c48h, 0fca2h
	.word 06c48h, 0035eh, 06bbfh, 00a14h, 06aadh, 010b7h, 06913h, 0173ch
	.word 066f6h, 01d95h, 0645ch, 023b9h, 06147h, 0299bh, 05dc4h, 02f32h
	.word 059d3h, 03473h, 05583h, 03957h, 050dah, 03dd5h, 04be4h, 041e6h
	.word 046abh, 04585h, 04138h, 048ach, 03b98h, 04b57h, 035d8h, 04d86h
	.word 03001h, 04f36h, 02a21h, 05065h, 02443h, 05117h, 01e72h, 0514dh
	.word 018bah, 05109h, 01324h, 05052h, 00dbch, 04f29h, 0088ah, 04d97h
	.word 00397h, 04ba1h, 0feech, 04950h, 0fa8eh, 046abh, 0f685h, 043bdh
	.word 0f2d5h, 0408eh, 0ef82h, 03d26h, 0ec90h, 03993h, 0ea01h, 035d9h
	.word 0e7d6h, 03207h, 0e60fh, 02e25h, 0e4aah, 02a3fh, 0e3a7h, 0265ah
	.word 0e303h, 02282h, 0e2b9h, 01ec0h, 0e2c6h, 01b1bh, 0e324h, 0179bh
	.word 0e3cdh, 01447h, 0e4bch, 01124h, 0e5eah, 00e38h, 0e74eh, 00b88h
	.word 0e8e2h, 00917h, 0ea9eh, 006e8h, 0ec7ah, 004feh, 0ee6eh, 00358h
	.word 0f074h, 001f8h, 0f283h, 000dch, 0f495h, 00004h, 0f6a0h, 0ff6fh
	.word 0f8a0h, 0ff18h, 0fa8eh, 0fefch, 0fc65h, 0ff17h, 0fe1eh, 0ff66h
	.word 0ffb7h, 0ffe4h, 00128h, 00089h, 00274h, 00153h, 00396h, 0023dh
	.word 0048bh, 0033eh, 00552h, 00452h, 005eah, 00573h, 00655h, 0069bh
	.word 00693h, 007c6h, 006a5h, 008eeh, 0068ch, 00a0ch, 0064ch, 00b1eh
	.word 005e7h, 00c20h, 00560h, 00d0bh, 004bch, 00ddfh, 003fdh, 00e99h
	.word 00327h, 00f34h, 00240h, 00fb3h, 0014ah, 0100fh, 0004bh, 0104bh
	.word 0ff47h, 01067h, 0fe41h, 01061h, 0fd3dh, 0103bh, 0fc41h, 00ff6h
	.word 0fb4eh, 00f94h, 0fa68h, 00f17h, 0f993h, 00e7fh, 0f8d0h, 00dd3h
	.word 0f823h, 00d13h, 0f78dh, 00c41h, 0f70eh, 00b63h, 0f6aah, 00a79h
	.word 0f65eh, 0098ah, 0f62dh, 00896h, 0f617h, 007a1h, 0f619h, 006b0h
	.word 0f634h, 005c3h, 0f668h, 004deh, 0f6b1h, 00404h, 0f70dh, 00338h
	.word 0f77ch, 0027ah, 0f7fbh, 001cdh, 0f887h, 00132h, 0f91fh, 000aah
	.word 0f9bfh, 00037h, 0fa66h, 0ffd9h, 0fb0fh, 0ff8eh, 0fbbbh, 0ff59h
	.word 0fc63h, 0ff37h, 0fd0ah, 0ff29h, 0fdaah, 0ff2eh, 0fe42h, 0ff43h
	.word 0fed1h, 0ff69h, 0ff54h, 0ff9dh, 0ffcbh, 0ffddh, 00032h, 00026h
	.word 0008dh, 0007ah, 000d9h, 000d4h, 00116h, 00134h, 00144h, 00196h
	.word 00162h, 001fah, 00171h, 0025ch, 00171h, 002bbh, 00164h, 00316h
	.word 0014bh, 0036bh, 00126h, 003b9h, 000f7h, 003feh, 000bfh, 0043bh
	.word 00080h, 0046ch, 0003bh, 00493h, 0fff3h, 004b0h, 0ffa7h, 004c0h
	.word 0ff5ah, 004c4h, 0ff0dh, 004bdh, 0fec2h, 004adh, 0fe7ah, 00491h
	.word 0fe36h, 0046ch, 0fdf8h, 0043eh, 0fdc0h, 00409h, 0fd8fh, 003cch
	.word 0fd67h, 0038ah, 0fd46h, 00344h, 0fd2fh, 002fah, 0fd20h, 002afh
	.word 0fd1bh, 00262h, 0fd1fh, 00216h, 0fd2ah, 001cbh, 0fd3fh, 00183h
	.word 0fd5bh, 0013fh, 0fd7fh, 000feh, 0fda8h, 000c4h, 0fdd6h, 0008fh
	.word 0fe09h, 00061h, 0fe41h, 0003ah, 0fe7bh, 0001ah, 0feb6h, 00001h
	.word 0fef2h, 0fff1h, 0ff2dh, 0ffe7h, 0ff69h, 0ffe5h, 0ffa1h, 0ffe9h
	.word 0ffd7h, 0fff3h, 00007h, 00002h, 00035h, 00018h, 0005eh, 00032h
	.word 00081h, 00050h, 0009fh, 00071h, 000b8h, 00094h, 000cah, 000b9h
	.word 000d6h, 000deh, 000dch, 00103h, 000ddh, 00127h, 000d7h, 00148h
	.word 000ceh, 00169h, 000bfh, 00185h, 000adh, 001a0h, 00096h, 001b5h
	.word 0007dh, 001c6h, 00062h, 001d3h, 00045h, 001dbh, 00028h, 001dfh
	.word 0000ah, 001ddh, 0ffedh, 001d7h, 0ffd1h, 001cch, 0ffb6h, 001bch
	.word 0ff9eh, 001a9h, 0ff88h, 00193h, 0ff76h, 00179h, 0ff67h, 0015dh
	.word 0ff5ch, 0013eh, 0ff55h, 0011fh, 0ff53h, 000fdh, 0ff54h, 000ddh
	.word 0ff5ah, 000bch, 0ff64h, 0009ch, 0ff71h, 0007eh, 0ff82h, 00061h
	.word 0ff97h, 00048h, 0ffadh, 00031h, 0ffc8h, 0001ch, 0ffe3h, 0000ch
	.word 00000h, 00000h, 0001eh, 0fff8h, 0003ch, 0fff3h, 0005ch, 0fff1h
	.word 00079h, 0fff4h, 00098h, 0fffbh, 000b3h, 00004h, 000cdh, 00011h
	.word 000e5h, 00022h, 000f9h, 00035h, 0010bh, 0004ah, 00119h, 00062h
 .endif
 .if (V34_3200=ENABLED)
 .if $isdefed("XDAIS_API")
	.global	_VCOEF_MESI_RCOS3200f1875
 .else
	.global _RCOS3200_f1875
 .endif										;* "XDAIS_API endif
_RCOS3200_f1875:
_VCOEF_MESI_RCOS3200f1875:
	.word 0feabh, 0ffe4h, 0fe9ah, 0ffd1h, 0fe89h, 0ffbeh, 0fe7ch, 0ffa9h
	.word 0fe71h, 0ff93h, 0fe67h, 0ff7bh, 0fe5fh, 0ff63h, 0fe59h, 0ff4ah
	.word 0fe55h, 0ff31h, 0fe54h, 0ff17h, 0fe56h, 0fefeh, 0fe59h, 0fee4h
	.word 0fe5fh, 0fecch, 0fe67h, 0feb4h, 0fe71h, 0fe9ch, 0fe7dh, 0fe85h
	.word 0fe8ch, 0fe70h, 0fe9dh, 0fe5dh, 0feaeh, 0fe4bh, 0fec3h, 0fe3ch
	.word 0fed8h, 0fe2eh, 0feeeh, 0fe22h, 0ff06h, 0fe19h, 0ff1fh, 0fe12h
	.word 0ff38h, 0fe0eh, 0ff52h, 0fe0dh, 0ff6bh, 0fe0dh, 0ff85h, 0fe12h
	.word 0ff9eh, 0fe17h, 0ffb7h, 0fe21h, 0ffcfh, 0fe2dh, 0ffe5h, 0fe3bh
	.word 0fffah, 0fe4ch, 0000dh, 0fe5fh, 0001fh, 0fe75h, 0002eh, 0fe8dh
	.word 0003bh, 0fea7h, 00046h, 0fec1h, 0004dh, 0fedfh, 00052h, 0fefdh
	.word 00054h, 0ff1ch, 00053h, 0ff3ch, 0004fh, 0ff5bh, 00047h, 0ff7bh
	.word 0003ch, 0ff9bh, 0002eh, 0ffbah, 0001dh, 0ffd9h, 00007h, 0fff7h
	.word 0fff2h, 00010h, 0ffd8h, 0002ah, 0ffbah, 00042h, 0ff9ah, 00058h
	.word 0ff76h, 0006ch, 0ff51h, 0007ch, 0ff2ah, 0008ah, 0ff01h, 00094h
	.word 0fed8h, 0009ah, 0feabh, 0009eh, 0fe7fh, 0009dh, 0fe52h, 00099h
	.word 0fe25h, 00091h, 0fdf8h, 00084h, 0fdcbh, 00074h, 0fd9fh, 00061h
	.word 0fd75h, 00049h, 0fd4dh, 0002dh, 0fd27h, 0000eh, 0fd02h, 0ffedh
	.word 0fce1h, 0ffc7h, 0fcc1h, 0ff9eh, 0fca6h, 0ff72h, 0fc8eh, 0ff44h
	.word 0fc7ah, 0ff14h, 0fc69h, 0fee2h, 0fc5dh, 0feaeh, 0fc56h, 0fe7ah
	.word 0fc51h, 0fe44h, 0fc53h, 0fe0fh, 0fc59h, 0fddah, 0fc63h, 0fda5h
	.word 0fc72h, 0fd71h, 0fc85h, 0fd3dh, 0fc9dh, 0fd0dh, 0fcb8h, 0fcdeh
	.word 0fcd8h, 0fcb2h, 0fcfch, 0fc89h, 0fd23h, 0fc63h, 0fd4eh, 0fc42h
	.word 0fd7bh, 0fc23h, 0fdabh, 0fc0bh, 0fdddh, 0fbf5h, 0fe11h, 0fbe5h
	.word 0fe46h, 0fbdah, 0fe7dh, 0fbd4h, 0feb3h, 0fbd3h, 0feeah, 0fbd9h
	.word 0ff20h, 0fbe3h, 0ff54h, 0fbf1h, 0ff88h, 0fc07h, 0ffb9h, 0fc20h
	.word 0ffe7h, 0fc3eh, 00012h, 0fc63h, 0003ah, 0fc8bh, 0005eh, 0fcb7h
	.word 0007eh, 0fce7h, 00099h, 0fd1bh, 000afh, 0fd53h, 000bfh, 0fd8ch
	.word 000cah, 0fdc7h, 000cfh, 0fe05h, 000cdh, 0fe45h, 000c6h, 0fe84h
	.word 000b8h, 0fec3h, 000a3h, 0ff02h, 00088h, 0ff40h, 00067h, 0ff7dh
	.word 00040h, 0ffb6h, 00012h, 0ffedh, 0ffe0h, 0001fh, 0ffa7h, 0004fh
	.word 0ff69h, 00079h, 0ff26h, 0009fh, 0feddh, 000c0h, 0fe92h, 000dbh
	.word 0fe42h, 000efh, 0fdefh, 000fdh, 0fd9bh, 00103h, 0fd45h, 00102h
	.word 0fcech, 000fbh, 0fc95h, 000ebh, 0fc3dh, 000d3h, 0fbe6h, 000b4h
	.word 0fb8fh, 0008eh, 0fb3ch, 0005fh, 0faebh, 00029h, 0fa9eh, 0ffedh
	.word 0fa55h, 0ffaah, 0fa12h, 0ff5fh, 0f9d4h, 0ff0fh, 0f99bh, 0feb8h
	.word 0f969h, 0fe5dh, 0f93fh, 0fdfdh, 0f91dh, 0fd99h, 0f901h, 0fd31h
	.word 0f8efh, 0fcc7h, 0f8e3h, 0fc5ah, 0f8e3h, 0fbedh, 0f8ebh, 0fb80h
	.word 0f8fbh, 0fb12h, 0f915h, 0faa7h, 0f937h, 0fa3dh, 0f962h, 0f9d6h
	.word 0f996h, 0f972h, 0f9d2h, 0f913h, 0fa15h, 0f8b9h, 0fa61h, 0f865h
	.word 0fab3h, 0f818h, 0fb0ch, 0f7d2h, 0fb6bh, 0f795h, 0fbceh, 0f75eh
	.word 0fc36h, 0f732h, 0fca1h, 0f70fh, 0fd0fh, 0f6f4h, 0fd7fh, 0f6e5h
	.word 0fdf0h, 0f6e1h, 0fe61h, 0f6e6h, 0fed1h, 0f6f5h, 0ff3fh, 0f710h
	.word 0ffaah, 0f734h, 00010h, 0f764h, 00073h, 0f79ch, 000d0h, 0f7deh
	.word 00126h, 0f82ah, 00175h, 0f87dh, 001bbh, 0f8d9h, 001f9h, 0f93ch
	.word 0022dh, 0f9a5h, 00256h, 0fa14h, 00274h, 0fa88h, 00287h, 0faffh
	.word 0028fh, 0fb78h, 00289h, 0fbf5h, 00277h, 0fc71h, 00259h, 0fcech
	.word 0022eh, 0fd67h, 001f7h, 0fddeh, 001b3h, 0fe51h, 00162h, 0fec0h
	.word 00107h, 0ff28h, 0009fh, 0ff89h, 0002eh, 0ffe1h, 0ffb3h, 0002fh
	.word 0ff2eh, 00074h, 0fe9fh, 000aeh, 0fe09h, 000dch, 0fd6dh, 000fdh
	.word 0fccbh, 00110h, 0fc23h, 00116h, 0fb7ah, 0010dh, 0faceh, 000f5h
	.word 0fa21h, 000ceh, 0f974h, 00098h, 0f8cbh, 00052h, 0f823h, 0fffdh
	.word 0f782h, 0ff99h, 0f6e5h, 0ff26h, 0f64fh, 0fea4h, 0f5c4h, 0fe14h
	.word 0f540h, 0fd77h, 0f4c9h, 0fcceh, 0f45eh, 0fc19h, 0f400h, 0fb5ah
	.word 0f3b1h, 0fa91h, 0f370h, 0f9c0h, 0f340h, 0f8e8h, 0f31fh, 0f80ah
	.word 0f312h, 0f729h, 0f314h, 0f643h, 0f32ah, 0f55eh, 0f351h, 0f479h
	.word 0f38bh, 0f395h, 0f3d7h, 0f2b5h, 0f435h, 0f1dbh, 0f4a4h, 0f106h
	.word 0f525h, 0f03bh, 0f5b7h, 0ef7ah, 0f658h, 0eec4h, 0f707h, 0ee1bh
	.word 0f7c5h, 0ed81h, 0f88fh, 0ecf7h, 0f964h, 0ec7dh, 0fa42h, 0ec14h
	.word 0fb29h, 0ebc0h, 0fc16h, 0eb7fh, 0fd08h, 0eb51h, 0fdfdh, 0eb3ah
	.word 0fef3h, 0eb38h, 0ffe8h, 0eb4bh, 000d9h, 0eb74h, 001c7h, 0ebb3h
	.word 002aeh, 0ec08h, 0038ch, 0ec72h, 0045fh, 0ecf0h, 00525h, 0ed82h
	.word 005deh, 0ee26h, 00685h, 0eedbh, 0071bh, 0efa2h, 0079ch, 0f078h
	.word 00808h, 0f15bh, 0085eh, 0f248h, 0089bh, 0f340h, 008bfh, 0f43eh
	.word 008c9h, 0f542h, 008b8h, 0f64bh, 0088bh, 0f753h, 00842h, 0f85ah
	.word 007ddh, 0f95dh, 0075ch, 0fa59h, 006bfh, 0fb4ch, 00604h, 0fc34h
	.word 00530h, 0fd0dh, 00441h, 0fdd6h, 00338h, 0fe8bh, 00219h, 0ff2ah
	.word 000e1h, 0ffb2h, 0ff99h, 0001eh, 0fe39h, 0006fh, 0fccah, 000a2h
	.word 0fb4bh, 000b4h, 0f9c1h, 000a5h, 0f82bh, 00071h, 0f690h, 00019h
	.word 0f4f0h, 0ff9dh, 0f350h, 0fef8h, 0f1b1h, 0fe2ch, 0f018h, 0fd37h
	.word 0ee86h, 0fc1bh, 0ecffh, 0fad6h, 0eb89h, 0f96ah, 0ea24h, 0f7d6h
	.word 0e8d5h, 0f61dh, 0e79eh, 0f43dh, 0e684h, 0f23ah, 0e588h, 0f015h
	.word 0e4afh, 0edcfh, 0e3fbh, 0eb6bh, 0e370h, 0e8ech, 0e30dh, 0e653h
	.word 0e2d9h, 0e3a4h, 0e2d3h, 0e0e2h, 0e2feh, 0de0fh, 0e35dh, 0db30h
	.word 0e3efh, 0d847h, 0e4b7h, 0d559h, 0e5b6h, 0d269h, 0e6edh, 0cf7ch
	.word 0e85ch, 0cc97h, 0ea02h, 0c9bah, 0ebe1h, 0c6eeh, 0edf6h, 0c433h
	.word 0f042h, 0c18fh, 0f2c4h, 0bf07h, 0f57ah, 0bc9eh, 0f863h, 0ba58h
	.word 0fb7bh, 0b838h, 0fec2h, 0b644h, 00233h, 0b47dh, 005ceh, 0b2e9h
	.word 0098eh, 0b189h, 00d70h, 0b060h, 01170h, 0af72h, 0158bh, 0aec2h
	.word 019beh, 0ae4fh, 01e01h, 0ae20h, 02253h, 0ae32h, 026afh, 0ae89h
	.word 02b0fh, 0af24h, 02f70h, 0b006h, 033cbh, 0b12fh, 0381eh, 0b29ch
	.word 03c62h, 0b451h, 04094h, 0b64ah, 044adh, 0b887h, 048aah, 0bb08h
	.word 04c86h, 0bdc9h, 0503ch, 0c0cah, 053cah, 0c406h, 05729h, 0c77dh
	.word 05a55h, 0cb2bh, 05d4dh, 0cf0ch, 0600bh, 0d31eh, 0628dh, 0d75bh
	.word 064cfh, 0dbc2h, 066cfh, 0e04ch, 0688ah, 0e4f6h, 069ffh, 0e9bbh
	.word 06b2ch, 0ee97h, 06c0dh, 0f384h, 06ca6h, 0f87dh, 06cf1h, 0fd7fh
	.word 06cf1h, 00281h, 06ca6h, 00783h, 06c0dh, 00c7ch, 06b2ch, 01169h
	.word 069ffh, 01645h, 0688ah, 01b0ah, 066cfh, 01fb4h, 064cfh, 0243eh
	.word 0628dh, 028a5h, 0600bh, 02ce2h, 05d4dh, 030f4h, 05a55h, 034d5h
	.word 05729h, 03883h, 053cah, 03bfah, 0503ch, 03f36h, 04c86h, 04237h
	.word 048aah, 044f8h, 044adh, 04779h, 04094h, 049b6h, 03c62h, 04bafh
	.word 0381eh, 04d64h, 033cbh, 04ed1h, 02f70h, 04ffah, 02b0fh, 050dch
	.word 026afh, 05177h, 02253h, 051ceh, 01e01h, 051e0h, 019beh, 051b1h
	.word 0158bh, 0513eh, 01170h, 0508eh, 00d70h, 04fa0h, 0098eh, 04e77h
	.word 005ceh, 04d17h, 00233h, 04b82h, 0fec2h, 049bch, 0fb7bh, 047c8h
	.word 0f863h, 045a8h, 0f57ah, 04362h, 0f2c4h, 040f9h, 0f042h, 03e71h
	.word 0edf6h, 03bcdh, 0ebe1h, 03912h, 0ea02h, 03646h, 0e85ch, 03369h
	.word 0e6edh, 03084h, 0e5b6h, 02d97h, 0e4b7h, 02aa7h, 0e3efh, 027b9h
	.word 0e35dh, 024d0h, 0e2feh, 021f1h, 0e2d3h, 01f1eh, 0e2d9h, 01c5ch
	.word 0e30dh, 019adh, 0e370h, 01714h, 0e3fbh, 01495h, 0e4afh, 01231h
	.word 0e588h, 00febh, 0e684h, 00dc6h, 0e79eh, 00bc3h, 0e8d5h, 009e3h
	.word 0ea24h, 0082ah, 0eb89h, 00696h, 0ecffh, 0052ah, 0ee86h, 003e5h
	.word 0f018h, 002c9h, 0f1b1h, 001d4h, 0f350h, 00108h, 0f4f0h, 00063h
	.word 0f690h, 0ffe7h, 0f82bh, 0ff8fh, 0f9c1h, 0ff5bh, 0fb4bh, 0ff4ch
	.word 0fccah, 0ff5eh, 0fe39h, 0ff91h, 0ff99h, 0ffe2h, 000e1h, 0004eh
	.word 00219h, 000d6h, 00338h, 00175h, 00441h, 0022ah, 00530h, 002f3h
	.word 00604h, 003cch, 006bfh, 004b4h, 0075ch, 005a7h, 007ddh, 006a3h
	.word 00842h, 007a6h, 0088bh, 008adh, 008b8h, 009b5h, 008c9h, 00abeh
	.word 008bfh, 00bc2h, 0089bh, 00cc0h, 0085eh, 00db8h, 00808h, 00ea5h
	.word 0079ch, 00f88h, 0071bh, 0105eh, 00685h, 01125h, 005deh, 011dah
	.word 00525h, 0127eh, 0045fh, 01310h, 0038ch, 0138eh, 002aeh, 013f8h
	.word 001c7h, 0144dh, 000d9h, 0148ch, 0ffe8h, 014b5h, 0fef3h, 014c8h
	.word 0fdfdh, 014c6h, 0fd08h, 014afh, 0fc16h, 01481h, 0fb29h, 01440h
	.word 0fa42h, 013ech, 0f964h, 01383h, 0f88fh, 01309h, 0f7c5h, 0127fh
	.word 0f707h, 011e5h, 0f658h, 0113ch, 0f5b7h, 01086h, 0f525h, 00fc5h
	.word 0f4a4h, 00efah, 0f435h, 00e25h, 0f3d7h, 00d4bh, 0f38bh, 00c6bh
	.word 0f351h, 00b87h, 0f32ah, 00aa2h, 0f314h, 009bdh, 0f312h, 008d7h
	.word 0f31fh, 007f6h, 0f340h, 00718h, 0f370h, 00640h, 0f3b1h, 0056fh
	.word 0f400h, 004a6h, 0f45eh, 003e7h, 0f4c9h, 00332h, 0f540h, 00289h
	.word 0f5c4h, 001ech, 0f64fh, 0015ch, 0f6e5h, 000dah, 0f782h, 00067h
	.word 0f823h, 00003h, 0f8cbh, 0ffaeh, 0f974h, 0ff68h, 0fa21h, 0ff32h
	.word 0faceh, 0ff0bh, 0fb7ah, 0fef3h, 0fc23h, 0feeah, 0fccbh, 0fef0h
	.word 0fd6dh, 0ff03h, 0fe09h, 0ff24h, 0fe9fh, 0ff52h, 0ff2eh, 0ff8ch
	.word 0ffb3h, 0ffd1h, 0002eh, 0001fh, 0009fh, 00077h, 00107h, 000d8h
	.word 00162h, 00140h, 001b3h, 001afh, 001f7h, 00222h, 0022eh, 00299h
	.word 00259h, 00314h, 00277h, 0038fh, 00289h, 0040bh, 0028fh, 00488h
	.word 00287h, 00501h, 00274h, 00578h, 00256h, 005ech, 0022dh, 0065bh
	.word 001f9h, 006c4h, 001bbh, 00727h, 00175h, 00783h, 00126h, 007d6h
	.word 000d0h, 00822h, 00073h, 00864h, 00010h, 0089ch, 0ffaah, 008cch
	.word 0ff3fh, 008f0h, 0fed1h, 0090bh, 0fe61h, 0091ah, 0fdf0h, 0091fh
	.word 0fd7fh, 0091bh, 0fd0fh, 0090ch, 0fca1h, 008f1h, 0fc36h, 008ceh
	.word 0fbceh, 008a2h, 0fb6bh, 0086bh, 0fb0ch, 0082eh, 0fab3h, 007e8h
	.word 0fa61h, 0079bh, 0fa15h, 00747h, 0f9d2h, 006edh, 0f996h, 0068eh
	.word 0f962h, 0062ah, 0f937h, 005c3h, 0f915h, 00559h, 0f8fbh, 004eeh
	.word 0f8ebh, 00480h, 0f8e3h, 00413h, 0f8e3h, 003a6h, 0f8efh, 00339h
	.word 0f901h, 002cfh, 0f91dh, 00267h, 0f93fh, 00203h, 0f969h, 001a3h
	.word 0f99bh, 00148h, 0f9d4h, 000f1h, 0fa12h, 000a1h, 0fa55h, 00056h
	.word 0fa9eh, 00013h, 0faebh, 0ffd7h, 0fb3ch, 0ffa1h, 0fb8fh, 0ff72h
	.word 0fbe6h, 0ff4ch, 0fc3dh, 0ff2dh, 0fc95h, 0ff15h, 0fcech, 0ff05h
	.word 0fd45h, 0fefeh, 0fd9bh, 0fefdh, 0fdefh, 0ff03h, 0fe42h, 0ff11h
	.word 0fe92h, 0ff25h, 0feddh, 0ff40h, 0ff26h, 0ff61h, 0ff69h, 0ff87h
	.word 0ffa7h, 0ffb1h, 0ffe0h, 0ffe1h, 00012h, 00013h, 00040h, 0004ah
	.word 00067h, 00083h, 00088h, 000c0h, 000a3h, 000feh, 000b8h, 0013dh
	.word 000c6h, 0017ch, 000cdh, 001bbh, 000cfh, 001fbh, 000cah, 00239h
	.word 000bfh, 00274h, 000afh, 002adh, 00099h, 002e5h, 0007eh, 00319h
	.word 0005eh, 00349h, 0003ah, 00375h, 00012h, 0039dh, 0ffe7h, 003c2h
	.word 0ffb9h, 003e0h, 0ff88h, 003f9h, 0ff54h, 0040fh, 0ff20h, 0041dh
	.word 0feeah, 00427h, 0feb3h, 0042dh, 0fe7dh, 0042ch, 0fe46h, 00426h
	.word 0fe11h, 0041bh, 0fdddh, 0040bh, 0fdabh, 003f5h, 0fd7bh, 003ddh
	.word 0fd4eh, 003beh, 0fd23h, 0039dh, 0fcfch, 00377h, 0fcd8h, 0034eh
	.word 0fcb8h, 00322h, 0fc9dh, 002f3h, 0fc85h, 002c3h, 0fc72h, 0028fh
	.word 0fc63h, 0025bh, 0fc59h, 00226h, 0fc53h, 001f1h, 0fc51h, 001bch
	.word 0fc56h, 00186h, 0fc5dh, 00152h, 0fc69h, 0011eh, 0fc7ah, 000ech
	.word 0fc8eh, 000bch, 0fca6h, 0008eh, 0fcc1h, 00062h, 0fce1h, 00039h
	.word 0fd02h, 00013h, 0fd27h, 0fff2h, 0fd4dh, 0ffd3h, 0fd75h, 0ffb7h
	.word 0fd9fh, 0ff9fh, 0fdcbh, 0ff8ch, 0fdf8h, 0ff7ch, 0fe25h, 0ff6fh
	.word 0fe52h, 0ff67h, 0fe7fh, 0ff63h, 0feabh, 0ff62h, 0fed8h, 0ff66h
	.word 0ff01h, 0ff6ch, 0ff2ah, 0ff76h, 0ff51h, 0ff84h, 0ff76h, 0ff94h
	.word 0ff9ah, 0ffa8h, 0ffbah, 0ffbeh, 0ffd8h, 0ffd6h, 0fff2h, 0fff0h
	.word 00007h, 00009h, 0001dh, 00027h, 0002eh, 00046h, 0003ch, 00065h
	.word 00047h, 00085h, 0004fh, 000a5h, 00053h, 000c4h, 00054h, 000e4h
	.word 00052h, 00103h, 0004dh, 00121h, 00046h, 0013fh, 0003bh, 00159h
	.word 0002eh, 00173h, 0001fh, 0018bh, 0000dh, 001a1h, 0fffah, 001b4h
	.word 0ffe5h, 001c5h, 0ffcfh, 001d3h, 0ffb7h, 001dfh, 0ff9eh, 001e9h
	.word 0ff85h, 001eeh, 0ff6bh, 001f3h, 0ff52h, 001f3h, 0ff38h, 001f2h
	.word 0ff1fh, 001eeh, 0ff06h, 001e7h, 0feeeh, 001deh, 0fed8h, 001d2h
	.word 0fec3h, 001c4h, 0feaeh, 001b5h, 0fe9dh, 001a3h, 0fe8ch, 00190h
	.word 0fe7dh, 0017bh, 0fe71h, 00164h, 0fe67h, 0014ch, 0fe5fh, 00134h
	.word 0fe59h, 0011ch, 0fe56h, 00102h, 0fe54h, 000e9h, 0fe55h, 000cfh
	.word 0fe59h, 000b6h, 0fe5fh, 0009dh, 0fe67h, 00085h, 0fe71h, 0006dh
	.word 0fe7ch, 00057h, 0fe89h, 00042h, 0fe9ah, 0002fh, 0feabh, 0001ch
 .endif

;****************************************************************************
