// gui_network.cpp

#include "stdafx.h"

#include "gui_network.h"
#include "gui_maplist.h"

#include "GuiManager.h"

#include "Text.h"
#include "Edit.h"
#include "Button.h"
#include "Console.h"

#include "Interface.h"
#include "functions.h"
#include "Level.h"
#include "Macros.h"

#include "config/Config.h"

#include "core/Console.h"

#include "network/TankServer.h"

#include "gc/Player.h"


namespace UI
{
///////////////////////////////////////////////////////////////////////////////

CreateServerDlg::CreateServerDlg(Window *parent)
  : Dialog(parent, 770, 450)
{
	Text *title = new Text(this, GetWidth() / 2, 16, "������� ������", alignTextCT);
	title->SetTexture("font_default");
	title->Resize(title->GetTextureWidth(), title->GetTextureHeight());

	float x1 = 16;
	float x2 = x1 + 550;
	float x3 = x2 + 20;
	float x4 = x3 + 20;

	//
	// map list
	//

	new Text(this, x1, 46, "�������� �����", alignTextLT);

	_maps = new MapList(this, x1, 62, x2 - x1, 300);
	GetManager()->SetFocusWnd(_maps);


	//
	// settings
	//

	{
		float y =  56;

		_nightMode = new CheckBox(this, x3, y, "������ �����");
		_nightMode->SetCheck( g_conf.cl_nightmode->Get() );


		new Text(this, x3, y+=30, "�������� ����, %", alignTextLT);
		_gameSpeed = new Edit(this, x4, y+=15, 80);
		_gameSpeed->SetInt(g_conf.cl_speed->GetInt());

		new Text(this, x3, y+=30, "����� ������", alignTextLT);
		_fragLimit = new Edit(this, x4, y+=15, 80);
		_fragLimit->SetInt(g_conf.cl_fraglimit->GetInt());

		new Text(this, x3, y+=30, "����� �������", alignTextLT);
		_timeLimit = new Edit(this, x4, y+=15, 80);
		_timeLimit->SetInt(g_conf.cl_timelimit->GetInt());

		new Text(this, x3+30, y+=30, "(0 - ��� ������)", alignTextLT);

		new Text(this, x3, y+=40, "�������� ����, fps", alignTextLT);
		_svFps = new Edit(this, x4, y+=15, 100);
		_svFps->SetInt(g_conf.sv_fps->GetInt());

		new Text(this, x3, y+=30, "��������", alignTextLT);
		_svLatency = new Edit(this, x4, y+=15, 100);
		_svLatency->SetInt(g_conf.sv_latency->GetInt());
	}

	Button *btn;
	btn = new Button(this, 544, 410, "�������");
	btn->eventClick.bind(&CreateServerDlg::OnOK, this);

	btn = new Button(this, 656, 410, "������");
	btn->eventClick.bind(&CreateServerDlg::OnCancel, this);
}

CreateServerDlg::~CreateServerDlg()
{
}

void CreateServerDlg::OnOK()
{
	string_t fn;
	int index = _maps->GetCurSel();
	if( -1 != index )
	{
		fn = _maps->GetItemText(index, 0);
	}
	else
	{
		//		MessageBoxT(NULL, "�������� �����", MB_OK|MB_ICONHAND);
		return;
	}

	script_exec(g_env.L, "reset()");


	string_t path = DIR_MAPS;
	path += "\\";
	path += fn + ".map";

	GAMEINFO gi = {0};
	gi.dwVersion  = VERSION;
	gi.dwMapCRC32 = CalcCRC32(path.c_str());
	gi.seed       = rand();
	gi.fraglimit  = __max(0, __min(MAX_FRAGLIMIT, _fragLimit->GetInt()));
	gi.timelimit  = __max(0, __min(MAX_TIMELIMIT, _timeLimit->GetInt()));
	gi.server_fps = __max(MIN_NETWORKSPEED, __min(MAX_NETWORKSPEED, _svFps->GetInt()));
	gi.latency    = __max(MIN_LATENCY, __min(MAX_LATENCY, _svLatency->GetInt()));
	gi.nightmode  = _nightMode->GetCheck();

	strcpy(gi.cMapName, fn.c_str());
	strcpy(gi.cServerName, "ZOD Server");

	_ASSERT(NULL == g_server);
	g_server = new TankServer();
	if( !g_server->init(&gi) )
	{
		SAFE_DELETE(g_server);
		MessageBoxT(g_env.hMainWnd, "�� ������� ��������� ������. "
			"��������� ������������ ����", MB_OK|MB_ICONHAND);
		return;
	}

	g_conf.cl_map->Set(fn.c_str());

	(new ConnectDlg(GetParent(), "localhost"))->eventClose.bind(&CreateServerDlg::OnCloseChild, this);
	Show(false);
}

void CreateServerDlg::OnCancel()
{
	Close(_resultCancel);
}

void CreateServerDlg::OnCloseChild(int result)
{
	if( _resultCancel == result )
	{
		Show(true);
	}

	if( _resultOK == result )
	{
		Close(_resultOK);
	}
}


///////////////////////////////////////////////////////////////////////////////

ConnectDlg::ConnectDlg(Window *parent, const char *autoConnect)
  : Dialog(parent, 512, 384)
{
	Text *title = new Text(this, GetWidth() / 2, 16, "���������� � ��������", alignTextCT);
	title->SetTexture("font_default");
	title->Resize(title->GetTextureWidth(), title->GetTextureHeight());


	new Text(this, 20, 65, "����� �������", alignTextLT);
	_name = new Edit(this, 25, 80, 300);
	_name->SetText(g_conf.cl_server->Get());


	new Text(this, 20, 105, "������", alignTextLT);
	_status = new List(this, 25, 120, 400, 180);


	_btnOK = new Button(this, 312, 350, "����������");
	_btnOK->eventClick.bind(&ConnectDlg::OnOK, this);

	(new Button(this, 412, 350, "������"))->eventClick.bind(&ConnectDlg::OnCancel, this);

	GetManager()->SetFocusWnd(_name);


	if( autoConnect )
	{
		_ASSERT(g_server);
		_ASSERT(!g_level);

		_name->SetText(autoConnect);
		OnOK();
	}
}

ConnectDlg::~ConnectDlg()
{
}

void ConnectDlg::OnOK()
{
	_status->DeleteAllItems();

	_btnOK->Enable(false);
	_name->Enable(false);

	if( g_level )
	{
		script_exec(g_env.L, "reset()");
	}
	_ASSERT(NULL == g_level);
	_ASSERT(NULL == g_client);
	g_client = new TankClient();

	if( !g_client->Connect(_name->GetText().c_str(), g_env.hMainWnd) )
	{
		Error("������ ����!");
	}
	else
	{
		SetTimeStep(true);
	}
}

void ConnectDlg::OnCancel()
{
	if( g_server )
	{
		SAFE_DELETE(g_level);
		SAFE_DELETE(g_client);
		SAFE_DELETE(g_server);
	}

	Close(_resultCancel);
}

void ConnectDlg::OnTimeStep(float dt)
{
	DataBlock db;
	while( g_client->GetData(db) )
	{
		switch( db.type() )
		{
			case DBTYPE_GAMEINFO:
			{
				GAMEINFO &gi = db.cast<GAMEINFO>();

				if( VERSION != gi.dwVersion )
				{
					Error("������������� ������ �������");
					break;
				}

				g_conf.sv_timelimit->SetInt(gi.timelimit);
				g_conf.sv_fraglimit->SetInt(gi.fraglimit);
				g_conf.sv_fps->SetInt(gi.server_fps);
				g_conf.sv_nightmode->Set(gi.nightmode);

				char msg[MAX_PATH + 32];
				sprintf(msg, "�������� ����� '%s'...", gi.cMapName);
				_status->AddItem(msg);

				char path[MAX_PATH];
				wsprintf(path, "%s\\%s.map", DIR_MAPS, gi.cMapName);

				if( CalcCRC32(path) != gi.dwMapCRC32 )
				{
					Error("������������� ������ �����");
					break;
				}


				SAFE_DELETE(g_level);
				g_level = new Level();
				g_level->Pause(true);

				if( g_level->init_newdm(path) )
				{
					g_conf.cl_map->Set(gi.cMapName);
				}
				else
				{
					Error("�� ������� ��������� �����");
					break;
				}

				(new WaitingForPlayersDlg(GetParent()))->eventClose = eventClose;
				Close(-1); // close with any code except ok and cancel
				break;
			}

			case DBTYPE_ERRORMSG:
				Error((const char *) db.data());
				break;

			case DBTYPE_TEXTMESSAGE:
				_status->AddItem((const char *) db.data());
				break;
			default:
				_ASSERT(FALSE);
		}

		if( !g_client ) break;
	}
}

void ConnectDlg::Error(const char *msg)
{
	_status->AddItem(msg);
	SAFE_DELETE(g_level);
	SAFE_DELETE(g_client);
	if( g_server )
	{
		SAFE_DELETE(g_server);
	}
	else
	{
		_btnOK->Enable(true);
		_name->Enable(true);
	}
	SetTimeStep(false);
}

///////////////////////////////////////////////////////////////////////////////

WaitingForPlayersDlg::WaitingForPlayersDlg(Window *parent)
  : Dialog(parent, 700, 512)
{
	Text *title = new Text(this, GetWidth() / 2, 16, "�������� ������ �������", alignTextCT);
	title->SetTexture("font_default");
	title->Resize(title->GetTextureWidth(), title->GetTextureHeight());

	new Text(this, 20, 40, "��� � ����", alignTextLT);
	_players = new List(this, 20, 55, 512, 200);


	_buf = new ConsoleBuffer(80, 500);
	_chat = new Console(this, 20, 300, 512, 200, _buf);
	_chat->SetEcho(false);

	_btnOK = new Button(this, 312, 350, "� �����!");
	_btnOK->eventClick.bind(&WaitingForPlayersDlg::OnOK, this);
	_btnOK->Enable(false);

	(new Button(this, 412, 350, "������"))->eventClick.bind(&WaitingForPlayersDlg::OnCancel, this);


	PlayerDescEx pde;
	pde.dwNetworkId = g_client->GetId();
	strcpy(pde.nick, "player");
	strcpy(pde.cls, "");
	pde.score = 0;
	strcpy(pde.skin, "");
	pde.team = 0;
	g_client->SendDataToServer(DataWrap(pde, DBTYPE_NEWPLAYER));

	SetTimeStep(true);
}

WaitingForPlayersDlg::~WaitingForPlayersDlg()
{
	SAFE_DELETE(_buf);
}

void WaitingForPlayersDlg::OnOK()
{
	DataBlock db(sizeof(dbPlayerReady));
	db.type() = DBTYPE_PLAYERREADY;
	db.cast<dbPlayerReady>().player_id = g_client->GetId();
	db.cast<dbPlayerReady>().ready = TRUE;
	g_client->SendDataToServer(db);
}

void WaitingForPlayersDlg::OnCancel()
{
	SAFE_DELETE(g_level);
	SAFE_DELETE(g_client);
	SAFE_DELETE(g_server);

	Close(_resultCancel);
}

void WaitingForPlayersDlg::OnTimeStep(float dt)
{
	DataBlock db;
	while( g_client->GetData(db) )
	{
		switch( db.type() )
		{
		case DBTYPE_PING:
			_buf->printf("%d ms\n", timeGetTime() - db.cast<DWORD>());
			break;
		case DBTYPE_PLAYERREADY:
		{
			int count = g_level->GetList(LIST_players).size();
			_ASSERT(_players->GetSize() == count);

			const DWORD who = db.cast<dbPlayerReady>().player_id;

			int index = 0;
			for( ;index < count; ++index )
			{
				GC_PlayerRemote *player = (GC_PlayerRemote *) _players->GetItemData(index);
				_ASSERT(player);
				_ASSERT(!player->IsKilled());
				_ASSERT(0 != player->GetNetworkID());

				if( who == player->GetNetworkID() )
				{
					if( db.cast<dbPlayerReady>().ready )
					{
						_players->SetItemText(index, 3, "�����");
					}
					else
					{
						_players->SetItemText(index, 3, "");
					}
					break;
				}
			}
			_ASSERT(index < count);
			break;
		}

		case DBTYPE_PLAYERQUIT:
		{
			int count = g_level->GetList(LIST_players).size();
			_ASSERT(_players->GetSize() == count);

			const DWORD who = db.cast<DWORD>();

			int index = 0;
			for( ; index < count; ++index )
			{
				GC_PlayerRemote *player = (GC_PlayerRemote *) _players->GetItemData(index);
				_ASSERT(player);
				_ASSERT(!player->IsKilled());
				_ASSERT(0 != player->GetNetworkID());

				if( who == player->GetNetworkID() )
				{
					_players->DeleteItem(index);
					_buf->printf("%s ������� ����.\n", player->GetName());
					player->Kill();
					break;
				}
			}
			_ASSERT(index < count);
			break;
		}

		case DBTYPE_NEWPLAYER:
		{
			PlayerDescEx &pd = db.cast<PlayerDescEx>();

			GC_PlayerRemote *player = new GC_PlayerRemote(pd.dwNetworkId);
			player->SetNick(pd.nick);
			player->SetSkin(pd.skin);
			player->SetTeam(pd.team);
			player->UpdateSkin();
		//	player->SetController( pd.type );

			_players->AddItem(player->GetNick().c_str(), (UINT_PTR) player);
		//	if( pd.type >= MAX_HUMANS )
		//		ListView_SetItemText(hwndLV, ListView_GetItemCount(hwndLV) - 1, 3, "���");

			if( g_client->GetId() )
			{
				_btnOK->Enable(true);
			}
			_buf->printf("%s ����� � ����.\n", player->GetNick().c_str());
			break;
		}

		case DBTYPE_ERRORMSG:
			_btnOK->Enable(false);
		case DBTYPE_TEXTMESSAGE:
			_buf->printf("%s\n", (const char *) db.data());
			break;

		case DBTYPE_STARTGAME:
		{
			g_level->Pause(false);
			for( size_t i = 0; i < g_client->_dwLatency; ++i )
			{
				g_client->SendControl(ControlPacket());
			}
			Close(_resultOK);
			break;
		}
		default:
			_ASSERT(FALSE);
		} // end of switch( db.type() )
	}
}

///////////////////////////////////////////////////////////////////////////////
} // end of namespace UI

// end of file
