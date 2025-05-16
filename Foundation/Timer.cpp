//
// Realizations of basic timer classes for PC
//
// Author: Alex V. Boreskoff
// Last modified: 08/07/2002
//

#include	<windows.h>
#include	<winbase.h>
#include	<mmsystem.h>
#include	<math.h>
#include	"Timer.h"

MetaClass	Timer :: classInstance ( "Timer", &Object :: classInstance );

////////////////////// Timer using timeGetTime method ///////////////////////////

class	WindowsTimer : public Timer
{
private:
	DWORD	startTime;							// time in milliseconds since creation of time
	DWORD	startOfPause;
	float	pausedTime;							// time in milliseconds elapsed in paused state
	int		paused;
public:
	WindowsTimer ( const char * theName ) : Timer ( theName )
	{
		startTime  = timeGetTime ();
		pausedTime = 0.0f;
		paused     = 0;
	}

	virtual float	getTime () const			// get elapsed time since timer's creation
	{
        return abs(static_cast<long>(timeGetTime() - startTime)) * 0.001f - pausedTime;
	}

	virtual	void	pause ()
	{
		if ( ++paused == 1 )
			startOfPause = timeGetTime ();
	}

	virtual	void	resume ()
	{
		if ( paused > 0 )
			if ( --paused == 0 )
				pausedTime = abs (static_cast<long>(timeGetTime() - startOfPause)) * 0.001f;
	}

	static	MetaClass	classInstance;
};

//////////////////////////// High-precision Timer class //////////////////////////

class	HiPrecTimer : public Timer
{
private:
	__int64	startTime;
	__int64	startOfPause;
	__int64	frequency;
	float	pausedTime;						// time in milliseconds elapsed in paused state
	int		paused;
public:
	HiPrecTimer ( const char * theName ) : Timer ( theName )
	{
		LARGE_INTEGER	start;
		LARGE_INTEGER	freq;

		QueryPerformanceCounter   ( &start );
		QueryPerformanceFrequency ( &freq );

		startTime  = start.QuadPart;
		frequency  = freq.QuadPart;
		pausedTime = 0.0f;
		paused     = 0;
	}

	__int64	getCounter () const
	{
		LARGE_INTEGER	c;

		QueryPerformanceCounter ( &c );

		return c.QuadPart;
	}
		
	virtual float	getTime () const			// get elapsed time since timer's creation
	{
		__int64	curTime = getCounter ();
		__int64	delta   = curTime - startTime;;

		return (float)delta / (float) frequency - pausedTime;
	}

	virtual	void	pause ()
	{
		if ( ++paused == 1 )
			startOfPause = getCounter ();
	}

	virtual	void	resume ()
	{
		if ( paused > 0 )
			if ( --paused == 0 )
			{
				__int64 curTime = getCounter ();

				pausedTime = (float)(curTime - startOfPause) / (float) frequency;
			}
	}

	static	MetaClass	classInstance;
};

MetaClass	WindowsTimer :: classInstance ( "WindowsTimer", &Timer :: classInstance );
MetaClass	HiPrecTimer  :: classInstance ( "HiPrecTimer" , &Timer :: classInstance );

///////////////////////////// Timer factory method ///////////////////////////////

Timer * getTimer ( const char * theName )
{
//	return new WindowsTimer ( theName );
	return new HiPrecTimer ( theName );
}

