
# define DLE      0x10
# define ETX      0x03
# define TCF      0x00
# define FILL     0x00

# define ADDR     0xff  /* HDLC address is palindromic */
# define CTLNXT   0x03  /* intermediate frame control field 11000000 */
# define CTLLST   0x13  /* final control field 11001000 */

# define DIS      0x80  /* answering capability follows 00000001 */
# define CSI      0x40  /* answering station identity follows 00000010 */
# define NSF      0x20  /* answering nonstandard facilities 00000100 */

# define DTC      0x81  /* polling capability follows 10000001 */
# define CIG      0x41  /* polling station identity follows 10000010 */
# define NSC      0x21  /* polling nonstandard facilities 10000100 */

# define DCS      0x82  /* transmitter capability follows x1000001 */
# define TSI      0x42  /* transmitter station identity follows x1000010 */
# define NSS      0x22  /* transmitter nonstandard facilities x1000100 */

# define CFR      0x84  /* confirmation to received x0100001 */
# define FTT      0x44  /* failure to train x0100010 */

# define EOM      0x8e  /* end of document x1110001 */
# define MPS      0x4e  /* end of page x1110010 */
# define EOP      0x2e  /* end of transmission x1110100 */
# define PRIEOM   0x9e  /* end of document - operator pls x1111001 */
# define PRIMPS   0x5e  /* end of page - operator pls x1111010 */
# define PRIEOP   0x3e  /* end of transmission - operator pls x1111100 */

# define MCF      0x8c  /* message confirmation x0110001 */
# define RTP      0xcc  /* message confirmation with retrain x0110011 */
# define PIP      0xac  /* message confirmation - operator pls x0110101 */
# define RTN      0x4c  /* message not received with retrain x0110010 */
# define PIN      0x2c  /* message not received - operator pls x0110100 */

# define DCN      0xfa  /* disconnect x1011111 */
# define CRP      0x1a  /* command repeat x1011000 */

/* ECM frames */

# define FCD   0x06  /* Facsimile Coded Data - 01100000 */
# define RCP   0x86  /* Return to Control for Partial page - 01100001 */
# define PPS   0xbe  /* Partial Page Signal - x1111101 */
# define PPR   0xbc  /* Partial Page Reques - x0111101 */
# define CTC   0x12  /* Continue to Correct - x1001000 */
# define CTR   0xc4  /* Continue to correct Response - x0100011 */
# define EOR   0xce  /* End Of Retransmission - x1110011 */
# define ERR   0x1c  /* End Of retransmission Response - x0111000 */
# define RR    0x6e  /* Receive Ready - x1110110 */
# define RNR   0xec  /* Receive Not Ready  - x0110111 */
# define FDM   0xfc  /* File Diagnostic Message (T.434) x0111111 */
# define NUL   0x00  /* Partial partial page 00000000 */


#define FR_MAX_FUNCTIONS 5
#define FR_MAX_TRANSITIONS 8


typedef struct MODEM_TAG
{
	IFI	char_out;
	IFI	char_out_non_block;
	IFPC string_out;
	int i_stack_driver;
	char last_char;
	char current_char;
	char frame_in[64];
	char frame_out[64];
	int in_flag[5];
	VFI driver[5]; 	
} MODEM, *PMODEM;

enum fr_states_in_tag
{
 	FR_STATE_SLEEP = 0,
 		ii1,
		ii2,
		ii3,
		ii4,
		ii5,
		ii6,
		ii7,
		ii8,
		ii9,
		ii10,
		ii11,
		ii12,
		ii13,
		ii14,
		ii15,
		ii16,
		ii17,
		ii18,
		ii19,
		ii20,
		ii21,
		ii22,
		ii23,
		ii24,
		ii25,
		ii26,
		ii27,
		ii28,
		ii29,
		ii30,
		ii31,
		ii32,
		ii33,
		ii34,
		ii35,
		ii36,
		ii37,
		ii38
};

enum fr_states_out_tag
{
 		o0 = 0,
 		oo1,
		oo2,
		oo3,
		oo4,
		oo5,
		oo6,
		oo7,
		oo8,
		oo9,
		oo10,
		oo11,
		oo12,
		oo13,
		oo14,
		oo15,
		oo16,
		oo17,
		oo18,
		oo19,
		oo20,
		oo21,
		oo22,
		oo23,
		oo24,
		oo25,
		oo26,
		oo27,
		oo28,
		oo29,
		oo30,
		oo31,
		oo32,
		oo33,
		oo34,
		oo35,
		oo36,
		oo37,
		oo38,
		oo39,
		oo40
};


