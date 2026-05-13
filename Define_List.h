#ifndef __DEFINE_LIST_H__
#define	__DEFINE_LIST_H__

#define TEST_FLAG 0
/*모터 위치 확인 스텝  */
typedef enum _Step_PosChk{
	POS_CHK_SEL_AXIS = 1000,
	POS_CHK_OR_DISPENSER_CHK,
	POS_CHK_START ,
	POS_CHK_STOP,

	/* 모터 위치 확인 스텝*/
	POS_CHK_READY_MOVE_COLUMN,
	POS_CHK_READY_MOVE_END_COLUMN,

	POS_CHK_READY_MOVE_S,
	POS_CHK_READY_MOVE_END_S,
	POS_CHK_READY_POSCHK_S, // PosChk 대기 위치로 이동했는지 위치 확인 스텝

	SEL_AXIS_MOVE_START_SK,	// 정위치구동 헤더별 S축, K축 위치로 이동함.
	SEL_AXIS_MOVE_START_TWO,// 테스트 후 수정하기. 현재는 미사용
	SEL_AXIS_MOVE_END,
	
	POS_CHK_GET_MARK_START,
	POS_CHK_RESULT_SAVE,
	POS_CHK_END_POS_MOVE_START,
	POS_CHK_END_POS_MOVE_END,

	NEX_POS_CHK,
	
	POS_CHK_END,

	MOTION_WAIT,
	MOTION_WAIT_EMG,

	/* 토출 유무 측정 STEP(AUTO REPAIR)*/
	DIS_CHK_START,
	DIS_CHK_GET_FIX_POINT_DATA,// Repair 헤더와 포인트 정보, 노즐과 카메라의 상대 거리.
	DIS_CHK_CAM_MOVE_FIX_POINT_COL, // 
	DIS_CHK_CAM_MOVE_FIX_POINT_COL_END,
	DIS_CHK_CAM_MOVE_FIX_POINT_S,
	DIS_CHK_CAM_MOVE_FIX_POINT_S_END,
	DIS_CHK_GET_BLOB,
	DIS_CHK_GET_RESULT,
	
	
	/* 노즐과 카메라의 상대 거리 구하기.*/
	/*
		임의 헤더를 수동으로 티칭하여 기본 티칭값을 획득 한 후 
		전체 헤더에서 토출후 1번 ~ 16번 헤더 까지의 노즐과 카메라의 상대 위치를 구한다.
		- 더미 토출위치
	*/
	
	/* 얼라인 모드 추가하기. */
	/* 2008.12.26
		Auto Mode에서 사용시 데이터 표시 부분은 추후 테스트 후 추가할 것.
		시작 스텝을 메뉴얼 모드와 ,자동 모드에서의 스텝을 구별함.
	*/

	ALIGN_MOVE_READY,		// ALIGN 구동을 위한 동작( 동기 묶기, 조명 셋팅  등)
	ALIGN_MOVE_POSITION1,	// 1번 마크 위치 이동 
	ALIGN_MARK_GET1,
	ALIGN_MOVE_POSITION2,	// 2번 마크 위치 이동 
	ALIGN_MARK_GET2,
	ALIGN_CALIBRATE,		// 마크 위치 오차값 계산
	ALIGN_CAL_END,			// 결과 값 저장 및 표시





	/*************************************/
	/**********ERROR STEP*****************/
	ERR_MARK_NOT_SEARCH = 2000,
	ERR_AXIS_POS,
	ERR_VISION_LIGHT,
	ERR_READY_BUT,
	ERR_ROBOT_ARM,
	ERR_HOME_NOT_SEARCH,
	ERR_SYNC_AXIS_S,
	ERR_TEACHING_POSCHK,
	ERR_DOOR_LOCK,
	ERR_ALIGN,				// Glass 틀어짐 알람.
		
}STEP_POSCHK;

/* Pos_Chk 확인 용 쓰레드 스텝 데이터 */
typedef struct PosChk_ThreadData
{
	int nPreStep; // 이전 
	int nCntStep; // 현재
	int nNextStep;// 다음

	BOOL bExitFlag; // 스레드 구동 여부
	BOOL bPosChk;	// PosChk 완료  여부, 작업이 시작시 True, 미완료면 False
	BOOL bDisChk;	// 토출 여부 확인.
	BOOL bPosChkConCnt;	// 1-8,9-16축 동시에 PosChk가 가능여부
	
	int nPosChkAxis;// 현재 PosChk 축
	CString strMsg;// 현재 스레드 메세지

	BOOL bFirstChk;// 처음 마크 획득 위치를 저장 하기 위해, 기준 값이므로, 임의로 사용하지 말자. 

	int nMode;	// 현재 사용 모드 ( MODE_POSCHK : 모터 정위치 , MODE_CMATO_NZL: 카메라 - 노즐 간 거리 획득, MODE_DROP_POS: Drop 위치 획득 )

	long lSearchColor ;// 찾아야할 마크의 색깔.
}POSCHK_THREADSTEP;

// BY CKH 2008.12.26 // 코멘트, 모드 추가함. - 얼라인 마크 모드 추가.
typedef enum PosChk_Mode{
	MODE_POSCHK =0, // 정위치 
	MODE_CMA_TO_NZL, // 카메라,노즐간 위치
	MODE_DROP_POS,	// Drop 위치
	MODE_ALIGN,		// ALIGN MARK찾기

}POSCHK_MODE;

/* Pos_Chk 마크 및 토출 데이터의 위치 및 사이즈 데이터*/
typedef struct PosChk_Data{
	double dPosX;	// 마크 좌표 위치
	double dPosY;	// 마크 좌표 위치

	double dTagetPosX; // 이동해야 할 위치
	double dTagetPosY; // 이동해야 할 위치

	int nAxis ;	
	double dMarkX;		// 마크 위치, 화면상의 값
	double dMarkY;		// 마크 위치, 화면상의 값
	double dMarkAngle;	// 마크의 회전 각도 , 화면상의 값, 얼라인 마크 서치 모드에서는 두 마크에 따른 Glass 회전 각도임. 
	double dMarkSize;	// PosChk 마크 크기(티칭 마크 사이즈)	
	double dMarkSizeMin;	// 마크 최소 or 토출 최소 사이즈
	double dMarkSizeMax;	// 마크 최대 or 토출 최대 사이즈
	
	double dCamToNzlX; // 노즐에서 카메라까지 거리(노즐위치 - 카메라 위치)
	double dCamToNzlY; // 노즐에서 카메라까지 거리(노즐위치 - 카메라 위치)

	BOOL bSel;		// 선택 유무
	BOOL bPosChk;	// 구동 완료 유무( Spec과 무관함.)

	BOOL bSpecInX; // Spec내 구동 여부 (PIXEL)
	BOOL bSpecInY; // Spec내 구동 여부 (PIXEL)
	double dSpecMin_X;  // Spec 최소 (PIXEL)
	double dSpecMax_X;  // Spec 최대 (PIXEL)
	double dSpecMin_Y;  // Spec 최소 (PIXEL)
	double dSpecMax_Y;  // Spec 최대 (PIXEL)

	double dErrorVal_X; // X 방향 오차값 (PIXEL)(티칭 픽셀- 측정픽셀)
	double dErrorVal_Y; // X 방향 오차값 (PIXEL)

	double dSpeed_K;	// 컬럼 속도
	double dSpeed_S;	// S축 속도
	long lSearchColor;// 찾아야 하는 마크의 색깔.

	// 20008.12.20 by ckh 
	BOOL bLightUse;
	int nLightVal;

	// 없어도 될듯함.
	int nMux; // Mux 번호
	int nCam; // 카메라 번호

	double dTagetPosX1; // 이동해야 할 위치
	double dTagetPosY1; // 이동해야 할 위치

	BOOL b1stMoveCheck;
	BOOL b2ndMoveCheck;

	int nSelectPos;//0: 기준위치, 1: 설정 위치 

}POSCHK_DATA;
/*  마크 및 토출에서의 비전 데이터  */// 사용 안함. 
// typedef struct Blob_Data{
// 	double dMarkX;		// PosChk 마크 위치, 화면상의 값
// 	double dMarkY;		// PosChk 마크 위치, 화면상의 값
// 
// 	long lMarkSize;	// PosChk 마크 크기(티칭 마크 사이즈)	
// 
// 	double dErrorVal_X; // X 방향 오차값 (PIXEL)(티칭 픽셀- 측정픽셀)
// 	double dErrorVal_Y; // X 방향 오차값 (PIXEL)
// 
// 	BOOL bSpecInX; // Spec내 구동 여부 (PIXEL)
// 	BOOL bSpecInY; // Spec내 구동 여부 (PIXEL)
// 	BOOL bPosChk;	// PosChk 구동 완료 유무( Spec과 무관함.)
// }BLOB_DATA;


/* Pos_Chk  관련 Flag*/
typedef struct PosChk_flag{
	BOOL bPosChk;
	BOOL bConCnt; // 2축 이상의 축이 동시에 구동 
}POSCHK_FLAG;



/* Dis_Chk 관련 데이터*/
typedef struct DisChk_Data{

	BOOL bDisChkRepair;// 추가 수정 토출 했는지 유무 
	BOOL bDisChkEnd ; // 검사 완료

	int nBlobSize ; // 카메라로 찾은 토출의 크기
	double dSpecMin; // 스팩 최소
	double dSpecMax; // 스펙 최대
	BOOL bSpedIn; // 스펙 만족 여부 

	int nHeadNum; // 확인할 헤더 번호
	int nPoint; // 확인 할 포인트 번호
	double dPointX; // 수정할 위치
	double dPointY; // 수정할 위치
	

}DISCHK_DATA;

// 모터 정위치 확인 클래스 타이머 번호
typedef enum _Timer_PosChk{
	START_POSCHK=1,
	BUT_TIMER ,

}TIMER_POSCHK;

// 각 S축 및 보드 정보
typedef struct S_Axis_BD{
	int nBd;
	int nAxis;
}S_AXIS_BD;

// 모드 구분 
typedef enum _Motion_Mode{
	// 0: Auto Mode, 1: 반송 모드, 2: Manual Pattern Drop 한 Pattern만, 3: Manual Pattern Drop 모든 Pattern Drop
	MODE_AUTO=0,
	MODE_PASS,
	MODE_DROP_ONE,
	MODE_DROP_ALL
}MOTION_MODE;

// by ckh 2009.01.07
typedef enum _Data_Mode{
	DATA_ADDDEL=0,
	DATA_TFT,
	DATA_CF,
	
}DATA_MODE;

//2010.05.17 by tskim ECS Data
typedef struct RMS_Data
{
	CString CEID;
	CString RecipeCmd;
	CString RecipeNum;
	CString RecipeLevel;
	CString RecipeType;
	CString RecipeVer;
	CString UnitNumber;
	CString Reserve;
	CString CEIDOrder;
	CString TotalCnt;
	CString ReportCnt;
	CString status;
	CString FromRecipe;
	CString ToRecipe;
} RMS_DATA;

#endif
