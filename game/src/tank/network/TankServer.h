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

class LobbyClient;

class TankServer
{
	GameInfo _gameInfo;

	DWORD _nextFreeId;

	typedef std::list<SafePtr<PeerServer> >  PeerList;
	PeerList _clients;
	int _connectedCount;
	int _frameReadyCount;     // how much clients have ctrl data in buffer


	Socket _socketListen;

	SafePtr<LobbyClient> _announcer;

#ifdef NETWORK_DEBUG
	typedef std::map<unsigned int, DWORD> FrameToCSMap;
	FrameToCSMap _frame2cs;
#endif

	void SendFrame();

	void OnListenerEvent();
	void OnRecv(Peer *who, const DataBlock &db);
	void OnDisconnect(Peer *who, int err);

public:
	TankServer(const SafePtr<LobbyClient> &announcer);
	~TankServer(void);

	bool init(const GameInfo *info);
	void ShutDown();
};

///////////////////////////////////////////////////////////////////////////////
// end of file
