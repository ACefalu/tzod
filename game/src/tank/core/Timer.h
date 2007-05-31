// Timer.h: interface for the Timer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

/*---------------------------------------------------------------
 *  ������ �������� ����������. �������� � �������������� QPF
 *  ���� QPF ����������, ����� �������������� timeGetTime()
 *--------------------------------------------------------------*/

class Timer
{
protected:
	BOOL	_useQPF;
	LONG	_stopCount;

	// qpf
	double        _qpf_frequency;
	LARGE_INTEGER _qpf_time_pause;
	LARGE_INTEGER _qpf_time_last_dt;
	LARGE_INTEGER _qpf_time_max_dt;  // ������������ �������� dt.

	// timeGetTime
	DWORD _time_pause;
	DWORD _time_last_dt;
	DWORD _time_max_dt;

public:
	Timer();  // ��� �������� ������ ����������
	virtual ~Timer();

public:
	float GetDt(); // ���������� ����� ����� �������� GetDt()
	void  SetMaxDt(float dt); // ��������� ������������� �������� dt

	void Start();
	void Stop();
	BOOL IsRuning() {return !_stopCount;};
};

///////////////////////////////////////////////////////////////////////////////
// end of file
