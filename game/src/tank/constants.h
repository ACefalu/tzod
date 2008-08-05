// constants.h

#pragma once

//-----------------------------------------------------------------------------
// z-������
enum enumZOrder
{
	Z_EDITOR,           // ����� � ���������
	Z_WATER,            // ����
	Z_GAUSS_RAY,        // ��� �� ������ (�������� ��� ������� � ��� �����)
	Z_WALLS,            // �����
	Z_FREE_ITEM,        // ������� �� ����� ������� (�� ����� ��� ������)
	Z_VEHICLES,         // �����
	Z_ATTACHED_ITEM,    // �������, ������������ �� ����� (������ � �.�.)
	Z_PROJECTILE,       // ������� �������
	Z_EXPLODE,          // ������
	Z_VEHICLE_LABEL,    // ����� �� ����� (������, ��������, ...)
	Z_PARTICLE,         // �������, ���
	Z_WOOD,             // ���
	//------------------//
	Z_COUNT,            //
	//------------------//
	Z_NONE = 0x7FFFFFFF // �� ��������
};


//-----------------------------------------------------------------------------
// �������� ������� ������
#define SCORE_POS_NUMBER     16
#define SCORE_POS_NAME       48
#define SCORE_POS_SCORE      16 // �� ������� ���� �������
#define SCORE_LIMITS_LEFT    64
#define SCORE_TIMELIMIT_TOP  16
#define SCORE_FRAGLIMIT_TOP  36
#define SCORE_NAMES_TOP      64
#define SCORE_ROW_HEIGHT     24

//-----------------------------------------------------------------------------
// ��������� ������
#define LEVEL_MINSIZE   16
#define LEVEL_MAXSIZE   512
#define CELL_SIZE       32             // ������ ������
#define LOCATION_SIZE   (CELL_SIZE*8)  // ������ ������� (������ ���� ��� �������
                                       // � 2 ���� ������ ������ �������)

#define MAX_GAMESPEED   200
#define MIN_GAMESPEED   20

#define MAX_TIMELIMIT   180
#define MAX_FRAGLIMIT   1000

#define MAX_NETWORKSPEED    60
#define MIN_NETWORKSPEED    10

#define MAX_LATENCY         10
#define MIN_LATENCY         1


//-----------------------------------------------------------------------------
// ��������� ������
#define WEAP_MG_TIME_RELAX      3.0f    // ����� �������������� ������� � ��������

#define WEAP_RL_HOMMING_FACTOR  600.0f  // �-�� ����������� ������
#define WEAP_RL_HOMMING_TIME    4.0f    // ����� �� ������ ���� ��� ������

#define WEAP_BFG_HOMMING_FACTOR 500.0f  // �-�� ����������� bfg
#define WEAP_BFG_RADIUS         200.0f  // ������ ����������� ������ bfg

#define WEAP_RAM_PERCUSSION     8.0f

// �������� ������
#define SPEED_ROCKET         750.0f
#define SPEED_PLAZMA         800.0f
#define SPEED_BULLET        4000.0f
#define SPEED_TANKBULLET    1400.0f
#define SPEED_GAUSS        10000.0f
#define SPEED_ACBULLET      1800.0f
#define SPEED_DISK          2000.0f
#define SPEED_BFGCORE        500.0f
#define SPEED_SMOKE          vec2d(0, -40.0f)

// ����������� �� ��������
#define DAMAGE_ROCKET_AK47   50.0f
#define DAMAGE_BULLET         4.0f
#define DAMAGE_PLAZMA        30.0f
#define DAMAGE_TANKBULLET    60.0f
#define DAMAGE_BFGCORE       60.0f
#define DAMAGE_ACBULLET      15.0f
#define DAMAGE_GAUSS         60.0f
#define DAMAGE_GAUSS_FADE    15.0f
#define DAMAGE_DISK_MIN      20.0f
#define DAMAGE_DISK_MAX      28.0f
#define DAMAGE_DISK_FADE      3.0f
#define DAMAGE_RAM_ENGINE   100.0f


// �������� ��������
#define ANIMATION_FPS        25.0f  // ����� � �������


// ���������� �� �������
#define CH_DISTANCE_NORMAL  200.0f  // �������
#define CH_DISTANCE_THIN    150.0f  // � ��������

// �������� ����� �����
#define TOWER_ROT_SPEED      5.5f   // ������������ ��������
#define TOWER_ROT_ACCEL     15.5f   // ���������
#define TOWER_ROT_SLOWDOWN  30.1f   // ����������

// ����� ��������
#define BOOSTER_TIME        20.0f
#define PROTECT_TIME        15.0f


//-----------------------------------------------------------------------------
// ���������� ���������

// ���� ���������� <= AIP_NOTREQUIRED �� ��� ���������� �������
#define AIP_NOTREQUIRED     0.0f

// �������� ������� ����������
#define AIP_NORMAL          1.0f

// � ����������� ���������� �����������: p = (base - AIP_NORMAL * l / AI_MAX_DEPTH)
// ��� base - ������� ������� ����������, l - ���������� � �������


#define AIP_WEAPON_NORMAL   (AIP_NORMAL)        // ���������� ������, ����� ���� ���������
#define AIP_WEAPON_FAVORITE (AIP_NORMAL / 2)    // ����� ��� �������� ������
#define AIP_WEAPON_ADVANCED (AIP_NORMAL / 2)    // ���������� ������ � ������������ ��������
#define AIP_HEALTH          (AIP_NORMAL)        // ���������� ������, ����� �������� ����� �� ����
#define AIP_BOOSTER         (AIP_NORMAL)        // ������ ������
#define AIP_BOOSTER_HAVE    (AIP_BOOSTER / 10)  // ���� ������ ��� ����
#define AIP_SHOCK           (AIP_NORMAL)        // ����������
#define AIP_INVULN          (AIP_NORMAL)        // ������������

#define AI_MAX_DEPTH   50.0f
#define AI_MAX_SIGHT   20.0f
#define AI_MAX_LEVEL   4

//-----------------------------------------------------------------------------
// ��������� ������������ ���������
#define TURET_ROCKET_RELOAD  0.9f
#define TURET_CANON_RELOAD   0.4f
#define TURET_SIGHT_RADIUS   500

//-----------------------------------------------------------------------------
// ��������� ������
#define PLAYER_RESPAWNTIME   2.0f  // �������� �������� ������


//-----------------------------------------------------------------------------
// �������������

#define TXT_VERSION      "Tank: Zone of Death (1.48)"
#define TXT_WNDCLASS     "TankMainWindow"
#define TXT_PROPGRID     "TankPropertyGrid"
#define TXT_PROPGRIDINT  "TankPropertyGridInt"

// ������  �����  Save  ���  ���������  �������� �� ������������.
// �����  �  ������������  ������� �� ����� ������������ � ������
#define VERSION    0x1480

//-----------------------------------------------------------------------------
// �����������

// ������������ ����� �������
#define MAX_PLAYERS     32
#define MAX_HUMANS       4

// ������������ ����� ������ � ��������� ����� (������� 0)
#define MAX_TEAMS        6

// ������������ ����� dt. ���� ��� ������� �������� dt �����
// �������� ������, �� ��� ������������� ����� ��������� �� MAX_DT
#define MAX_DT           0.05f

#define MAX_DT_FIXED     0.02f


// ������������ ������� �������
#define NET_MULTIPLER    2

//-----------------------------------------------------------------------------
// ���� ����

// ����� ���������.
// � ���� ������ �� ������� timelimit � fraglimit,
// �������� �������� ������
#define GT_EDITOR        0

// ��� ���� deathmatch. ������ ��� �� ����.
#define GT_DEATHMATCH    1

// main menu intro
#define GT_INTRO         2


//-----------------------------------------------------------------------------
// �������������� ���������
#define PI    3.141593f
#define PI2   6.283185f


//-----------------------------------------------------------------------------
// �����
#define DIR_SCRIPTS      "scripts"
#define DIR_SAVE         "save"
#define DIR_MAPS         "maps"
#define DIR_SKINS        "skins"
#define DIR_THEMES       "themes"
#define DIR_MUSIC        "music"
#define DIR_SCREENSHOTS  "screenshots"

#define FILE_CONFIG      "config.cfg"
#define FILE_LANGUAGE    "lang.cfg"
#define FILE_TEXTURES    DIR_SCRIPTS"/textures.lua"
#define FILE_STARTUP     DIR_SCRIPTS"/init.lua"

///////////////////////////////////////////////////////////////////////////////
// end of file
