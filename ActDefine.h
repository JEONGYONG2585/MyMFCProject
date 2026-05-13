/// CPU TYPE
// QnACPU
#define		CPU_Q2ACPU								0x0011					// Q2A
#define		CPU_Q2AS1CPU							0x0012					// Q2AS1
#define		CPU_Q3ACPU								0x0013					// Q3A
#define		CPU_Q4ACPU								0x0014					// Q4A
// QCPU Q
#define		CPU_Q02CPU								0x0022					// Q02(H) Q
#define		CPU_Q06CPU								0x0023					// Q06H   Q
#define		CPU_Q12CPU								0x0024					// Q12H   Q
#define		CPU_Q25CPU								0x0025					// Q25H   Q
#define		CPU_Q00JCPU								0x0030					// Q00J	  Q
#define		CPU_Q00CPU								0x0031					// Q00	  Q
#define		CPU_Q01CPU								0x0032					// Q01	  Q
#define		CPU_Q12PHCPU							0x0041					// Q12PHCPU Q
#define		CPU_Q25PHCPU							0x0042					// Q25PHCPU Q
#define		CPU_Q25SSCPU							0x0055					// Q25SS
#define		CPU_Q06UDHCPU							0x0072					// Q06UDHCPU

// ACPU
#define		CPU_A0J2HCPU							0x0102					// A0J2H
#define		CPU_A1FXCPU								0x0103					// A1FX
#define		CPU_A1SCPU								0x0104					// A1S,A1SJ
#define		CPU_A1SHCPU								0x0105					// A1SH,A1SJH
#define		CPU_A1NCPU								0x0106					// A1(N)
#define		CPU_A2CCPU								0x0107					// A2C,A2CJ
#define		CPU_A2NCPU								0x0108					// A2(N),A2S
#define		CPU_A2SHCPU								0x0109					// A2SH
#define		CPU_A3NCPU								0x010A					// A3(N)
#define		CPU_A2ACPU								0x010C					// A2A
#define		CPU_A3ACPU								0x010D					// A3A
#define		CPU_A2UCPU								0x010E					// A2U,A2US
#define		CPU_A2USHS1CPU							0x010F					// A2USHS1
#define		CPU_A3UCPU								0x0110					// A3U
#define		CPU_A4UCPU								0x0111					// A4U
// QCPU A
#define		CPU_Q02CPU_A							0x0141					// Q02(H)
#define		CPU_Q06CPU_A							0x0142					// Q06H
// FXCPU
#define		CPU_FX0CPU								0x0201					// FX0/FX0S
#define		CPU_FX0NCPU								0x0202					// FX0N
#define		CPU_FX1CPU								0x0203					// FX1
#define		CPU_FX2CPU								0x0204					// FX2/FX2C
#define		CPU_FX2NCPU								0x0205					// FX2N/FX2NC
#define		CPU_FX1SCPU								0x0206					// FX1S
#define		CPU_FX1NCPU								0x0207					// FX1N
// BOARD
#define		CPU_BOARD								0x0401					// NETWORK BOARD
// MOTION 	
#define		CPU_A171SHCPU							0x0601					// A171SH
#define		CPU_A172SHCPU							0x0602					// A172SH
#define		CPU_A273UHCPU							0x0603					// A273UH
#define		CPU_A173UHCPU							0x0604					// A173UH
// GOT
#define		CPU_A900GOT								0x0701					// A900GOT

/// PORT
#define		PORT_1								    0x01    				// CommunicationPort1
#define		PORT_2								    0x02    				// CommunicationPort2
#define		PORT_3								    0x03    				// CommunicationPort3
#define		PORT_4								    0x04    				// CommunicationPort4
#define		PORT_5								    0x05    				// CommunicationPort5
#define		PORT_6								    0x06    				// CommunicationPort6
#define		PORT_7								    0x07    				// CommunicationPort7
#define		PORT_8								    0x08    				// CommunicationPort8
#define		PORT_9								    0x09    				// CommunicationPort9
#define		PORT_10								    0x0A					// CommunicationPort10


/// BAUDRATE
#define		BAUDRATE_300							CBR_300     	       	// 300bps
#define		BAUDRATE_600							CBR_600  	          	// 600bps
#define		BAUDRATE_1200							CBR_1200            	// 1200bps
#define		BAUDRATE_2400							CBR_2400            	// 2400bps
#define		BAUDRATE_4800							CBR_4800            	// 4800bps
#define 	BAUDRATE_9600							CBR_9600            	// 9600bps
#define 	BAUDRATE_19200							CBR_19200           	// 19200bps
#define 	BAUDRATE_38400							CBR_38400           	// 38400bps
#define 	BAUDRATE_57600							CBR_57600           	// 57600bps
#define 	BAUDRATE_115200							CBR_115200          	// 115200bps


/// DATA BIT
#define 	DATABIT_7								7						// DATA BIT 7
#define 	DATABIT_8								8						// DATA BIT 8


/// PARITY
#define 	NO_PARRITY								NOPARITY            	// NO PARITY
#define 	ODD_PARITY								ODDPARITY           	// ODD PARITY
#define 	EVEN_PARITY								EVENPARITY          	// EVEN PARITY


/// STOP BITS
#define 	STOPBIT_ONE								ONESTOPBIT          	// 1 STOP BIT
#define 	STOPBIT_TWO								TWOSTOPBITS         	// 2 STOP BIT


/// SERIAL CONTROL
#define		TRC_DTR									0x01					// DTR
#define		TRC_RTS									0x02					// RTS
#define		TRC_DTR_AND_RTS							0x07					// DTR and RTS 
#define		TRC_DTR_OR_RTS							0x08					// DTR or RTS


/// SUM CHECK
#define SUM_CHECK									1						// Sum Check
#define NO_SUM_CHECK								0						// No Sum Check


///PACKET TYPE
#define		PACKET_ASCII							0x02					//PACKET TYPE ASCII
#define		PACKET_BINARY							0x03					//PACKET TYPE BINARY

///CONNECT WAY
#define		TEL_AUTO_CONNECT						0x00					// AUTO LINE CONNECT
#define		TEL_AUTO_CALLBACK						0x01					// AUTO LINE CONNECT(CALLBACK FIXATION)
#define		TEL_AUTO_CALLBACK_NUMBER				0x02					// AUTO LINE CONNECT(CALLBACK NUMBER SPECIFICATION)
#define		TEL_CALLBACK							0x03					// CALLBACK CONNECT(FIXATION)
#define		TEL_CALLBACK_NUMBER						0x04					// CALLBACK CONNECT(NUMBER SPECIFICATION)
#define		TEL_CALLBACK_REQUEST					0x05					// CALLBACK REQUEST(FIXATION)
#define		TEL_CALLBACK_REQUEST_NUMBER				0x06					// CALLBACK REQUEST(NUMBER SPECIFICATION)
#define		TEL_CALLBACK_WAIT						0x07					// CALLBACK RECEPTION WAITING

///LINE TYPE
#define		LINETYPE_PULSE							0x00					// PULSE
#define		LINETYPE_TONE							0x01					// TONE
#define		LINETYPE_ISDN							0x02					// ISDN
