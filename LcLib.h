#ifndef __LCLIB_H__
#define	__LCLIB_H__

#include "labelcontrol.h"
#include "msflexgrid.h"

extern "C" __declspec(dllimport) void Use_TKg(CMSFlexGrid &ctlGrid, double dMaxValue, double dMinValue , 
											  int nLeftPos,int nTopPos,BOOL bSingleCol,BOOL bSingleRow) ;
extern "C" __declspec(dllimport) void Use_TK(CLabelControl &ctl, double dMaxValue, double dMinValue,
											 int nLeftPos,int nTopPos);
extern "C" __declspec(dllimport) void Use_STR(CLabelControl &ctl, CPoint pos);

#endif
