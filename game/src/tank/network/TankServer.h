// TankServer.h

#pragma once

#include "Socket.h"
#include "datablock.h"
#include "TankClient.h"


class TankServer
{
	FILE *_logFile;
	void SERVER_TRACE(const char *fmt, ...);

	bool _init;
	GAMEINFO _GameInfo;

	struct ClientDesc
	{
		DWORD      id;               // ���������� ������������� �������
		Socket     s;                // ���������� ������
		WSAEVENT   stop;             // ������� �������� ������ ������� ����������
		//---------------------------//----------------------------
		BOOL       connected;        // ���� ����������, ��� ���� desc ���������
		PlayerDesc desc;             // �������� ������
		BOOL       ready;            // ����� ����� ������ ����
		//---------------------------//----------------------------
		HANDLE semaphore;            // ������� ������������ ������� ������ � �������
		CRITICAL_SECTION cs;         // ����������� ������ �������� ���� data � ctrl
		std::queue<DataBlock> data;  // ������, ������� ������ ���� ���������� �������
		std::queue<ControlPacket> ctrl;// ����������� �� ������ ������ ����������
	};


	DWORD _nextFreeId;

	std::list<ClientDesc> _clients;
	CRITICAL_SECTION    _csClients; // �������� _clients
	HANDLE     _hClientsEmptyEvent; // ������� � ���������� ���������, ����� ������ _clients ����

	struct ClientThreadData
	{
		TankServer *pServer;
		std::list<ClientDesc>::iterator it;
	};

	static DWORD WINAPI ClientProc(ClientThreadData *pData);


	Socket _socketListen;        // �������������� �����
	HANDLE _evStopListen;        // ������� ������ �������������

	HANDLE _hAcceptThread;       // �����, ����������� ����������� ��������
	static DWORD WINAPI AcceptProc(TankServer *pServer);


	HANDLE _hMainThread;         // �������� ��������� �����
	HANDLE _hMainSemaphore;      // ������� ������������ ������� ������ � ������� _MainData
	HANDLE _hMainStopEvent;      // ������� ��������� �������� �����
	CRITICAL_SECTION _MainCS;    // ����������� ������ �������� ������ _MainData

	struct MainThreadData
	{
		DWORD id_from;
		DataBlock data;
	};
	std::queue<MainThreadData> _MainData;   // ������, ������� ������ ���� ���������� �������� �������

	bool TrySendFrame();         // �������� ����� ���� ��� ������ ��������
	static DWORD WINAPI MainProc(TankServer *pServer);

	void SendMainThreadData(DWORD id_from, const DataBlock &data);
	void SendClientThreadData(const std::list<ClientDesc>::iterator &it, const DataBlock &data);

public:
	TankServer(void);
	~TankServer(void);

	bool init(const LPGAMEINFO pGameInfo);
	void ShutDown();
};

///////////////////////////////////////////////////////////////////////////////
// end of file
