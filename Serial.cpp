#include "stdafx.h"
#include "serial.h"

#define MAXLIN 20

void CSerial::PortInitialize( short port_num )
{
	CString	initStr ="";
	short nNewValue = 0;
	
	nNewValue = (short)(port_num) ;
/*
	SetCommPort( nNewValue );

	if(port_num == 2) initStr.Format("%d,%c,%d,%d",19200,'N',8,1);
	else initStr.Format("%d,%c,%d,%d",9600,'N',8,1);
*/
	SetCommPort( nNewValue );

	if(port_num == 2)
		initStr = CString( "19200," );
	else
		initStr = CString( "9600," );

//	if((port_num == 11) || (port_num == 12) || (port_num == 13) || (port_num == 14)) initStr += CString( "E," ); //중앙공급
//2015.02.10 by tskim Comport shift
	if((port_num == 13) || (port_num == 14) || (port_num == 15) || (port_num == 16)) 
		//initStr += CString( "E," ); //중앙공급
		initStr += CString( "N," );
	else 
		initStr += CString( "N," ); // Parity Bit : None 
//	initStr += CString( "O," );	// Parity Bit : Odd

	// Set Data Bit: 8 or 7
//	if((port_num == 11) || (port_num == 12) || (port_num == 13) || (port_num == 14)) initStr += "7,"; //중앙공급
//2015.02.10 by tskim Comport shift
	if((port_num == 13) || (port_num == 14) || (port_num == 15) || (port_num == 16)) 
		//initStr += "7,"; //중앙공급
		initStr += "8,";
	else
		initStr += "8,";  // Parity Bit를 None으로 설정하였을 경우...

//	initStr += "7,";	// Parity Bit를 Odd나 Even으로 설정하였을 경우...
	
	// Set Stop Bit.
	initStr += "1";

	SetSettings( initStr );

	SetInputMode( 1 );	// comminputMode: Binary ; 0:TEXT 1:Binary
	SetRThreshold( 1 ); // every time event 
	SetInputLen(0);     // receive entry data
}

void CSerial::DoEvents()
{
	MSG	msg;

	while(	GetOutBufferCount() && 
			::PeekMessage( &msg,NULL,0,0,PM_NOREMOVE ) )
	{
		if( !AfxGetThread()->PumpMessage() )
			return;		
	}
}

void CSerial::SendStr(CString str)
{
	VARIANT	var;

	var.vt = VT_BSTR;
	var.bstrVal = str.AllocSysString();

	CMSComm::SetOutput( var );

	DoEvents();

	SysFreeString( var.bstrVal );
}

