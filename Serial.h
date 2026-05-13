#ifndef __SERIAL_H__
#define __SERIAL_H__

// Class definition for Serial Communication using CMSComm Class.
//
#include "MSComm.h"
#include "Protocol.h"

class CSerial : public CMSComm
{
public:
	void SendStr( CString );
	void DoEvents();
	void PortInitialize(short port_num);
private:
};
#endif