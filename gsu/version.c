#include "su.h"
/************************* Version Control Header *****************************
Revision Number			Revision Description

1.00					Initial release version under control of this mechanism
						This version resolves all known bugs related to
						the Rev C subscriber unit board and is ready for
						integration of fax/modem support.

1.01					Resolved switch hook detection problem.

1.02					New extended SS setup mechanism introduced, RSSI level
						indication added, SLAC initialization improved and old
						debug codes cleaned up.
						
1.10					Fax/Modem integration.

1.11					Fax/Modem indications, CID and Slac improvements and
						Incomming bearer selection.

1.12					User interface updades, modem flow control fixes -Debug
						version, never released!!!
						
1.13					User interface updades, modem flow control fixes, Fax portion
						debuged in Montreal and Caller ID fixes for RSSI display added.

1.14					Alex's Elsalvador Fixes, Disallowing of NSF frames

1.15					Ringback during a fax call, continious ringing problems are fixed, 
						continious echo adaptation disabled during conferance call.

1.16					AutoPIN and Service Providers SS setup sequneces (*SS) support is added,
						Fax state machine is improved based on Elsalvador tests.

1.17					Bug fix release. Slac and CallerID modules are improved with software debounce,
						polling for hook-status, CID messages and slac state machine integration.
						CallerID low level drivers are updated. 
						
1.18					Bug fix release. New Faxrelay statemashine...

1.19					Alex's Mexico fax fixes left in Mexico. Also, slac bug fix
						that fixes caller id on call waiting.  Additional, 
						shipment to Graham Shepard for demos.

1.20					SMS read/list/delete, seting of DATABEARER via dialing sequence and status
						check for AUTOPIN features are added.

1.21					Autoboud on UARTB, GPRS releated rate and initialization changes are implemented.
						This will be the last release for  Rev. 03 boards.

1.22					Bug fix relese. Negative acknowledgement tone is integrated with the CallControl 
						Flash table is fixed for AUTOPIN enrty. Autoboud and GPRS releated changes rolled back
						form 1.21 version which is never released.

1.23					Analog and digital FAX updates. Flow control improvements 

1.24					Relese version after Mexico City tests with Telcel.

1.24i					Merged dual board updates, autobaud, and various unrelease
						bugfixes with latest Fax updates.  This merges resolves previously 
						releases branched in QVCS.

1.24k					CLIR, Version info on CallerID, SMS improvements, power saving, 
						pulse dialing improvements and low speed anlog modem support are added.
						
1.25					User configurable RS232 settings are introduced. (IPR, ICF, S0). 
						Battery status LED update problem is fixed, Caller ID presentation fixed for Rev.03 boards.

1.26					Flash detection and fax/lower speed analog modem integration fixes.	

1.27					GPRS support, Wavecom-Flash parameters synchronization fix and inband bussy tone
						suppression.

1.28					Call Metering, Call Related Supplementary Services improvements (flash), RS-232 speed
 						settings via phone interface, Call Waiting Caller ID improvements, power saving
 						improvements,  analog modem mode change fixes, +++ escape char filtering
 						and cmd file update for unused memory utilization. 
1.28a					Flash and Call Related Supplementary Services improvements.
1.28b					Added autobaud capability for UART B.
1.28c					Added AT+IPR=0 to AT+IPR=115200 dynamic substitution
						providing fixed 115200 bps connection to WAVECOM due to its
						inability to handle autobaud with rates higher then 38400 bps.
1.28d					SIM presence checking added to startup and Uart_A settings changed to 19200 for consistency.
1.28e					Fix for near pointer call in on_timer_functionCall() function.
1.28f					SIM presence checking added for cases where SIM requires 
						either PIN or PUK.  Also, allow incoming RING message to be 
						displayed while autobauding.
1.28g					Busy modem handling in analog data modem calls. 
1.29					Internal Release: Support for DTMF and V.23 FSK caller id
						added.  Also, power reduction in dsp clock, SLAC, and DUART.

2.00					Release for new hardware platform. (new boards with 2.02 firmware)
2.01					Audio gain adjustment for SLAC and Wavecom Modules, Wavecom Module platform detection,
						wake-up after SIM insertion and serial cable connection, Ringback test, 
						default CallerID protocol is to ETSI V.23, dialing sequence for forced power saving,
						Polarity Reversal support for payphone integration 

2.02					Fixes for synchronous modem operation at 4800 and 7200 bps
						SLAC channel 2 is moved to an unused timeslot during fax/data calls

2.03					Flexible charging indications support, no reset after RS232 cable removal.
2.04					Adjusted SLAC gains to improve far-end audio levels; adjusted SLAC's 
						autobattery selection to minimize "clacking" units; fixed home units scaling
						factor value stored into flash; and added AT+LCRG? to query line supervison
						values stored in flash.
						
2.04a					BER reporting added to RSSI display, Data Bearer handling updated and default
						bearer is set to Transparent preferred (CBST=0,0,2),autobaud problems are fixed,
						fax state machine updated for unhandled error conditions, fax call
						initiation and relese phases are improved by fixing the coordination problems with
						call control, general cleanup to remove unused code frangments and global variables,
						debug mode intialization is improved.

2.05                    Periodic re-registration process is added for Wavecom Module sleep problem.

2.06					vxx_tx_modem_ready corrected (no more pipe concurrency),
						v.14 transmit framer extra flag stuffing corrected,
						entered gain control dial sequences:
						slac channel 1 rx: ###1#1#00 - -12dB
											....
										   ###1#1#12 - +0dB
										   
						slac channel 1 tx: ###1#2#00 - +0dB
											....
										   ###1#2#12 - +12dB
										   
						slac channel 2 rx: ###2#1#00 - -12dB
											....
										   ###2#1#12 - +0dB
						
						slac channel 2 tx: ###2#2#00 - +0dB
											....
										   ###2#2#12 - +12dB
						
						Wavecom rx (at+vgr=***, page 48 AT comand interface): 
										   ###3#1#255 - -24dB
											....
										   ###3#1#000 - +6dB
										   
						Wavecom tx (at+vgt=***, page 48 AT comand interface):
										   ###3#2#000 - +30dB
										   	....
										   ###3#2#255 - +51dB
2.06a					All ###... sequences are available only in DEBUG mode.
						#*71*1 lower telephone volume
						#*71*5 higher telephone volume
						#*72*1 minimum microphone gain
						#*72*3 maximum microphone gain
						Compiled with -d"_PAYPHONE_" -d"_UART_B_DSR_" keys to provide pay phone requirements.				   
********************************************************************************/

#define FPGAVERSION portD000
volatile ioport u16 FPGAVERSION;

char SWversion[40]="";
char VersionNumber[6]="2.06a";

void set_version(void)
{
	unsigned int fpga_major;
	unsigned int fpga_minor;
	unsigned int fpga_reg;
	char fpga[6]=""; 
	
	fpga_reg = FPGAVERSION; 
	fpga_major= (fpga_reg >> 4) & 0x0f;
	fpga_minor= fpga_reg & 0x0f;
	sprintf(fpga,"%d.%02d", fpga_major, fpga_minor); 

	/* For each new software release update the version number and board revison. 
	   x.xx 	- Software version (feature.bugfixes)       
	   x.xx		- FPGA firmware revision
	*/  

	sprintf(SWversion,"\r%s %s %s %s \r\n", VersionNumber, fpga, __DATE__, __TIME__);

}
