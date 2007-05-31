// ai.h: interface for the AIController class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Controller.h"

#include "Level.h" // FIXME!
#include "GameClasses.h" // FIXME!

class GC_Object;
class GC_RigidBodyStatic;

//----------------------------------------------------------

struct AIITEMINFO
{
	GC_Object  *object;
	AIPRIORITY priority;
};

struct AIWEAPSETTINGS
{
	BOOL  bNeedOutstrip;       // FALSE, ���� ���������� ������ (gauss, ...)
	float fMaxAttackAngle;     // ������������ ���������� ����
	float fProjectileSpeed;    // �������� �������
	float fAttackRadius_min;   // ����������� ������ �����
	float fAttackRadius_max;   // ������������ ������ �����
	float fAttackRadius_crit;  // ����������� ������ �����, ����� ����� �������
	float fDistanceMultipler;  // ��������� ���������� ����
};

//----------------------------------------------------------

class CAttackList
{
	// ������������� ���� (�����, ������...)
	struct tagAttackNode
	{
		tagAttackNode      *_nextNode;
		tagAttackNode      *_prevNode;
		GC_RigidBodyStatic *_target;
	} *_firstTarget, *_lastTarget;

	static MemoryManager<tagAttackNode> s_anAllocator;

protected:
	tagAttackNode* FindObject(GC_RigidBodyStatic *object);
	void RemoveFromList(tagAttackNode *pNode);  // ������� ���� �� ������

public:
	CAttackList();
	CAttackList(CAttackList &al);
	virtual ~CAttackList();

	GC_RigidBodyStatic* Pop(BOOL bRemoveFromList = TRUE);  // ������� ���� �� ������ ������
	void PushToBegin(GC_RigidBodyStatic *target);          // ��������� ���� � ������ ������
	void PushToEnd  (GC_RigidBodyStatic *target);          // ��������� ���� � �����  ������

	void Clean();                                   // ���������� ��� ������ �������
	void ClearList() { while (!IsEmpty()) Pop(); }  // �������� ������

	inline BOOL IsEmpty() {return (NULL == _firstTarget);}

public:
	CAttackList& operator= (CAttackList &al);
};


template<class T> class JobManager;

class AIController : public CController
{
	static JobManager<AIController> _jobManager;


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

	std::list<PathNode> _path;  // ������ �����
	CAttackList  _AttackList;   // ������ �����



	//-------------------------------------------------------------------------
	// Desc: ������ ���� �� �������� �����, ��������� ��������� ����
	//  dst_x, dst_y - ���������� ����� ����������
	//  max_depth    - ������������ ������� ������ � �������
	//  bTest        - ���� true, �� ����������� ������ ��������� ����
	// Return: ��������� ���� ��� -1 ���� ���� �� ������
	//-------------------------------------------------------------------------
	float CreatePath(float dst_x, float dst_y, float max_depth, bool bTest);


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
	SafePtr<GC_PickUp> _pickupCurrent;
	GC_RigidBodyStatic* _target;  // ������� ����
	AIWEAPSETTINGS _weapSettings; // ��������� ������

	bool IsTargetVisible(GC_RigidBodyStatic *target, GC_RigidBodyStatic** ppObstacle = NULL);
	void LockTarget(GC_RigidBodyStatic *target);
	void FreeTarget();
	AIPRIORITY GetTargetRate(GC_Vehicle *target);

	bool FindTarget(/*out*/ AIITEMINFO &info);   // return true ���� ���� �������
	bool FindItem(/*out*/ AIITEMINFO &info);     // return true ���� ���-�� �������

	// �������� ������� ��� ��������� �������� ��������
	float _desired_offset;
	float _current_offset;

	// ������� ������
	ObjectType _otFavoriteWeapon;

protected:
	void RotateTo(VehicleState *pState, const vec2d &x, bool bForv, bool bBack);
	void TowerTo (VehicleState *pState, const vec2d &x, bool bFire);

	// ��������� ���������� ������ ���� ��� �������� �� ����������
	// target - ����
	// Vp      - �������� �������
	void CalcOutstrip(GC_Vehicle *target, float Vp, vec2d &fake);

	void ProcessAction();

	void SetL1(AIController::aiState_l1 new_state); // ����������� ��������� l1
	void SetL2(AIController::aiState_l2 new_state); // ����������� ��������� l2

	void SelectState();
	void DoState(VehicleState *pVehState);

public:
	AIController(GC_Player *pPlayer);
	virtual ~AIController();

	virtual void Reset(); // ����� ���������, ������������ ���� ������
	virtual void OnPlayerRespawn();
	virtual void OnPlayerDie();

protected:
	virtual void GetControl(VehicleState *pState, float dt);
};

///////////////////////////////////////////////////////////////////////////////
// end of file
