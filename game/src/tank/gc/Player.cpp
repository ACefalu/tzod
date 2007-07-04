// Player.cpp

#include "stdafx.h"

#include "Player.h"

#include "macros.h"
#include "level.h"

#include "KeyMapper.h"

#include "fs/SaveFile.h"

#include "network/TankClient.h"

#include "config/Config.h"

#include "video/TextureManager.h"

#include "core/Console.h"
#include "core/debug.h"

#include "ui/GuiManager.h"
#include "ui/gui_desktop.h"
#include "ui/gui.h"


#include "GameClasses.h"
#include "Camera.h"
#include "vehicle.h"
#include "indicators.h"
#include "particles.h"


//////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SELF_REGISTRATION(GC_Player)
{
	return true;
}

GC_Player::GC_Player()
  : GC_Service(), _memberOf(g_level->players, this)
{
	_timeRespawn = PLAYER_RESPAWNTIME;

	_team  = 0;
	_score = 0;

	SetEvents(GC_FLAG_OBJECT_EVENTS_TS_FIXED);


	// select nick from the random_names table
	lua_getglobal(g_env.L, "random_names");          // push table
	lua_pushinteger(g_env.L, g_level->net_rand() % lua_objlen(g_env.L, -1) + 1);  // push key
	lua_gettable(g_env.L, -2);                       // pop key, push value
	SetNick(lua_tostring(g_env.L, -1));              // get value
	lua_pop(g_env.L, 2);                             // pop value and table


	// select random class
	int count = 0;
	lua_getglobal(g_env.L, "classes");
	for( lua_pushnil(g_env.L); lua_next(g_env.L, -2); lua_pop(g_env.L, 1) )
	{
		if( 0 == g_level->net_rand() % ++count )
		{
			SetClass(lua_tostring(g_env.L, -2));  // get vehicle class
		}
	}
	lua_pop(g_env.L, 1); // remove classes table

	// select the default red skin
	SetSkin("red");
}

GC_Player::GC_Player(FromFile)
  : GC_Service(FromFile()), _memberOf(g_level->players, this)
{
}

GC_Player::~GC_Player()
{
}

void GC_Player::Serialize(SaveFile &f)
{
	GC_Service::Serialize(f);

	f.Serialize(_scriptOnDie);
	f.Serialize(_nick);
	f.Serialize(_skin);
	f.Serialize(_class);
	f.Serialize(_score);
	f.Serialize(_team);
	f.Serialize(_timeRespawn);
	f.Serialize(_vehicle);
}

void GC_Player::Kill()
{
	if( _vehicle )
		_vehicle->Kill();	// ������ ����� ���������� � OnVehicleKill()
	GC_Service::Kill();
}

void GC_Player::SetSkin(const string_t &skin)
{
	_skin = skin;
	UpdateSkin();
}

void GC_Player::SetNick(const string_t &nick)
{
	_nick = nick;
}

void GC_Player::SetClass(const string_t &c)
{
	_class = c;
}

void GC_Player::SetTeam(int team)
{
	_team = team;
}

void GC_Player::UpdateSkin()
{
	if( _vehicle )
		_vehicle->SetSkin(_skin.c_str());
}

void GC_Player::SetScore(int score)
{
	_score = score;
	if( g_conf.sv_fraglimit->GetInt() )
	{
		if( _score >= g_conf.sv_fraglimit->GetInt() )
		{
			g_level->Pause(true);
			g_level->_limitHit = true;
		}
	}
}

void GC_Player::OnRespawn()
{
}

void GC_Player::OnDie()
{
}

void GC_Player::TimeStepFixed(float dt)
{
	GC_Service::TimeStepFixed( dt );

	if( IsDead() )
	{
		_timeRespawn -= dt;
		if( _timeRespawn <= 0 )
		{
			//
			// Respawn
			//

			_ASSERT(IsDead());
			_timeRespawn = PLAYER_RESPAWNTIME;

			std::vector<GC_SpawnPoint*> points;
			GC_SpawnPoint *pSpawnPoint;

			GC_SpawnPoint *pBestPoint = NULL; // ����������� �����
			float max_dist = -1;

			FOREACH( respawns, GC_SpawnPoint, object )
			{
				pSpawnPoint = (GC_SpawnPoint*) object;
				if( pSpawnPoint->_team && (pSpawnPoint->_team != _team) )
					continue;

				float dist = -1;
				FOREACH( vehicles, GC_Vehicle, pVeh )
				{
					if( pVeh->IsKilled() ) continue;
					float d = (pVeh->GetPos() - pSpawnPoint->GetPos()).Square();
					if( d < dist || dist < 0 ) dist = d;
				}

				if( dist > 0 && dist < 4*CELL_SIZE*CELL_SIZE )
				{
					continue;
				}

				if( dist < 0 || dist > 400*CELL_SIZE*CELL_SIZE )
					points.push_back(pSpawnPoint);

				if( dist > max_dist )
				{
					max_dist = dist;
					pBestPoint = pSpawnPoint;
				}
			}

			if( !pBestPoint && points.empty() )
			{
				char buf[64];
				wsprintf(buf, "��� ������� %d ��� ����� ��������!", _team);
				static_cast<UI::Desktop*>(g_gui->GetDesktop())->GetMsgArea()->puts(buf);
				return;
			}

			if( !points.empty() )
			{
				pBestPoint = points[g_level->net_rand() % points.size()];
			}

			new GC_Text_ToolTip(pBestPoint->GetPos(), _nick.c_str(), "font_default");


			//if( !dynamic_cast<AIController*>(_controller) )
			//{
			//	static const TextureCache tex1("particle_1");
			//	for( int n = 0; n < 64; ++n )
			//	{
			//		vec2d a(PI2 * (float) n / 64);
			//		new GC_Particle(pBestPoint->GetPos() + a * 28, a * 28, tex1, frand(0.5f) + 0.1f);
			//	}
			//}

			_vehicle = new GC_Tank_Light(pBestPoint->GetPos().x, pBestPoint->GetPos().y);
			_vehicle->SetBodyAngle(pBestPoint->GetRotation());
			_vehicle->SetPlayer(this);

			_vehicle->Subscribe(NOTIFY_RIGIDBODY_DESTROY, this,
				(NOTIFYPROC) &GC_Player::OnVehicleDestroy, true, false);
			_vehicle->Subscribe(NOTIFY_OBJECT_KILL, this,
				(NOTIFYPROC) &GC_Player::OnVehicleKill, true, false);

			_vehicle->SetClass(GetClass());

			UpdateSkin();
			OnRespawn();
		}
	}
}

void GC_Player::OnVehicleDestroy(GC_Object *sender, void *param)
{
	_vehicle->Unsubscribe(this);
	_vehicle = NULL;
	OnDie();
	if( !_scriptOnDie.empty() )
	{
		script_exec(g_env.L, _scriptOnDie.c_str());
	}
}

void GC_Player::OnVehicleKill(GC_Object *sender, void *param)
{
	_vehicle->Unsubscribe(this);
	_vehicle = NULL;
	OnDie();
}

SafePtr<PropertySet> GC_Player::GetProperties()
{
	return new MyPropertySet(this);
}

GC_Player::MyPropertySet::MyPropertySet(GC_Object *object)
: BASE(object)
, _propTeam(  ObjectProperty::TYPE_INTEGER,     "team"   )
, _propScore( ObjectProperty::TYPE_INTEGER,     "score"  )
, _propNick(  ObjectProperty::TYPE_STRING,      "nick"   )
, _propClass( ObjectProperty::TYPE_MULTISTRING, "class"  )
, _propSkin(  ObjectProperty::TYPE_MULTISTRING, "skin"   )
, _propOnDie( ObjectProperty::TYPE_STRING,      "on_die" )
{
	_propTeam.SetRange(0, MAX_TEAMS);


	lua_getglobal(g_env.L, "classes");
	for( lua_pushnil(g_env.L); lua_next(g_env.L, -2); lua_pop(g_env.L, 1) )
	{
		// now 'key' is at index -2 and 'value' at index -1
		_propClass.AddItem(lua_tostring(g_env.L, -2));
	}
	lua_pop(g_env.L, 1); // pop classes table


	std::vector<string_t> skin_names;
	g_texman->GetTextureNames(skin_names, "skin/", true);
	for( size_t i = 0; i < skin_names.size(); ++i )
	{
		_propSkin.AddItem( skin_names[i]);
	}


	//-----------------------------------------
	Exchange(false);
}

int GC_Player::MyPropertySet::GetCount() const
{
	return BASE::GetCount() + 6;
}

ObjectProperty* GC_Player::MyPropertySet::GetProperty(int index)
{
	if( index < BASE::GetCount() )
		return BASE::GetProperty(index);

	switch( index - BASE::GetCount() )
	{
		case 0: return &_propTeam;
		case 1: return &_propScore;
		case 2: return &_propNick;
		case 3: return &_propClass;
		case 4: return &_propSkin;
		case 5: return &_propOnDie;
	}

	_ASSERT(FALSE);
	return NULL;
}

void GC_Player::MyPropertySet::Exchange(bool applyToObject)
{
	BASE::Exchange(applyToObject);

	GC_Player *tmp = static_cast<GC_Player *>(GetObject());

	if( applyToObject )
	{
		tmp->SetTeam( _propTeam.GetValueInt() );
		tmp->SetScore( _propScore.GetValueInt() );
		tmp->SetNick( _propNick.GetValue() );
		tmp->SetClass( _propClass.GetSetValue(_propClass.GetCurrentIndex()) );
		tmp->SetSkin( _propSkin.GetSetValue(_propSkin.GetCurrentIndex()) );
		tmp->_scriptOnDie = _propOnDie.GetValue();
	}
	else
	{
		_propOnDie.SetValue(tmp->_scriptOnDie);
		_propTeam.SetValueInt(tmp->GetTeam());
		_propScore.SetValueInt(tmp->GetScore());
		_propNick.SetValue(tmp->GetNick());

		for( size_t i = 0; i < _propClass.GetSetSize(); ++i )
		{
			if( tmp->GetClass() == _propClass.GetSetValue(i) )
			{
				_propClass.SetCurrentIndex(i);
				break;
			}
		}

		for( size_t i = 0; i < _propSkin.GetSetSize(); ++i )
		{
			if( tmp->GetSkin() == _propSkin.GetSetValue(i) )
			{
				_propSkin.SetCurrentIndex(i);
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SELF_REGISTRATION(GC_PlayerLocal)
{
	ED_SERVICE("player_local", "�����");
	return true;
}

GC_PlayerLocal::GC_PlayerLocal()
{
	new GC_Camera(this);
	
	// select first available profile
	std::vector<string_t> tmp;
	g_conf.dm_profiles->GetKeyList(tmp);
	SetProfile(tmp.empty() ? "" : tmp[0].c_str());
}

GC_PlayerLocal::GC_PlayerLocal(FromFile)
{
}

GC_PlayerLocal::~GC_PlayerLocal()
{
}

void GC_PlayerLocal::Serialize(SaveFile &f)
{
	GC_Player::Serialize(f);
	f.Serialize(_profile);
	if( f.loading() )
	{
		SetProfile(_profile.c_str());
	}
}

void GC_PlayerLocal::TimeStepFixed(float dt)
{
	GC_Player::TimeStepFixed( dt );

	ControlPacket cp;

#ifdef NETWORK_DEBUG
	cp.checksum = g_level->_dwChecksum;
#endif

	if( IsDead() )
	{
		if( g_level->_client )
		{
			g_level->_client->SendControl(cp);
			bool ok = g_level->_client->RecvControl(ControlPacket());
			_ASSERT(ok);
		}
	}
	else
	{
		VehicleState vs;
		GetControl(vs);

		if( g_level->_client  )
		{
			cp.fromvs(vs);
			g_level->_client->SendControl(cp);
			bool ok = g_level->_client->RecvControl(cp);
			_ASSERT(ok);
			cp.tovs(vs);
		}

		GetVehicle()->SetState(vs);
	}
}

void GC_PlayerLocal::SetProfile(const char *name)
{
	_profile = name;
	ConfVar *p = g_conf.dm_profiles->Find(name);

	if( p && ConfVar::typeTable == p->GetType() )
	{
		ConfVarTable *t = static_cast<ConfVarTable *>(p);

		_keyForward     = g_keys->GetCode(t->GetStr( "key_forward"      )->Get());
		_keyBack        = g_keys->GetCode(t->GetStr( "key_back"         )->Get());
		_keyLeft        = g_keys->GetCode(t->GetStr( "key_left"         )->Get());
		_keyRight       = g_keys->GetCode(t->GetStr( "key_right"        )->Get());
		_keyFire        = g_keys->GetCode(t->GetStr( "key_fire"         )->Get());
		_keyLight       = g_keys->GetCode(t->GetStr( "key_light"        )->Get());
		_keyTowerLeft   = g_keys->GetCode(t->GetStr( "key_tower_left"   )->Get());
		_keyTowerRight  = g_keys->GetCode(t->GetStr( "key_tower_right"  )->Get());
		_keyTowerCenter = g_keys->GetCode(t->GetStr( "key_tower_center" )->Get());
		_keyPickup      = g_keys->GetCode(t->GetStr( "key_pickup"       )->Get());
	}
	else
	{
		_keyForward     = 0;
		_keyBack        = 0;
		_keyLeft        = 0;
		_keyRight       = 0;
		_keyFire        = 0;
		_keyLight       = 0;
		_keyTowerLeft   = 0;
		_keyTowerRight  = 0;
		_keyTowerCenter = 0;
		_keyPickup      = 0;

		TRACE("WARNING: profile '%s' not found\n", name);
	}
}

void GC_PlayerLocal::GetControl(VehicleState &vs)
{
	ZeroMemory(&vs, sizeof(VehicleState));

	vs._bState_MoveForward = g_env.envInputs.keys[_keyForward] != 0;
	vs._bState_MoveBack    = g_env.envInputs.keys[_keyBack   ] != 0;
	vs._bState_RotateLeft  = g_env.envInputs.keys[_keyLeft   ] != 0;
	vs._bState_RotateRight = g_env.envInputs.keys[_keyRight  ] != 0;
	vs._bState_Fire        = g_env.envInputs.keys[_keyFire   ] != 0;

//	bool tmp = g_env.envInputs.keys[_keyLight] != 0;
//	if( tmp && !_last_light_key_state )
//	{
//		PLAY(SND_LightSwitch, _player->GetVehicle()->GetPos());
//		_bLight = !_bLight;
//	}
//	_last_light_key_state = tmp;
//	vs._bLight = _bLight;

	vs._bState_AllowDrop = 
		(g_env.envInputs.keys[_keyForward] != 0 && g_env.envInputs.keys[_keyBack] != 0) ||
		(g_env.envInputs.keys[_keyLeft] != 0 && g_env.envInputs.keys[_keyRight] != 0) ||
		g_env.envInputs.keys[_keyPickup] != 0;

	vs._bState_TowerLeft   = g_env.envInputs.keys[_keyTowerLeft  ] != 0;
	vs._bState_TowerRight  = g_env.envInputs.keys[_keyTowerRight ] != 0;

	vs._bState_TowerCenter = g_env.envInputs.keys[_keyTowerCenter] != 0 ||
		g_env.envInputs.keys[_keyTowerLeft] != 0 && g_env.envInputs.keys[_keyTowerRight] != 0;

	if( vs._bState_TowerCenter )
	{
		vs._bState_TowerLeft  = 0;
		vs._bState_TowerRight = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SELF_REGISTRATION(GC_PlayerRemote)
{
	return true;
}

GC_PlayerRemote::GC_PlayerRemote()
{
	_networkId = 0;
}

GC_PlayerRemote::GC_PlayerRemote(FromFile)
{
}

GC_PlayerRemote::~GC_PlayerRemote()
{
}

void GC_PlayerRemote::Serialize(SaveFile &f)
{
	GC_Player::Serialize(f);
	f.Serialize(_networkId);
}

void GC_PlayerRemote::TimeStepFixed(float dt)
{
	_ASSERT(g_level->_client);

	GC_Player::TimeStepFixed( dt );
	
	if( IsDead() )
	{
		bool ok = g_level->_client->RecvControl(ControlPacket());
		_ASSERT(ok);
	}
	else
	{
		ControlPacket cp;
		bool ok = g_level->_client->RecvControl(cp);
		_ASSERT(ok);

		VehicleState vs;
		cp.tovs(vs);
		GetVehicle()->SetState(vs);
	}
}



///////////////////////////////////////////////////////////////////////////////
// end of file
