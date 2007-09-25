// gui_settings.cpp

#include "stdafx.h"
#include "gui_settings.h"

#include "Text.h"
#include "List.h"
#include "Button.h"
#include "Scroll.h"

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
	std::vector<string_t> profiles;
	g_conf.dm_profiles->GetKeyList(profiles);
	for( size_t i = 0; i < profiles.size(); ++i )
	{
		int index = _profiles->AddItem(profiles[i].c_str());
	}
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
}

SettingsDlg::~SettingsDlg()
{
}

void SettingsDlg::OnAddProfile()
{
}

void SettingsDlg::OnEditProfile()
{
	int i = _profiles->GetCurSel();
	_ASSERT(i >= 0);
	new ControlProfileDlg(this, g_conf.dm_profiles->GetTable(_profiles->GetItemText(i).c_str()));
}

void SettingsDlg::OnDeleteProfile()
{
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

///////////////////////////////////////////////////////////////////////////////
// class ControlProfileDlg

ControlProfileDlg::ControlProfileDlg(Window *parent, ConfVarTable *profile)
  : Dialog(parent, 10, 10, 512, 384)
  , _profile(profile)
{
	_ASSERT(profile);

	Move((parent->GetWidth() - GetWidth()) * 0.5f, (parent->GetHeight() - GetHeight()) * 0.5f);
	SetEasyMove(true);

	new Text(this,  10, 5, "��������", alignTextLT);
	new Text(this, 210, 5, "������", alignTextLT);
	_actions = new List(this, 10, 20, 400, 300);
	_actions->SetTabPos(0, 2);
	_actions->SetTabPos(1, 200);

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


	(new Button(this, 304, 360, "��"))->eventClick.bind(&ControlProfileDlg::OnOK, this);
	(new Button(this, 408, 360, "������"))->eventClick.bind(&ControlProfileDlg::OnCancel, this);
}

ControlProfileDlg::~ControlProfileDlg()
{
}

void ControlProfileDlg::AddAction(const char *rawname, const char *display)
{
	int index = _actions->AddItem(display);
	_actions->SetItemText(index, 1, 
		g_keys->GetName(g_keys->GetCode(_profile->GetStr(rawname)->Get())).c_str());
}

void ControlProfileDlg::OnOK()
{
	Dialog::Close(_resultOK);
}

void ControlProfileDlg::OnCancel()
{
	Dialog::Close(_resultCancel);
}

///////////////////////////////////////////////////////////////////////////////
} // end of namespace UI

// end of file