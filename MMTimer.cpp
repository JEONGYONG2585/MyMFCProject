/*

  MMTimer.cpp - Implementation for the CMMTimer class. This class is a light 
                wrapper around the Windows multimedia timer. 

  NOTE: Be sure to link to the winmm library.

  Copyright (C) 2002  Leslie Sanford

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
  USA

  Contact: jabberdabber@hotmail.com

  Last modified: 10/24/2002

*/


//-----------------------------------------------------------------------------
// Dependencies
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "MMTimer.h"
#pragma comment(lib, "winmm.lib")

//-----------------------------------------------------------------------------
// CMMTimer class implementation
//-----------------------------------------------------------------------------


// Default constructor
CMMTimer::CMMTimer() :
	m_Milliseconds(0),
	m_Delay(0),
	m_Count(0),
	m_RunningFlag(false)
{

}


// Destructor
CMMTimer::~CMMTimer()
{
    if(m_RunningFlag)
    {
        Stop();
    }
}


// Starts timer
bool CMMTimer::Start(UINT Delay, UINT Resolution, HANDLE EventHandle)
{
    bool Result = true;

    // Only begin timer if it is not already running
    if(!m_RunningFlag)
    {
        // If this timer started without error
        if((m_Id = ::timeSetEvent(Delay, Resolution, TimeProc,
            reinterpret_cast<DWORD>(this), TIME_PERIODIC)) != NULL)
        {            
            // Keep track of delay interval and note that this timer is 
            // running
            m_Delay       = Delay;
            m_RunningFlag = true;
			m_hEvent	  = EventHandle;
        }
        // Else an error occurred
        else
        {
            // Note error
            Result = false;
        }
    }

    return Result;
}


// Stop timer
void CMMTimer::Stop()
{
    if(m_RunningFlag)
    {
        ::timeKillEvent(m_Id);
        m_RunningFlag = false;
    }
}


// Reset timer values
void CMMTimer::Reset()
{
    m_Milliseconds = m_Count = 0;
}


// Gets total number of seconds that have passed since this timer began.
DWORD CMMTimer::GetTotalSeconds() const
{
    return m_Milliseconds / MILLISECONDS_PER_SECOND;
}


// Gets total number of minutes that have passed since this timer began.
DWORD CMMTimer::GetTotalMinutes() const
{
    return GetTotalSeconds() / SECONDS_PER_MINUTE;
}


// Gets total number of hours that have passed since this timer began.
DWORD CMMTimer::GetTotalHours() const
{
    return GetTotalMinutes() / MINUTES_PER_HOUR;
}


// Gets number of milliseconds that have passed since the last second.
DWORD CMMTimer::GetCurrentMilliseconds() const
{ 
    return m_Milliseconds - GetTotalSeconds() * MILLISECONDS_PER_SECOND;
}


// Gets number of seconds that have passed since last minute.
DWORD CMMTimer::GetCurrentSeconds() const
{
    return GetTotalSeconds() - GetTotalMinutes() * SECONDS_PER_MINUTE;
}


// Gets number of minutes that have passed since last hour.
DWORD CMMTimer::GetCurrentMinutes() const
{
    return GetTotalMinutes() - GetTotalHours() * MINUTES_PER_HOUR;
}


// Gets number of hours that have passed since last day (starts from zero 
// every 24 hours).
DWORD CMMTimer::GetCurrentHours() const
{
    DWORD CurrentHours = GetTotalHours();

    if(CurrentHours >= HOURS_PER_DAY)
    {
        CurrentHours %= HOURS_PER_DAY;
    }

    return CurrentHours;
}

    
// Gets multimedia device capabilities.
void CMMTimer::GetDevCaps(LPTIMECAPS TimeCap)
{
    ::timeGetDevCaps(TimeCap, sizeof(TIMECAPS));
}


// Function called by Windows when a timing event associated with this timer
// occurs.
void CALLBACK CMMTimer::TimeProc(UINT uID, UINT UMsg, DWORD dwUser, DWORD dw1,
                                 DWORD dw2)
{
    CMMTimer *Timer = reinterpret_cast<CMMTimer *>(dwUser);

    Timer->m_Milliseconds += Timer->m_Delay;
    Timer->m_Count++;
	SetEvent(Timer->m_hEvent);
}


















