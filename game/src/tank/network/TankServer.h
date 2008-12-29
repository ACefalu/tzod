// TankServer.h

#pragma once

#include "Socket.h"
#include "datablock.h"
#include "TankClient.h"


class TankServer
{
	bool _init;
	GameInfo _gameInfo;

	struct ClientDesc
	{
		DWORD      id;               // ���������� ������������� �������
		Socket     s;                // ���������� ������
		WSAEVENT   stop;             // ������� �������� ������ ������� ����������
		//---------------------------//----------------------------
		PlayerDesc desc;             // �������� ������
		BOOL       connected;        // ���� ����������, ��� ���� desc ���������
		BOOL       ready;            // ����� ����� ������ ����
		//---------------------------//----------------------------
		HANDLE evData;               // ������� �������� ������� ������ � data
		CRITICAL_SECTION cs;         // ����������� ������ �������� ���� data � ctrl
		std::vector<DataBlock> data; // ������ ��� �������� � ����
		std::queue<ControlPacket> ctrl;// ����������� �� ���� ������ ����������
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


	bool TrySendFrame();         // �������� ����� ���� ��� ������ ��������
	static DWORD WINAPI MainProc(TankServer *pServer);

	void SendClientThreadData(const std::list<ClientDesc>::iterator &it, const DataBlock &data);

public:
	TankServer(void);
	~TankServer(void);

	bool init(const GameInfo *info);
	void ShutDown();
};

///////////////////////////////////////////////////////////////////////////////
// end of file
