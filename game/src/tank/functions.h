// functions.h

#pragma once

//-------------------------------------------------------

void CalcOutstrip(const vec2d &fp, // fire point
				  float vp,        // speed of projectile
				  const vec2d &tx, // target position
				  const vec2d &tv, // target velocity
				  vec2d &fake);    // out: coordinates of fake target


//-------------------------------------------------------

// ������������� ���������� � �������
// power �� ������ ���� �������������
int intPower(int base, unsigned int power);


// �������� ��������������� �� �����������.
// ������� �� ��������� �� �����������.
// �������������� ������ ���� �����������
//     (�. �. left < right, top < bottom)
BOOL IsIntersect(const LPFRECT lprtRect1, const LPFRECT lprtRect2);


bool PtInFRect(const FRECT &rect, const vec2d &pt);


void RectToFRect(LPFRECT lpfrt, const LPRECT lprt);
void FRectToRect(LPRECT  lprt,  const LPFRECT lpfrt);
void OffsetFRect(LPFRECT lpfrt, float x, float y);
void OffsetFRect(LPFRECT lpfrt, const vec2d &x);


int net_rand();

// ��������� ���������� ����� �� 0 �� max
float frand(float max);
float net_frand(float max);

// ��������� �������� ������������ ������� ������ len
vec2d vrand(float len);
vec2d net_vrand(float len);


// �������� ����� �� ������������
BOOL CheckFile_ie(LPCTSTR fileName);
BOOL CheckFile_ls(LPCTSTR fileName);


DWORD CalcCRC32(LPCTSTR fileName);


// ���������� ����� ������� ����������
BOOL SafeSetCurDir(LPCTSTR lpstrName, HWND hDlg);

///////////////////////////////////////////////////////////////////////////////
// end of file
