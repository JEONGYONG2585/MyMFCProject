#ifndef __IODEF_H__
#define	__IODEF_H__

/////////////////////////////////////////////////////////////////////////////////////////
////////////	input
/////////////////////////////////////////////////////////////////////////////////////////

// Board #1 Input ......Low Bits
		//IONIZER ALARM
		//1.Level Alarm : Ion 발생량이 저감, 오염되었을경우 발생됨.
		//2.Condi Alarm : 주변에 금속체가 가깝거나,많을경우 발생됨. (대게..Condi Alarm발생전에 Level Alarm이 먼저 발생한다고함.)
		//3.Normal Alarm: 고장, 전원Off, 내부회로 단선시 발생됨. (N.C : 평상시 On상태임)
		#define	IONIZER1_ALARM1		0x00000001	// 1-0 : Ionizer Level Alarm (N.O)
		#define	IONIZER1_ALARM2		0x00000002	// 1-1 : Ionizer Condition Alarm (N.O)
		#define	IONIZER1_ALARM3		0x00000004	// 1-2 : Ionizer Normal Alarm (N.C)

		#define	IONIZER2_ALARM1		0x00000010	// 1-4 : Ionizer Level Alarm (N.O)
		#define	IONIZER2_ALARM2		0x00000020	// 1-5 : Ionizer Condition Alarm (N.O)
		#define	IONIZER2_ALARM3		0x00000040	// 1-6 : Ionizer Normal Alarm (N.C)

		#define	LINE_DORP_COM_IN	7			// 1-7
		#define	LC_LIMIT1			8			// 1-8
		#define	LC_LIMIT2			9			// 1-9
		#define	LC_LIMIT3			10			// 1-10
		#define	LC_LIMIT4			11			// 1-11
		#define	LC_LIMIT5			12			// 1-12
		#define	LC_LIMIT6			13			// 1-13
		#define	LC_LIMIT7			14			// 1-14
		#define	LC_LIMIT8			15			// 1-15
		#define	LC_LIMIT9			16			// 1-16
		#define	LC_LIMIT10			17			// 1-17
		#define	LC_LIMIT11			18			// 1-18
		#define	LC_LIMIT12			19			// 1-19
		#define	LC_LIMIT13			20			// 1-20
		#define	LC_LIMIT14			21			// 1-21
		#define	LC_LIMIT15			22			// 1-22
		#define	LC_LIMIT16			23			// 1-23
		#define	ASSEMBLE_SENSOR1	24			// 1-24
		#define	ASSEMBLE_SENSOR2	25			// 1-25
		#define	ASSEMBLE_SENSOR3	26			// 1-26
		#define	ASSEMBLE_SENSOR4	27			// 1-27
		#define	ASSEMBLE_SENSOR5	28			// 1-28
		#define	ASSEMBLE_SENSOR6	29			// 1-29
		#define	ASSEMBLE_SENSOR7	30			// 1-30
		#define	ASSEMBLE_SENSOR8	31			// 1-31
// Board #1 Input ......High Bits
		#define	ASSEMBLE_SENSOR9	0			// 1-32
		#define	ASSEMBLE_SENSOR10	1			// 1-33
		#define	ASSEMBLE_SENSOR11	2			// 1-34
		#define	ASSEMBLE_SENSOR12	3			// 1-35
		#define	ASSEMBLE_SENSOR13	4			// 1-36
		#define	ASSEMBLE_SENSOR14	5			// 1-37
		#define	ASSEMBLE_SENSOR15	6			// 1-38
		#define	ASSEMBLE_SENSOR16	7			// 1-39
		#define	DOOR_SENSOR1		8			// 1-40
		#define	DOOR_SENSOR2		9			// 1-41
		#define	DOOR_SENSOR3		10			// 1-42
		#define	DOOR_SENSOR1234		0x00002700	// 1-40,41,42,45
		#define DOOR_SENSOR23       0x00000600  // 1-41,42
		#define	ROBOT_ARM_DETECT	11			// 1-43  0x0000800
		#define IONIZER_N2_PRESS	12			// 1-44
		#define MAINT_DOOR			13			// 1-45
		#define INTERLOCK_OFF		14			// 1-46
		#define GLASS_DETECT		15			// 1-47
		#define	DROP_CHK1			16			// 1-48
		#define	DROP_CHK2			17			// 1-49
		#define	DROP_CHK3			18			// 1-50
		#define	DROP_CHK4			19			// 1-51
		#define	DROP_CHK5			20			// 1-52
		#define	DROP_CHK6			21			// 1-53
		#define	DROP_CHK7			22			// 1-54
		#define	DROP_CHK8			23			// 1-55
		#define	DROP_CHK9			24			// 1-56
		#define	DROP_CHK10			25			// 1-57
		#define	DROP_CHK11			26			// 1-58
		#define	DROP_CHK12			27			// 1-59
		#define	DROP_CHK13			28			// 1-60
		#define	DROP_CHK14			29			// 1-61
		#define	DROP_CHK15			30			// 1-62
		#define	DROP_CHK16			31			// 1-63


// Board #2 Input ......Low Bits
		#define	UP_SWITCH			0x00000001	// 2-0
		#define	DOWN_SWITCH			0x00000002	// 2-1
		#define LEFT_SWITCH			0x00000004	// 2-2
		#define RIGHT_SWITCH		0x00000008	// 2-3
		#define OK_SWITCH			0x00000010	// 2-4
		#define UDRL_SWITCH			0x0000000f  // 4-5,6,7,8
		#define	START_SWITCH		5			// 2-5
		#define DROP_END			6			// 2-6
		#define	STOP_SWITCH			7			// 2-7
		#define MAIN_CDA			8			// 2-8
		#define MAIN_VACCUM			9			// 2-9
		#define STAGE_VACUUM_PRESS	10			// 2-10
		#define STAGE_PURGE_PRESS1	11			// 2-11
		#define AREA1				9			// 2-9
		#define AREA2				10			// 2-10
		#define AREA3				11			// 2-11
		#define PISTON_PRESS		12			// 2-12
		#define MANUAL_MODE			13			// 2-13
		#define AUTO_MODE	        14		    // 2-14
		#define MUTING_ON			15			// 2-15
		#define SUPPLY_MIN1			16			// 2-16
		#define SUPPLY_MIN2			17			// 2-17
		#define SUPPLY_MIN3			18			// 2-18
		#define SUPPLY_MIN4			19			// 2-19
		#define SUPPLY_MIN5			20			// 2-20
		#define SUPPLY_MIN6			21			// 2-21
		#define SUPPLY_MIN7			22			// 2-22
		#define SUPPLY_MIN8			23			// 2-23
		#define SUPPLY_MIN9			24			// 2-24
		#define SUPPLY_MIN10		25			// 2-25
		#define SUPPLY_MIN11		26			// 2-26
		#define SUPPLY_MIN12		27			// 2-27
		#define SUPPLY_MIN13		28			// 2-28
		#define SUPPLY_MIN14		29			// 2-29
		#define SUPPLY_MIN15		30			// 2-30
		#define SUPPLY_MIN16		31			// 2-31

// Board #2 Input ......High Bits
		#define SUPPLY_MAX1			0			// 2-32
		#define SUPPLY_MAX2			1			// 2-33
		#define SUPPLY_MAX3			2			// 2-34
		#define SUPPLY_MAX4			3			// 2-35
		#define SUPPLY_MAX5			4			// 2-36
		#define SUPPLY_MAX6			5			// 2-37
		#define SUPPLY_MAX7			6			// 2-38
		#define SUPPLY_MAX8			7			// 2-39
		#define SUPPLY_MAX9			8			// 2-40
		#define SUPPLY_MAX10		9			// 2-41
		#define SUPPLY_MAX11		10			// 2-42
		#define SUPPLY_MAX12		11			// 2-43
		#define SUPPLY_MAX13		12			// 2-44
		#define SUPPLY_MAX14		13			// 2-45
		#define SUPPLY_MAX15		14			// 2-46
		#define SUPPLY_MAX16		15			// 2-47
		#define HEAD1_UP_SENSOR		16			// 2-48
		#define HEAD2_UP_SENSOR		17			// 2-49
		#define HEAD3_UP_SENSOR		18			// 2-50
		#define HEAD4_UP_SENSOR		19			// 2-51
		#define HEAD5_UP_SENSOR		20			// 2-52
		#define HEAD6_UP_SENSOR		21			// 2-53
		#define HEAD7_UP_SENSOR		22			// 2-54
		#define HEAD8_UP_SENSOR		23			// 2-55
		#define HEAD9_UP_SENSOR		24			// 2-56
		#define HEAD10_UP_SENSOR	25			// 2-57
		#define HEAD11_UP_SENSOR	26			// 2-58
		#define HEAD12_UP_SENSOR	27			// 2-59
		#define HEAD13_UP_SENSOR	28			// 2-60
		#define HEAD14_UP_SENSOR	29			// 2-61
		#define HEAD15_UP_SENSOR	30			// 2-62
		#define HEAD16_UP_SENSOR	31			// 2-63

// Board #3 Input ......Low Bits
		#define HEAD1_DW_SENSOR		0			// 3-0
		#define HEAD2_DW_SENSOR		1			// 3-1
		#define HEAD3_DW_SENSOR		2			// 3-2
		#define HEAD4_DW_SENSOR		3			// 3-3
		#define HEAD5_DW_SENSOR		4			// 3-4
		#define HEAD6_DW_SENSOR		5			// 3-5
		#define HEAD7_DW_SENSOR		6			// 3-6
		#define HEAD8_DW_SENSOR		7			// 3-7
		#define HEAD9_DW_SENSOR		8			// 3-8
		#define HEAD10_DW_SENSOR	9			// 3-9
		#define HEAD11_DW_SENSOR	10			// 3-10
		#define HEAD12_DW_SENSOR	11			// 3-11
		#define HEAD13_DW_SENSOR	12			// 3-12
		#define HEAD14_DW_SENSOR	13			// 3-13
		#define HEAD15_DW_SENSOR	14			// 3-14
		#define HEAD16_DW_SENSOR	15			// 3-15
		#define GATE1_OPEN_POSITION	16			// 3-16
		#define GATE2_OPEN_POSITION	17			// 3-17
		#define GATE3_OPEN_POSITION	18			// 3-18
		#define GATE4_OPEN_POSITION	19			// 3-19
		#define GATE5_OPEN_POSITION	20			// 3-20
		#define GATE6_OPEN_POSITION	21			// 3-21
		#define GATE7_OPEN_POSITION	22			// 3-22
		#define GATE8_OPEN_POSITION	23			// 3-23
		#define GATE9_OPEN_POSITION	24			// 3-24
		#define GATE10_OPEN_POSITION 25			// 3-25
		#define GATE11_OPEN_POSITION 26			// 3-26
		#define GATE12_OPEN_POSITION 27			// 3-27
		#define GATE13_OPEN_POSITION 28			// 3-28
		#define GATE14_OPEN_POSITION 29			// 3-29
		#define GATE15_OPEN_POSITION 30			// 3-30
		#define GATE16_OPEN_POSITION 31			// 3-31

// Board #3 Input ......High Bits
		#define GATE1_CLOSE_POSITION 0			// 3-32
		#define GATE2_CLOSE_POSITION 1			// 3-33
		#define GATE3_CLOSE_POSITION 2			// 3-34
		#define GATE4_CLOSE_POSITION 3			// 3-35
		#define GATE5_CLOSE_POSITION 4			// 3-36
		#define GATE6_CLOSE_POSITION 5			// 3-37
		#define GATE7_CLOSE_POSITION 6			// 3-38
		#define GATE8_CLOSE_POSITION 7			// 3-39
		#define GATE9_CLOSE_POSITION 8			// 3-40
		#define GATE10_CLOSE_POSITION 9			// 3-41
		#define GATE11_CLOSE_POSITION 10		// 3-42
		#define GATE12_CLOSE_POSITION 11		// 3-43
		#define GATE13_CLOSE_POSITION 12		// 3-44
		#define GATE14_CLOSE_POSITION 13		// 3-45
		#define GATE15_CLOSE_POSITION 14		// 3-46
		#define GATE16_CLOSE_POSITION 15		// 3-47
		#define NOZZLE_FORWARD1		 20			// 3-52
		#define NOZZLE_FORWARD2		 21			// 3-53
		#define NOZZLE_BACKWARD1	 22			// 3-54
		#define NOZZLE_BACKWARD2	 23			// 3-55
		#define PINCH_OPEN1			 24			// 3-56
		#define PINCH_OPEN2			 25			// 3-57
		#define PINCH_CLOSE1		 26			// 3-58
		#define PINCH_CLOSE2		 27			// 3-59
		#define SUPPLY_CDA1			 28			// 3-60
		#define SUPPLY_CDA2			 29			// 3-61
		#define SUPPLY_INIT_CDA1	 30			// 3-62
		#define SUPPLY_INIT_CDA2	 31			// 3-63


// Board #4 Input ......Low Bits
		#define LOAD_UNLOAD_SENSOR	  0	        // 4-0 		//IO 확인 필요. 10 확인 완료.
		#define	POWER_ON			  1			// 4-1 //Power On 상태 이면 14,15 input이 전부 들어 옴 ...뭔가 이상함 나중에 확인 할것...
		#define	READY_SWITCH		  2			// 4-2

		#define	FAN_ALARM1			  3			// 4-3
		#define	FAN_ALARM2			  4			// 4-4
		#define	FAN_ALARM3			  5			// 4-5
		#define FAN_ALARM4			  6			// 4-6
		#define	EMO1				  8		    // 4-8  //Ready ON 하면 off된다.. //EMO1
		#define EMO2				  9			// 4-9  //EMO2
		#define EMO3				 10			// 4-10 //EMO3
		#define EMO4				 11			// 4-11 //EMO4
		#define EMO5				 12			// 4-12 //EMO5
		#define EMO6				 13			// 4-13 //EMO6
		#define EMO7				 14			// 4-14 //EMO7
		#define SYLINDER1_UP_SENSOR  16			// 4-16
		#define SYLINDER2_UP_SENSOR  17			// 4-17
		#define SYLINDER3_UP_SENSOR  18			// 4-18
		#define SYLINDER4_UP_SENSOR  19			// 4-19
		#define SYLINDER5_UP_SENSOR  20			// 4-20
		#define SYLINDER6_UP_SENSOR  21			// 4-21
		#define SYLINDER7_UP_SENSOR  22			// 4-22
		#define SYLINDER8_UP_SENSOR  23			// 4-23
		#define SYLINDER1_DW_SENSOR  24			// 4-24
		#define SYLINDER2_DW_SENSOR  25			// 4-25
		#define SYLINDER3_DW_SENSOR  26			// 4-26
		#define SYLINDER4_DW_SENSOR  27			// 4-27
		#define SYLINDER5_DW_SENSOR  28			// 4-28
		#define SYLINDER6_DW_SENSOR  29			// 4-29
		#define SYLINDER7_DW_SENSOR  30			// 4-30
		#define SYLINDER8_DW_SENSOR  31			// 4-31

/////////////////////////////////////////////////////////////////////////////////////////
////////////	output
/////////////////////////////////////////////////////////////////////////////////////////
// Board #1 Outputs.......Low Bits
		#define DROP_START      	6		    // 1-6 
		#define LINE_DORP_COM_OUT	7		    // 1-7 
		#define N_START_TRIGGER_01	8           // 1-8
		#define N_START_TRIGGER_02	9           // 1-9
		#define N_START_TRIGGER_03	10          // 1-10
		#define N_START_TRIGGER_04	11          // 1-11
		#define N_START_TRIGGER_05	12          // 1-12
		#define N_START_TRIGGER_06	13          // 1-13
		#define N_START_TRIGGER_07	14          // 1-14
		#define N_START_TRIGGER_08	15          // 1-15
		#define N_START_TRIGGER_09	16          // 1-16
		#define N_START_TRIGGER_10	17          // 1-17
		#define N_START_TRIGGER_11	18          // 1-18
		#define N_START_TRIGGER_12	19          // 1-19
		#define N_START_TRIGGER_13	20          // 1-20
		#define N_START_TRIGGER_14	21          // 1-21

		#define IONIZER_BLOW_SOL	28  		// 1-28


// Board #1 Outputs.......High Bits
		#define N_STOP_TRIGGER_01	0           // 1-32
		#define N_STOP_TRIGGER_02	1           // 1-33
		#define N_STOP_TRIGGER_03	2           // 1-34
		#define N_STOP_TRIGGER_04	3           // 1-35
		#define N_STOP_TRIGGER_05	4           // 1-36
		#define N_STOP_TRIGGER_06	5           // 1-37
		#define N_STOP_TRIGGER_07	6           // 1-38
		#define N_STOP_TRIGGER_08	7           // 1-39
		#define N_STOP_TRIGGER_09	8           // 1-40
		#define N_STOP_TRIGGER_10	9           // 1-41
		#define N_STOP_TRIGGER_11	10          // 1-42
		#define N_STOP_TRIGGER_12	11          // 1-43
		#define N_STOP_TRIGGER_13	12          // 1-44
		#define N_STOP_TRIGGER_14	13          // 1-45


// Board #2 Outputs.......Low Bits
		#define SYNC_K0				0			// 2-0
		#define SYNC_K1				2			// 2-2
		#define SYNC_K2				3			// 2-3
		#define N_START_TRIGGER_15	4           // 2-4
		#define N_START_TRIGGER_16	5           // 2-5
		#define N_STOP_TRIGGER_15	6           // 2-6
		#define N_STOP_TRIGGER_16	7           // 2-7

		#define STAGE_VACUUM_SOL	10			// 2-10
		#define STAGE_PURGE_SOL1	11			// 2-11
		#define STAGE_PURGE_SOL2	12			// 2-12
		#define STAGE_PURGE_SOL_SP	13			// 2-13
		#define SYNC_S0				16			// 2-16
		#define SYNC_S1				18			// 2-18
		#define SYNC_S2				19			// 2-19
		#define SYNC_S3				20			// 2-20
		#define SYNC_S4				21			// 2-21
		#define SYNC_S5				22			// 2-22
		#define SYNC_S6				23			// 2-23
		#define SYNC_S7				24			// 2-24
		#define SYNC_S8				25			// 2-25
		#define SYNC_S9				26			// 2-26
		#define SYNC_S10			27			// 2-27
		#define SYNC_S11			28			// 2-28
		#define SYNC_S12			29			// 2-29
		#define SYNC_S13			30			// 2-30
		#define SYNC_S14			31			// 2-31

// Board #2 Outputs.......Height Bits
		#define SYNC_S15			0			// 2-32
		#define SYNC_S16			1			// 2-33

		#define HEAD1_UP_SOL		16			// 2-48
		#define HEAD2_UP_SOL		17			// 2-49
		#define HEAD3_UP_SOL		18			// 2-50
		#define HEAD4_UP_SOL		19			// 2-51
		#define HEAD5_UP_SOL		20			// 2-52
		#define HEAD6_UP_SOL		21			// 2-53
		#define HEAD7_UP_SOL		22			// 2-54
		#define HEAD8_UP_SOL		23			// 2-55
		#define HEAD9_UP_SOL		24			// 2-56
		#define HEAD10_UP_SOL		25			// 2-57
		#define HEAD11_UP_SOL		26			// 2-58
		#define HEAD12_UP_SOL		27			// 2-59
		#define HEAD13_UP_SOL		28			// 2-60
		#define HEAD14_UP_SOL		29			// 2-61
		#define HEAD15_UP_SOL		30			// 2-62
		#define HEAD16_UP_SOL		31			// 2-63


// Board #3 Outputs.......Low Bits
		#define HEAD1_DW_SOL		16			// 3-16
		#define HEAD2_DW_SOL		17			// 3-17
		#define HEAD3_DW_SOL		18			// 3-18
		#define HEAD4_DW_SOL		19			// 3-19
		#define HEAD5_DW_SOL		20			// 3-20
		#define HEAD6_DW_SOL		21			// 3-21
		#define HEAD7_DW_SOL		22			// 3-22
		#define HEAD8_DW_SOL		23			// 3-23
		#define HEAD9_DW_SOL		24			// 3-24
		#define HEAD10_DW_SOL		25			// 3-25
		#define HEAD11_DW_SOL		26			// 3-26
		#define HEAD12_DW_SOL		27			// 3-27
		#define HEAD13_DW_SOL		28			// 3-28
		#define HEAD14_DW_SOL		29			// 3-29
		#define HEAD15_DW_SOL		30			// 3-30
		#define HEAD16_DW_SOL		31			// 3-31

		// Board #3 Outputs.......Height Bits
		#define NOZZLE_MOVE_SOL1	24			// 3-56
		#define NOZZLE_MOVE_SOL2	25			// 3-57
		#define PINCH_VALVE_SOL1	26			// 3-58
		#define PINCH_VALVE_SOL2	27			// 3-59
		#define SUPPLY_CDA_SOL1		28			// 3-60
		#define SUPPLY_CDA_SOL2		29			// 3-61
		#define SUPPLY_CHANGE_SOL	30			// 3-62


// Board #4 Outputs.......Low Bits
		#define READY_SIGNAL		2				// 4-2
		#define START_LAMP		  	3				// 4-3
		#define	STOP_LAMP			4				// 4-4
		#define UP_LAMP				5				// 4-5
		#define DOWN_LAMP			6				// 4-6
		#define LEFT_LAMP			7				// 4-7
		#define RIGHT_LAMP			8				// 4-8
		#define OK_LAMP				9				// 4-9
		#define VISION_LAMP1		10				// 4-10
		#define VISION_LAMP2		11				// 4-11
		#define BARCODE_TRIGGER1	12				// 4-12
		#define BARCODE_TRIGGER2	13				// 4-13
		#define SWITCHING_OUT		14				// 4-14
		#define SYLINDER_UPDW_SOL	17				// 4-17
		#define TRG_OPER_SIG		21				// 4-21
		#define TRG_ENC_RST			22				// 4-22			
		#define TRG_TRG_RST			23				// 4-23

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////// End of Genenal In/Out Contacts //////////////////////
//////////////////////////// Motion Status Define/////////////////////////////////
#define LIMITDETECT		0x00000018
#define LM_PLUS_DETECT	0x00000008
#define LM_MINUS_DETECT	0x00000010
#define HOME_DETECT		0x00800000
#define MOTIONING		0x08000000
#define SERVOALARM		0x00000002
#define SERVO_ON		0x00100000
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//				    Motion Board Axis Assignment								//
//////////////////////////////////////////////////////////////////////////////////
// board#1
#define AXIS_S_MASTER	0 		// 1-0
#define AXIS_K_MASTER	1 		// 1-1
#define AXIS_P1			2		// 1-2
#define AXIS_P2			3		// 1-3
#define AXIS_P3			4		// 1-4
#define AXIS_P4			5		// 1-5
#define AXIS_P5			6		// 1-6
#define AXIS_P6			7		// 1-7
#define AXIS_P7			8		// 1-8
#define AXIS_P8			9		// 1-9
#define AXIS_P9			10		// 1-10
#define AXIS_P10		11		// 1-11
#define AXIS_P11		12		// 1-12
#define AXIS_P12		13		// 1-13
#define AXIS_P13		14		// 1-14
#define AXIS_P14		15		// 1-15


// board#2
#define AXIS_K1			16 		// 2-0
#define AXIS_K2			17 		// 2-1
#define AXIS_P15		18		// 2-2	
#define AXIS_P16		19		// 2-3
#define AXIS_S1			20		// 2-4
#define AXIS_S2			21		// 2-5
#define AXIS_S3			22		// 2-6
#define AXIS_S4			23		// 2-7
#define AXIS_S5			24		// 2-8
#define AXIS_S6			25		// 2-9
#define AXIS_S7			26		// 2-10
#define AXIS_S8			27		// 2-11
#define AXIS_S9			28		// 2-12
#define AXIS_S10		29		// 2-13
#define AXIS_S11		30		// 2-14
#define AXIS_S12		31		// 2-15


// board#3
#define AXIS_S13		32 		// 3-0
#define AXIS_S14		33 		// 3-1
#define AXIS_S15		34		// 3-2	
#define AXIS_S16		35		// 3-3
#define AXIS_G1			36		// 3-4
#define AXIS_G2			37		// 3-5
#define AXIS_G3			38		// 3-6
#define AXIS_G4			39		// 3-7
#define AXIS_G5			40		// 3-8
#define AXIS_G6			41		// 3-9
#define AXIS_G7			42		// 3-10
#define AXIS_G8			43		// 3-11
#define AXIS_G9			44		// 3-12
#define AXIS_G10		45		// 3-13
#define AXIS_G11		46		// 3-14
#define AXIS_G12		47		// 3-15


// board#4
#define AXIS_G13		48 		// 4-0
#define AXIS_G14		49 		// 4-1
#define AXIS_G15		50		// 4-2	
#define AXIS_G16		51		// 4-3


//CAMERA
#define HEAD1_CAM      1
#define HEAD2_CAM      2
#define HEAD3_CAM      3
#define HEAD4_CAM      4
#define HEAD5_CAM      5
#define HEAD6_CAM      6
#define HEAD7_CAM      7
#define HEAD8_CAM      8
#define HEAD9_CAM      9
#define HEAD10_CAM     10
#define HEAD11_CAM     11
#define HEAD12_CAM     12
#define HEAD13_CAM     13
#define HEAD14_CAM     14
#define HEAD15_CAM     15
#define HEAD16_CAM     16

/////////////////////////////////////////////////////////////////////////////////////////
////////////	SYNC  I/O 정보 (동기함수 Param.)
/////////////////////////////////////////////////////////////////////////////////////////

#define SYNC_AXIS_S			1
#define SYNC_AXIS_K			2

/////////////////////////////////////////////////////////////////////////////////////////
////////////	MOTOR 기어비 및 펄스수에 관한 정보
/////////////////////////////////////////////////////////////////////////////////////////
// 1 mm 직선 이동하는데 필요한 pulse 수 계산.
#define XY_PULSE	1000	// 1㎛ / pulse
#define Y_PULSE		1000	// 1㎛ / pulse
#define K_PULSE		1000	// 1㎛ / pulse
#define Z_PULSE		2000	// screw pitch  6mm, 12000 pulse/rev // 0.5㎛ / pulse
#define S_PULSE		1000	// 1㎛ / pulse
#define D_PULSE		6400	// drop motor pulse, 6400 pulse/rev

#define G_PULSE		2000	// drop motor pulse, 2000 pulse/rev
#define VOLUME_RESOLUTION		0.0000625// 2.0mm / 32000 pulse ==> 0.0000625 mm/pulse

//#define A_PULSE		62500	// 구형 Head adjust motor pulse :  2 mm pitch, 125000 pulse/rev  
//#define A_PULSE		250000	 // adjust motor pulse : 0.5mm pitch, 125000 pulse/rev 
//#define A_PULSE		125000	// adjust motor pulse, 125000 pulse/rev
//#define	A_RESOLUTION	0.000016

// FASTECH
#define BOARD_AXES		16
#define BOARD_IO		64
#define BOARD_IO2		32



///////////////////////////// Calibration Range 설정 관련 /////////////////////////////////////////////////////
// adjust motor pulse, 125000 pulse/rev ==> 0.000016mm/pulse (설정치를 기본값 0으로 할 경우)
// Case 1st : 전체 Calibration 범위를 약 -0.36mg ~ +0.36mg 정도로 맞추고자 하는 경우 선택, 식 : Y=18000*X - 45000
//#define A_CAL_PULSE	18000					// Calibration시 한 Step 당 이동하여야 하는 Pulse 정의
//#define A_CAL_SCALE_Y_INTERCEPT	(-45000)	// Calibration 시 Step 별로 이동하는 그래프의 Y 절편

// Case 2nd : 전체 Calibration 범위를 약 -0.50mg ~ +0.50mg 정도로 맞추고자 하는 경우 선택, 식 
//#define A_CAL_PULSE	15625						// Calibration시 한 Step 당 이동하여야 하는 Pulse 정의
//#define A_CAL_SCALE_Y_INTERCEPT	(-62500)		// Calibration 시 Step 별로 이동하는 그래프의 Y 절편

// Case 3rd : 전체 Calibration 범위를 약 -0.35mg ~ +0.35mg 정도로 맞추고자 하는 경우 선택, 식 
//#define A_CAL_PULSE		6250						// Calibration시 한 Step 당 이동하여야 하는 Pulse 정의
#define A_CAL_SCALE_Y_INTERCEPT	(-43750)			// Calibration 시 Step 별로 이동하는 그래프의 Y 절편
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define PLUS	1
#define MINUS	0

#define OFF	0
#define ON	1
#define NORMAL	2
//
#define TFT_GLASS	0
#define CF_GLASS	1

//
#define FILE_SAVE_COUNT	50
//
#define PASS	0

#define OFFLINE_ERRORCODE	6	// by ckh 2009.03.20 오프라인시 알람 코드임.( )
/////////////////////////////////////////////////////////////////////////////////////////
////////////	PLC ADDRESS
/////////////////////////////////////////////////////////////////////////////////////////
#define DISP_BIT			0x2800
#define DISP_SEND			DISP_BIT+0xB0
#define DISP_RECEIVE		DISP_BIT+0xC0
#define DISP_OP_MODE		DISP_BIT+0x10
#define DISP_REPORT			DISP_BIT+0x20
#define DISP_UNIT_SPC       DISP_BIT+0x40
#define DISP_GLS_POS1		DISP_BIT+0x80	

#define DISP_GLS_EXIST		DISP_BIT+0xF0

#define DISP_LOAD_POS		DISP_BIT+0xF2
	
//#define DISP_SEND			DISP_BIT+0xB0
//#define DISP_RECEIVE		DISP_BIT+0xC0
#define OUT_OF_AREA			0x00		  // Un/Load Enable
#define GLASS_DATA_REQ		0x01		  // GLS Data Request
#define GLASS_DATA_OUT		0x01		  // GLS Data Report
#define ROBOT_SENDING		0x02	      // TRF BUSY
#define ROBOT_RECEIVEING	0x02		  // TRF BUSY
#define SEND_COMPLETE		0x03		  // TRF Unload Comp
#define RECEIVE_COMPLETE	0x03	      // TRF load Comp
#define SEND_ENABLE			0x04		  // Normal Status
#define RECEIVE_ENABLE		0x04		  // Normal Status
#define SEND_REQUEST		0x05		  // UnLoad Req
#define RECEIVE_REQUEST		0x05		  // Load Req
#define SEND_CONFIRM		0x06	      // Unload Comp Confirm
#define RECEIVE_CONFIRM		0x06		  // Load Comp Confirm

#define STATUS_CHANGE_MODEL		0x13   	  // 모델 변경 
#define STATUS_CLEARING			0X14	  // 크리링
#define STATUS_CHANGE_MATERIAL	0X15	  // 재료 교체

#define STATUS_PD_DOWN			0x19      //141026 PD DOWN : 상태 변경 BIT DEFINE 추가 	

#define STATUS_MEASURE		DISP_BIT + 0X02	// 현재 측정 유무 


/////////////////////////////////////////////////////////////////////////////
//TAS DATA 관련//
#define BIT_OFF				0
#define BIT_ON				1
#define ALL_OFF				2

#define TAS_EXCHANGE		0
#define TAS_LOADING			1
#define TAS_LC_SUPPLY		2
#define TAS_POS_ACCURACY	3
#define TAS_DOTTING			4
#define TAS_BALANCE_MEAS	5
#define TAS_INSPECT_DOT		6
/////////////////////////////////////////////////////////////////////////////

//#define BaseAddr_TFBK		0x2400
/////////////////////////////////////////////////////////////////////////////
//WORD
#define DISP_WORD			0x2000
#define PAIR_INFO			DISP_WORD+0x10				//0x7010
#define SELF_INFO			DISP_WORD+0x20				//0x7020
#define GLASS_ADD_DATA		DISP_WORD+0X30				//0X7030	// by ckh 2009.02.23
#define APD_REPORT1			DISP_WORD+0x40				//0x7040
#define GLS_SPC_DATA		DISP_WORD+0x70				//0x7070
#define DISP_UNIT_SPC_WORD	DISP_WORD+0xF0				//0x70F0	// 141026 PD DOWN : EQP UNIT SPC DATA WORD 영역 
#define POS_GLS_CODE		DISP_WORD+0x80				//0x7080
#define RECIPE_LIST			DISP_WORD+0xB0				//0x70B0
#define UNLOAD_PAIR_INFO	PAIR_INFO+0x100				//0x7110  
#define UNLOAD_SELF_INFO	SELF_INFO+0x100				//0x7120  

//2014.04.21 by tskim TPD 보고 관련 GlassData Back Up Address 이동 기존 0x21c0->0x2ac0
#define PAIR_INFO_BK		PAIR_INFO+0xAC0//UNLOAD_PAIR_INFO+0xC0		//0x71D0  -># 각호기에서 읽어올때는 어디에서 읽어야 하는지 알아보자. 내부적으로 사용할수 있는 영역을 만들자.
#define SELF_INFO_BK		SELF_INFO+0xAC0//UNLOAD_SELF_INFO+0xC0		//0x71E0  -># 각호기에서 읽어올때는 어디에서 읽어야 하는지 알아보자. 내부적으로 사용할수 있는 영역을 만들자.

#define TAS_DATA1			DISP_WORD+0x200
#define TAS_DATA2			DISP_WORD+0x210
#define APD_REPORT2			DISP_WORD+0x220
#define TAS_MC_DATA			TAS_DATA2+0x07               //by shin//2009.08.25//Minloss팀 요청으로 M/C해당 BIT ON/OFF
//2014.04.21 by tskim TPD
#define TPD_DATA			DISP_WORD+0x1C0
//2014.04.21 by tskim Recipe body
#define RCP_BODY			DISP_WORD+0x220

//CIM AREA
#define CIM_BASE		  0x0000			  
#define CIM_TIME_BIT	  CIM_BASE+0x10
#define CIM_ACK_BIT		  CIM_BASE+0x20
#define CIM_BODY		  CIM_BASE+0x70

#define CIM_PDDOWN		  CIM_BASE+0x1A0	//141026 PD DOWN : ECS PD DOWN BIT START ADDRESS 
#define CIM_PDDOWN_WORD	  CIM_BASE+0x60		//141026 PD DOWN : ECS PD DOWN WORD START ADDRESS 

#define BaseAddr_DISPCTRL	0x1000
#define BaseAddr_CIM_1		0x2600
#define BaseAddr_MACH_STATUS	0x2700
#define BitDispModeStatus	0x00
#define	BitGlassReportData	0x20
#define	BitGlassReportDataSd	0x30
//#define BitDispReceiveIF	0xA0
//#define BitDispSendIF		0xB0
#define BitDispStatus		0xF0

// TF status
#define BitTFModeStatus		0x00

// CIM Refer
//#define BitProcKeyDataSendReq	0x00
//#define BitPanelNormalInfo		0x20
//#define BitPanelSpecialInfo		0x40
//
// disp control
#define BitDispBuzzer		0xE0
#define BitDispLamp			0xF0

#define GLASS_PROCESSING	0x0b
#define GLASS_BEING			0x0c

///////////이전 PLC ADDRESS///////////////////////////////////////////
#define BaseAddr_CIM		0x2000
#define BaseAddr_TF			0x2100
#define BaseAddr_DISP		0x2300
#define BaseAddr_TFBK		0x2400
//#define BaseAddr_DISPCTRL	0x2500
#define BaseAddr_CIM_1		0x2600
//#define BaseAddr_MACH_STATUS	0x2700

//
//#define BitDispModeStatus	0x00
#define	BitGlassReportData	0x20
#define	BitGlassReportDataSd	0x30
#define BitDispReceiveIF	0xA0
#define BitDispSendIF		0xB0
#define BitDispStatus		0xF0
//
// TF status
#define BitTFModeStatus		0x00

// CIM Refer
#define BitProcKeyDataSendReq	0x00
#define BitPanelNormalInfo		0x20
#define BitPanelSpecialInfo		0x40
//
// disp control
#define BitDispBuzzer		0xE0
#define BitDispLamp			0xF0

////////////이전 PLC ADDRESS END/////////////////////////////////////////////////
//2010.05.17 by tskim 
#define CIM_BASE_RMS		0x2C00
#define CIM_DISP_RMS		0x3800	

//2014.10.23
#define READ_VALC_RUN_1		0x3200
#define READ_VALC_RUN_2		0x3210

#endif
