#if !defined(MMTIMER_H_LESLIE_SANFORD)
#define MMTIMER_H_LESLIE_SANFORD


/*

  MMTimer.h - Interface for the CMMTimer class. This class is a light 
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

#include <mmsystem.h>


//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------


const DWORD MILLISECONDS_PER_SECOND = 1000;
const DWORD SECONDS_PER_MINUTE      = 60;
const DWORD MINUTES_PER_HOUR        = 60;
const DWORD HOURS_PER_DAY           = 24;


//-----------------------------------------------------------------------------
// CMMTimer class
//
//  A light class wrapper for the Windows multimedia timer.
//-----------------------------------------------------------------------------


class CMMTimer
{
public:
    // Construction/Destruction
    CMMTimer();
    virtual ~CMMTimer();

    // Starts the timer. Delay is the interval in milliseconds between each 
    // timing event. Resolution is the timing resolution of this timer. A value
    // of zero gives the greatest timing resolution. Returns true if the 
    // operation was successful.
    bool Start(UINT Delay, UINT Resolution, HANDLE EventHandle);

    // Stops this timer. Timer can be restarted by calling Start.
    void Stop();

    // Resets this timer back to zero.
    void Reset();

    // Returns true if this timer is running.
    bool IsRunning() const { return m_RunningFlag; }

    // Gets the total timing values since this timer was started.
    DWORD GetTotalMilliseconds() const { return m_Milliseconds; }
    DWORD GetTotalSeconds() const;
    DWORD GetTotalMinutes() const;
    DWORD GetTotalHours() const;

    //
    // The following accessor functions return relative timing values. For 
    // example, the GetCurrentMilliseconds function returns how many 
    // milliseconds have passed since the last second has occurred. Let's say  
    // that 3.5 seconds have passed since the timer was started. 
    // GetCurrentMilliseconds would return 500 because that is how many 
    // milliseconds have passed since the last second began. 
    //

    DWORD GetCurrentMilliseconds() const;
    DWORD GetCurrentSeconds() const;
    DWORD GetCurrentMinutes() const;
    DWORD GetCurrentHours() const;

    // Gets the number of times a timing event has occurred.
    DWORD GetCount() const { return m_Count; }

    // Gets the device capabilities of this timer. Results are stored in the
    // TIMECAPS structure.
    static void GetDevCaps(LPTIMECAPS TimeCap);

private:
    // Function called by Windows when a timing event associated with this 
    // timer occurs.
    static void CALLBACK TimeProc(UINT uID, UINT uMsg, DWORD dwUser, 
                                  DWORD dw1, DWORD dw2);

    // Copying not allowed
//	CMMTimer(const CMMTimer &);
//	CMMTimer &operator = (const CMMTimer &);

private:
    UINT	m_Id;
    UINT	m_Delay;
    DWORD	m_Milliseconds;
    DWORD	m_Count;
    bool	m_RunningFlag;
	HANDLE	m_hEvent;
};


#endif