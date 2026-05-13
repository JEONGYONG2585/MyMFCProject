#ifndef __LCDEF_ADD_H__
#define	__LCDEF_ADD_H__

// 
typedef struct TeachingConfig
{
	double dMax;
	double dMin;
	double bCellOn;
}TEACHING_CONFIG;

// Io에서 사용되는 str struct..
typedef struct IoCaptionSet
{
	CString strInput[64];  //	strInput[io_no_in_1_bd]
	CString strOutput[64]; //	strOutput[io_no_in_1_ba]
	CString strInput_Index[64];  //	strInput[io_no_in_1_bd]
	CString strOutput_Index[64]; //	strOutput[io_no_in_1_ba]
	CString strAxis[16];    //	strAxis[axis_no_in_1_bd]
}IO_CAPTION_SET;

// Auto Interface에서 사용되는 str struct..
typedef struct IFCaptionSet
{
	CString strIF[6][11]; //
}IF_CAPTION_SET;

// 전역에서 사용되는 head관련 struct..
typedef struct HeadConfig
{
	UINT nA[2]; //	nA[bd_no, axis_no]
	UINT nD[2]; //	nD[bd_no, axis_no]
	UINT nS[2]; //	nS[bd_no, axis_no]
	UINT nZ[2]; //	nZ[bd_no, axis_no]
}HEAD_CONFIG;
//
/*
// config에 있는 변수
typedef struct tagCalibrationData
{
	// calibration 관련
//	double dCalculate[MAX_CALDATA][2];
	double dMeasure[MAX_MEADATA_FORCALIB][2];
	double dMeasure1[6][2];
	double dMeasure2[6][2];
	double dMeasure3[6][2];
//	double dCoefficient[MAX_MEADATA_FORCALIB]; 
	double dCoefficient1[6];
	double dCoefficient2[6];
	double dCoefficient3[6];

} CALIBRATIONDATA;*/
//
#endif
