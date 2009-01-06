// TankServer.h

#pragma once

#include "Peer.h"

class PeerServer : public Peer
{
public:
	DWORD      id;               // ���������� ������������� �������
	PlayerDesc desc;             // �������� ������
	BOOL       connected;        // ���� ����������, ��� ���� desc ���������
	BOOL       ready;            // ����� ����� ������ ����
	std::queue<ControlPacket> ctrl;// ����������� �� ���� ������ ����������

	PeerServer(SOCKET s_);
};

///////////////////////////////////////////////////////////////////////////////

class TankServer
{
	GameInfo _gameInfo;

	DWORD _nextFreeId;

	typedef std::list<SafePtr<PeerServer> >  PeerList;
	PeerList _clients;


	Socket _socketListen;


	bool TrySendFrame();         // �������� ����� ���� ��� ������ ��������
	static DWORD WINAPI MainProc(TankServer *pServer);

	void OnListenerEvent();
	void OnRecv(Peer *who, const DataBlock &db);

public:
	TankServer(void);
	~TankServer(void);

	bool init(const GameInfo *info);
	void ShutDown();
};

///////////////////////////////////////////////////////////////////////////////
// end of file
