//#define TOPENG_COMM_CLIENT_EXPORT

#define _MSG_STRUCT_TYPE_STRING			0

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef TOPENG_COMM_CLIENT_EXPORT
#define TOPENG_COMM_CLIENT __declspec(dllexport)
#else
#define TOPENG_COMM_CLIENT __declspec(dllimport)
#endif

	// Engine Start 의 3,4번째 인수에 입력할 수신함수 형태 정의
	// Server 로 부터의 메세지를 수신한다.
	typedef void  (__stdcall *TopEngCommClient_CBType_RcvMsg)(char*, int);
	// Server 와 Disconnect 됐다는 메세지를 수신한다.
	typedef void  (__stdcall *TopEngCommClient_CBType_DisconnectMsg)(void); 

	// 인수 char* chKey 에는 고유 키값 입력.
	// 인수 bool bStart 에는, Engine Start 시 true 를, Stop 시 flase 를 입력.
	// 성공시 0 반환. 실패시 0 미만값 반환됨.
	TOPENG_COMM_CLIENT int __stdcall TopEngCommClient_EngineStart(char* chKey, bool bStart, TopEngCommClient_CBType_RcvMsg pRcvMsgFunc, TopEngCommClient_CBType_DisconnectMsg pDisconnectMsgFunc);

	// 성공시 0 반환. 실패시 0 미만값 반환됨.
	TOPENG_COMM_CLIENT int __stdcall TopEngCommClient_OpenPort(char *chIP, int iPort);

	// 성공시 0 반환. 실패시 0 미만값 반환됨.
	TOPENG_COMM_CLIENT int __stdcall TopEngCommClient_ClosePort();

	// 성공시 1 이상값 반환됨. 실패시 0 이하값 반환됨.
	TOPENG_COMM_CLIENT int __stdcall TopEngCommClient_SendMsg_String(char *chMsg, int iMsgArrCnt, int iTimeOut);

	// Network Drive 를 연결함.
	// 인수 char* chIP 는 Server 측 IP 입력.
	// 인수 char* chFolderName 는 접근하고자 하는 Server 측의 공유폴더명 입력.
	// 인수 char* chDriveName 는 local 에서 Network Drive 로 지정할 Drive명 입력.
	// 인수 char* chUserID 는 Server 측 접속 ID. Server 측 Windows 에 비밀번호가 걸려있지 않을 경우 입력할 필요 없음.
	// 인수 char* chPW 는 Server 측 접속 Password.
	// 성공시 0 이상값 반환됨. 실패시 0 미만값 반환됨.
	TOPENG_COMM_CLIENT int __stdcall TopEngCommClient_ConnectNetDrive(char* chIP, char* chFolderName, char* chDriveName, char* chUserID, char* chPW);

#ifdef  __cplusplus
}
#endif
