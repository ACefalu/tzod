// struct.h

#pragma once

#include "script.h"
#include "constants.h"

//----------------------------------------------------------

typedef struct VEHICLEKEYS
{
	int keyLeft;
	int keyRight;
	int keyForward;
	int keyBack;
	int keyFire;
	int keyLight;
	int keyDrop;
	int keyTowerLeft;
	int keyTowerCenter;
	int keyTowerRight;
} VEHICLEKEYS, *LPVEHICLEKEYS;

//----------------------------------------------------------

enum enumControlType
{
	CT_USER_KB,
	CT_USER_KB2,
	CT_USER_MOUSE,
	CT_USER_MOUSE2,
	CT_USER_HYBRID,
};

//----------------------------------------------------------

struct PLAYER
{
	VEHICLEKEYS     KeyMap;
	enumControlType ControlType;
};

//----------------------------------------------------------

#define MAX_PLRNAME	30
#define MAX_VEHCLSNAME	30

struct PlayerDesc
{
	char nick[MAX_PLRNAME];
	char skin[MAX_PATH];
	char cls[MAX_VEHCLSNAME];
	int  team;
	int  score;
};

struct PlayerDescEx : public PlayerDesc
{
	union
	{
		DWORD dwHasPlayers; // �����, ���������� ����������� ������� � ������
		DWORD dwNetworkId;
	};
};

//----------------------------------------------------------

#define MAX_SRVNAME 16

typedef struct GAMEINFO
{
	DWORD dwVersion;
	DWORD dwMapCRC32;
    char  cMapName[MAX_PATH];
	char  cServerName[MAX_SRVNAME];
	short server_fps;
	short latency;
	short timelimit;
	short fraglimit;
	short seed;
	bool  nightmode;
} GAMEINFO, *LPGAMEINFO;

//----------------------------------------------------------


//----------------------------------------------------------
// ��������� ��������� �������
typedef struct GETFILENAME
{
	DWORD   dwFlags;
	LPCTSTR lpszDirectory;
	LPCTSTR lpszActionName; // ������� �� ������ OK
	LPSTR   fileName;
	LPCTSTR lpszFileExt;    // ���������� (��������, "map")
	BOOL  (*lpValidateFunc)(LPCTSTR);
} GETFILENAME, *LPGETFILENAME;

#define GFNF_OVERWRITEPROMPT	1
#define GFNF_FILEMUSTEXIST		2

//----------------------------------------------------------

typedef float AIPRIORITY;


///////////////////////////////////////////////////////////////////////////////
// end of file
