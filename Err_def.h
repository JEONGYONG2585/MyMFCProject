////////////////////////////////////////////////////////////////////////////
//
//      (C) 2002  Fas Technology Co., Ltd. 
//		FILENAME:		Err_def.h
//		DESCRIPTION:	FM_lib 
//
/////////////////////////////////////////////////////////////////////////////

#ifndef		__ERR_DEF_H__
#define		__ERR_DEF_H__

#pragma pack(1)

//Error List
typedef enum _ERROR {
	IDE_ERR_NONE = 0,
	//Open Error
	IDE_ERR_NONE_CARD,			//보드가 없음.
	IDE_ERR_EVENT_LINK,
	IDE_ERR_EVENT_UNLINK,
	IDE_ERR_EVENT_WAIT,
	IDE_ERR_THREAD_CREATE,
	IDE_ERR_THREAD_EXIT,		//종료시에 사용??
	IDE_ERR_GET_CARD_INFO,		//보드 정보를 얻는 중에 에러발생...
	
	//Interface Error
	IDE_ERR_OVER_CARD_NUM,		//보드 번호 오버?
	IDE_ERR_NONE_CARD_NUM,		//보드 번호 없음?

	IDE_ERR_GET_CARD_ID,		//보드 정보를 얻는 중에 에러발생...

	IDE_ERR_OVER_CARD_ID,		//BdID가 최대치를 초과했음.
	IDE_ERR_NONE_CARD_ID,		//해당 BdID가 존재하지 않음.

	IDE_ERR_OVER_OFFSET,		//Offset 값이 0x100 이상이어서 에러 발생.
	
	//Etc Error
	IDE_ERR_SYSTEM,
	IDE_ERR_UNKNOWN,
    IDE_ERR_LAST
}IDE_ERROR ;


//Error List
typedef enum _CARD_NO {
	IDC_CARD_0 = 0,
	IDC_CARD_1,
	IDC_CARD_2,
	IDC_CARD_3,
	IDC_CARD_4,
	IDC_CARD_5,
	IDC_CARD_6,
	IDC_CARD_7,
	IDC_CARD_LAST,
	IDC_CARD_ALL=100
} IDC_CARD_NO ;

#pragma pack()

#endif		//__ERR_DEF_H__