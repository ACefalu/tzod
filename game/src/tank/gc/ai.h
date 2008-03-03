// ai.h: interface for the GC_PlayerAI class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Level.h"    // FIXME!
#include "Player.h"

// forward declarations
template<class T> class JobManager;
struct VehicleState;
struct AIWEAPSETTINGS;
class GC_Actor;
class GC_RigidBodyStatic;

///////////////////////////////////////////////////////////////////////////////

struct AIITEMINFO
{
	GC_Actor  *object;
	AIPRIORITY priority;
};

class GC_PlayerAI : public GC_Player
{
	DECLARE_SELF_REGISTRATION(GC_PlayerAI);

	static JobManager<GC_PlayerAI> _jobManager;

	typedef std::list<SafePtr<GC_RigidBodyStatic> > AttackListType;

protected:
	class MyPropertySet : public GC_Player::MyPropertySet
	{
		typedef GC_Player::MyPropertySet BASE;
		ObjectProperty _propLevel;

	public:
		MyPropertySet(GC_Object *object);
		virtual int GetCount() const;
		virtual ObjectProperty* GetProperty(int index);
		virtual void Exchange(bool applyToObject);
	};
	virtual PropertySet* NewPropertySet();


	/*
	 ���� ������� �� ������ ����� � ������ �����, ������� ����� ���� ������.

	 ���� ������ ���� � ���������� ���� � ������.
	 � �������� �������� ����� ���� ��������� �������������.
	 ���� ���� ������, ���� ����� �� �����.

	 � ������ ����� �� �������, ������� ���������� ���������� ��� ����,
	 ����� ������� � ����� ����������.
	----------------------------------------------------------*/

	// ���� ����
	struct PathNode
	{
		vec2d coord;
	};

	//
	// ������� ����
	//

	vec2d _arrivalPoint;
	std::list<PathNode> _path;  // ������ �����
	AttackListType _attackList;


	//-------------------------------------------------------------------------
	// Desc: ������ ���� �� �������� �����, ��������� ��������� ����
	//  dst_x, dst_y - ���������� ����� ����������
	//  max_depth    - ������������ ������� ������ � �������
	//  bTest        - ���� true, �� ����������� ������ ��������� ����
	// Return: ��������� ���� ��� -1 ���� ���� �� ������
	//-------------------------------------------------------------------------
	float CreatePath(float dst_x, float dst_y, float max_depth, bool bTest, const AIWEAPSETTINGS *ws);


	//-------------------------------------------------------------------------
	// Name: CreatePath()
	// Desc: ������� ������� ���� � ������ �����.
	//-------------------------------------------------------------------------
	void ClearPath();


	//-------------------------------------------------------------------------
	// Desc: ��������� � ������� ���� �������������� ���� ��� ��������� �����
	//       ������� ����������.
	//-------------------------------------------------------------------------
	void SmoothPath();


	// find the nearest node to the vehicle
	std::list<PathNode>::const_iterator FindNearPathNode(const vec2d &pos, vec2d *proj, float *offset) const;


	//-------------------------------------------------------------------------
	// Desc: �������� ������������ ������ ����.
	//-------------------------------------------------------------------------
	bool CheckCell(const FieldCell &cell);

	struct TargetDesc
	{
		GC_Vehicle *target;
        bool bIsVisible;
	};

	// ��������� ��
	enum aiState_l2
	{
		L2_PATH_SELECT,   // ��������� ��������� �� ������
		L2_PICKUP,        // ���� �� ���������
		L2_ATTACK,        // ���������� �, ���� ��������, ������� ����
	} _aiState_l2;

	// ��������� �������� ������
	enum aiState_l1
	{
		L1_NONE,           // ��� ���� �� �����
		L1_PATH_END,       // ��������� ����� ����
		L1_STICK,          // ��������
	} _aiState_l1;

protected:
	SafePtr<GC_Pickup>          _pickupCurrent;
	SafePtr<GC_RigidBodyStatic> _target;  // ������� ����

	bool IsTargetVisible(GC_RigidBodyStatic *target, GC_RigidBodyStatic** ppObstacle = NULL);
	void LockTarget(GC_RigidBodyStatic *target);
	void FreeTarget();
	AIPRIORITY GetTargetRate(GC_Vehicle *target);

	bool FindTarget(/*out*/ AIITEMINFO &info, const AIWEAPSETTINGS *ws);   // return true if target found
	bool FindItem(/*out*/ AIITEMINFO &info, const AIWEAPSETTINGS *ws);     // return true if something found

	void SelectFavoriteWeapon();

	// �������� ������� ��� ��������� �������� ��������
	float _desired_offset;
	float _current_offset;

	// ������� ������
	ObjectType _favoriteWeaponType;

	// ��������
	int _level;

	float _backTime;
	float _stickTime;

protected:
	void RotateTo(VehicleState *pState, const vec2d &x, bool bForv, bool bBack);
	void TowerTo (VehicleState *pState, const vec2d &x, bool bFire, const AIWEAPSETTINGS *ws);

	// ��������� ���������� ������ ���� ��� �������� �� ����������
	// target - ����
	// Vp      - �������� �������
	void CalcOutstrip(GC_Vehicle *target, float Vp, vec2d &fake);

	void ProcessAction(const AIWEAPSETTINGS *ws);

	void SetL1(GC_PlayerAI::aiState_l1 new_state); // ����������� ��������� l1
	void SetL2(GC_PlayerAI::aiState_l2 new_state); // ����������� ��������� l2

	void SelectState(const AIWEAPSETTINGS *ws);
	void DoState(VehicleState *pVehState, const AIWEAPSETTINGS *ws);

	virtual void Serialize(SaveFile &f);

public:
	GC_PlayerAI();
	GC_PlayerAI(FromFile);
	virtual ~GC_PlayerAI();
	virtual void Kill();

	virtual void OnRespawn();
	virtual void OnDie();

	void debug_draw();

	void SetLevel(int level) { _level = level; }
	int  GetLevel() const { return _level; }

protected:
	virtual DWORD GetNetworkID() const { return 0; }
	virtual void TimeStepFixed(float dt);
};

///////////////////////////////////////////////////////////////////////////////
// end of file
