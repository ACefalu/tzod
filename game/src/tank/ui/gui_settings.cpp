// gui_settings.cpp

#include "stdafx.h"
#include "gui_settings.h"
#include "GuiManager.h"

#include "Text.h"
#include "List.h"
#include "Button.h"
#include "Scroll.h"
#include "Edit.h"

#include "config/Config.h"

#include "KeyMapper.h"


namespace UI
{
///////////////////////////////////////////////////////////////////////////////

SettingsDlg::SettingsDlg(Window *parent) : Dialog(parent, 0, 0, 512, 256)
{
	Move((parent->GetWidth() - GetWidth()) * 0.5f, (parent->GetHeight() - GetHeight()) * 0.5f);
	SetEasyMove(true);


	//
	// profiles
	//

	new Text(this, 8, 8, "�������", alignTextLT);
	_profiles = new List(this, 8, 24, 128, 104);
	UpdateProfilesList(); // fill the list before binding OnChangeSel
	_profiles->eventChangeCurSel.bind(&SettingsDlg::OnSelectProfile, this);

	(new Button(this, 16, 144, "��������"))->eventClick.bind(&SettingsDlg::OnAddProfile, this);
	_editProfile = new Button(this, 16, 176, "��������");
	_editProfile->eventClick.bind(&SettingsDlg::OnEditProfile, this);
	_editProfile->Enable( false );
	_deleteProfile = new Button(this, 16, 208, "�������");
	_deleteProfile->eventClick.bind(&SettingsDlg::OnDeleteProfile, this);
	_deleteProfile->Enable( false );

	//
	// other settings
	//

	float x = 192;
	float y = 8;

	_showFps = new CheckBox(this, x, y, "�������� FPS");
	_showFps->SetCheck(g_conf.ui_showfps->Get());
	y += _showFps->GetHeight();

	_showTime = new CheckBox(this, x, y, "�������� �����");
	_showTime->SetCheck(g_conf.ui_showtime->Get());
	y += _showTime->GetHeight();

	_particles = new CheckBox(this, x, y, "�������");
	_particles->SetCheck(g_conf.g_particles->Get());
	y += _particles->GetHeight();

	_showDamage = new CheckBox(this, x, y, "���������� �����������");
	_showDamage->SetCheck(g_conf.g_showdamage->Get());
	y += _showDamage->GetHeight();


//	_volume = new ScrollBar(this, x + 192, 8, 128 );
//	_volume->SetLimit(100);


	//
	// OK & Cancel
	//

	(new Button(this, 304, 216, "��"))->eventClick.bind(&SettingsDlg::OnOK, this);
	(new Button(this, 408, 216, "������"))->eventClick.bind(&SettingsDlg::OnCancel, this);


	_profiles->SetCurSel(0, true);
	GetManager()->SetFocusWnd(_profiles);
}

SettingsDlg::~SettingsDlg()
{
}

void SettingsDlg::OnAddProfile()
{
	(new ControlProfileDlg(this, NULL))->eventClose.bind(&SettingsDlg::OnProfileEditorClosed, this);
}

void SettingsDlg::OnEditProfile()
{
	int i = _profiles->GetCurSel();
	_ASSERT(i >= 0);
	(new ControlProfileDlg(this, _profiles->GetItemText(i).c_str()))
		->eventClose.bind(&SettingsDlg::OnProfileEditorClosed, this);
}

void SettingsDlg::OnDeleteProfile()
{
	int i = _profiles->GetCurSel();
	_ASSERT(i >= 0);
	g_conf.dm_profiles->Remove(_profiles->GetItemText(i).c_str());
	UpdateProfilesList();
}

void SettingsDlg::OnSelectProfile(int index)
{
	_editProfile->Enable( -1 != index );
	_deleteProfile->Enable( -1 != index );
}

void SettingsDlg::OnOK()
{
	g_conf.ui_showfps->Set(_showFps->GetCheck());
	g_conf.ui_showtime->Set(_showTime->GetCheck());
	g_conf.g_particles->Set(_particles->GetCheck());
	g_conf.g_showdamage->Set(_showDamage->GetCheck());

	Close(_resultOK);
}

void SettingsDlg::OnCancel()
{
	Close(_resultCancel);
}

void SettingsDlg::UpdateProfilesList()
{
	int sel = _profiles->GetCurSel();
	std::vector<string_t> profiles;
	g_conf.dm_profiles->GetKeyList(profiles);
	_profiles->DeleteAllItems();
	for( size_t i = 0; i < profiles.size(); ++i )
	{
		int index = _profiles->AddItem(profiles[i].c_str());
	}
	_profiles->SetCurSel(__min(_profiles->GetSize()-1, sel));
}

void SettingsDlg::OnProfileEditorClosed(int code)
{
	if( _resultOK == code )
	{
		UpdateProfilesList();
		GetManager()->SetFocusWnd(_profiles);
	}
}

///////////////////////////////////////////////////////////////////////////////
// class ControlProfileDlg

ControlProfileDlg::ControlProfileDlg(Window *parent, const char *profileName)
  : Dialog(parent, 10, 10, 512, 384)
{
	Move((parent->GetWidth() - GetWidth()) * 0.5f, (parent->GetHeight() - GetHeight()) * 0.5f);
	SetEasyMove(true);

	_time = 0;
	_activeIndex = -1;
	_skip = false;


	new Text(this, 20, 15, "�������� �������", alignTextLT);
	_nameEdit = new Edit(this, 20, 30, 250);

	if( profileName )
	{
		_name = profileName;
		_nameEdit->SetText(_name.c_str());
	}
	else
	{
		int i = 0;
		std::ostringstream buf;
		do
		{
			buf.str("");
			buf << "������� " << ++i;
		}
		while( g_conf.dm_profiles->Find(buf.str().c_str()) );
		_nameEdit->SetText(buf.str().c_str());
	}
	_profile = g_conf.dm_profiles->GetTable(_nameEdit->GetText().c_str());

	new Text(this,  20, 65, "��������", alignTextLT);
	new Text(this, 220, 65, "������", alignTextLT);
	_actions = new List(this, 20, 80, 400, 250);
	_actions->SetTabPos(0, 2);
	_actions->SetTabPos(1, 200);
	_actions->eventClickItem.bind(&ControlProfileDlg::OnSelectAction, this);

	AddAction( "key_forward"      , "������"            );
	AddAction( "key_back"         , "�����"             );
	AddAction( "key_left"         , "������� ������"    );
	AddAction( "key_right"        , "������� �������"   );
	AddAction( "key_fire"         , "�����!!!"          );
	AddAction( "key_light"        , "���� ���/����"     );
	AddAction( "key_tower_left"   , "������ ������"     );
	AddAction( "key_tower_right"  , "������ �������"    );
	AddAction( "key_tower_center" , "������ �� �����"   );
	AddAction( "key_pickup"       , "��������� �������" );
	_actions->SetCurSel(0, true);

	_aimToMouse = new CheckBox(this, 16, 340, "�������� ������");
	_aimToMouse->SetCheck(_profile->GetBool("aim_to_mouse", false)->Get());

	(new Button(this, 304, 350, "��"))->eventClick.bind(&ControlProfileDlg::OnOK, this);
	(new Button(this, 408, 350, "������"))->eventClick.bind(&ControlProfileDlg::OnCancel, this);

	GetManager()->SetFocusWnd(_actions);
}

ControlProfileDlg::~ControlProfileDlg()
{
}

void ControlProfileDlg::OnSelectAction(int index)
{
	_actions->SetItemText(index, 1, "...");
	_time = 0;
	_activeIndex = index;
	g_pKeyboard->SetCooperativeLevel( g_env.hMainWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	_skip = true;
	SetTimeStep(true);
}

void ControlProfileDlg::AddAction(const char *rawname, const char *display)
{
	int index = _actions->AddItem(display);
	_actions->SetItemData(index, (ULONG_PTR) rawname);
	_actions->SetItemText(index, 1, 
		g_keys->GetName(g_keys->GetCode(_profile->GetStr(rawname)->Get())).c_str());
}

void ControlProfileDlg::OnOK()
{
	_profile->SetBool("aim_to_mouse", _aimToMouse->GetCheck());
	for( int i = 0; i < _actions->GetSize(); ++i )
	{
		_profile->SetStr((const char *) _actions->GetItemData(i),
			_actions->GetItemText(i, 1).c_str());
	}
	Dialog::Close(_resultOK);
}

void ControlProfileDlg::OnCancel()
{
	if( _name.empty() )
	{
		g_conf.dm_profiles->Remove(_profile);
	}
	Dialog::Close(_resultCancel);
}

void ControlProfileDlg::OnTimeStep(float dt)
{
	_time += dt;
	_actions->SetItemText(_activeIndex, 1, fmodf(_time, 0.6f) > 0.3f ? "" : "...");

	for( int k = 0; k < sizeof(g_env.envInputs.keys) / sizeof(g_env.envInputs.keys[0]); ++k )
	{
		if( g_env.envInputs.keys[k] )
		{
			if( _skip )
			{
				return;
			}
			if( DIK_ESCAPE != k )
			{
				_actions->SetItemText(_activeIndex, 1, g_keys->GetName(k).c_str());
			}
			else
			{
				_actions->SetItemText(_activeIndex, 1, 
					g_keys->GetName(
						g_keys->GetCode(
							_profile->GetStr(
								(const char *) _actions->GetItemData(_activeIndex)
							)->Get()
						)
					).c_str()
				);
			}
			g_pKeyboard->SetCooperativeLevel(g_env.hMainWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
			SetTimeStep(false);
		}
	}

	_skip = false;
}

void ControlProfileDlg::OnRawChar(int c)
{
	switch(c)
	{
	case VK_RETURN:
		if( -1 != _actions->GetCurSel() )
		{
			OnSelectAction(_actions->GetCurSel());
		}
		break;
	default:
		Dialog::OnRawChar(c);
	}
}

///////////////////////////////////////////////////////////////////////////////
} // end of namespace UI

// end of file
