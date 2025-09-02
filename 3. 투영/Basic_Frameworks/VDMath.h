#pragma once
#include "DirectXMath.h"
#include <Windows.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define rad(x) (x * 3.14159265358979323846 / 180.0f)

template<typename T>
constexpr T pi = T(3.14159265358979323846);

class VDMATRIX; // ���� ����
class VDVIEWPORT; // ���� ����

class VDVECTOR
{
public:
	float x, y, z, w = 1.0f;

	VDVECTOR() : x(0.0f), y(0.0f), z(0.0f) {} // �⺻ ������
	VDVECTOR(float x, float y, float z) : x(x), y(y), z(z) {} // �Ű������� �޴� ������

	// �Ǽ� ������ �����ε�
	VDVECTOR operator* (float scalar) const { return VDVECTOR(x * scalar, y * scalar, z * scalar); }
	VDVECTOR operator/ (float scalar) const { return VDVECTOR(x / scalar, y / scalar, z / scalar); }

	// ���� ������ �����ε�
	VDVECTOR operator+ (const VDVECTOR& other) const { return VDVECTOR(x + other.x, y + other.y, z + other.z); }
	VDVECTOR& operator+= (const VDVECTOR& other) { x += other.x; y += other.y; z += other.z; return *this; } // ���� ����
	VDVECTOR operator- (const VDVECTOR& other) const { return VDVECTOR(x - other.x, y - other.y, z - other.z); }
	VDVECTOR& operator-= (const VDVECTOR& other) { x -= other.x; y -= other.y; z -= other.z; return *this; } // ���� ����

	// ����, ���� ������ �����ε�
	float operator* (const VDVECTOR& other) const { return x * other.x + y * other.y + z * other.z; } // ����
	VDVECTOR operator^ (const VDVECTOR& other) const { return VDVECTOR(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x); } // ����

	// ��Ÿ ������ �����ε�, �Լ�
	VDVECTOR operator= (const VDVECTOR& other) { x = other.x; y = other.y; z = other.z; return *this; } // ���� ����
	VDVECTOR operator- () const { return VDVECTOR(-x, -y, -z); } // ������ ��ȣ ���� // �ʿ��Ϸ���?
	bool operator== (const VDVECTOR& other) const { return x == other.x && y == other.y && z == other.z; } // ���� ��

	float Length() const { return sqrtf(x * x + y * y + z * z); } // ������ ����
	VDVECTOR Normalize() const { float len = Length(); if (len == 0.0f) return VDVECTOR(0.0f, 0.0f, 0.0f); return VDVECTOR(x / len, y / len, z / len); } // ����ȭ

	VDVECTOR Transform(const VDMATRIX& matrix) const; // ���� ��� ��ȯ
	VDVECTOR& VectorProjection(const VDVIEWPORT* viewport, const VDMATRIX* pProjection, const VDMATRIX* pView); // ���� ����
};

class VDMATRIX
{
	float m_matrix[4][4];
	bool m_isRowMajor = true;

	// VDVECTOR �� VDMATRIX ���� VDMATRIX�� ��Ŀ� ������ �� �ְ�
	friend class VDVECTOR;
	friend class VDMATRIX;

public:
	VDMATRIX() { for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) m_matrix[i][j] = (i == j) ? 1.0f : 0.0f; } // �⺻ ������, ���� ��ķ� �ʱ�ȭ
	VDMATRIX(bool isRowMajor, float matrix[4][4]); // �Ű������� �޴� ������, ��� �ʱ�ȭ

	// ��� ������ �����ε�
	VDMATRIX operator* (const VDMATRIX& other) const; // ��� ����
	VDMATRIX& operator*= (const VDMATRIX& other) { *this = *this * other; return *this; } // ��� ���� ����

	// ��� ��Ÿ �Լ�
	void Transposition(); // ��� ��ġ
	void Inverse(); // ��� ����� // �̰� ���� ������?

	// ��� ���� ���� �Լ�
	void VDMatrixLookAtLH(const VDVECTOR& eye, const VDVECTOR& target, const VDVECTOR& up); // LookAt LH ��� ����
	void VDMatrixPerspectiveFovLH(float fovY, float aspect, float nearZ, float farZ); // ���� ���� ��� ����
};

class VDCOLOR
{
public:
	float r, g, b, a;

	VDCOLOR() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {} // �⺻ ������
	VDCOLOR(float _r, float _g, float _b, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {} // �Ű������� �޴� ������
	VDCOLOR(DWORD color); // DWORD ���� ������ �ʱ�ȭ
	operator DWORD();

	// ���� ������ �����ε�
	VDCOLOR operator* (float scalar) const { return VDCOLOR(r * scalar, g * scalar, b * scalar, a * scalar); } // ���� ��Į�� ����
	VDCOLOR& operator*= (float scalar) { r *= scalar; g *= scalar; b *= scalar; a *= scalar; return *this; } // ���� ��Į�� ���� ����
	VDCOLOR operator* (const VDCOLOR& other) const { return VDCOLOR(r * other.r, g * other.g, b * other.b, a * other.a); } // ���� ����
	VDCOLOR& operator*= (const VDCOLOR& other) { r *= other.r; g *= other.g; b *= other.b; a *= other.a; return *this; } // ���� ���� ����
	VDCOLOR operator+ (const VDCOLOR& other) const { return VDCOLOR(r + other.r, g + other.g, b + other.b, a + other.a); } // ���� ����
	VDCOLOR& operator+= (const VDCOLOR& other) { r += other.r; g += other.g; b += other.b; a += other.a; return *this; } // ���� ���� ����
	VDCOLOR operator- (const VDCOLOR& other) const { return VDCOLOR(r - other.r, g - other.g, b - other.b, a - other.a); } // ���� ����
	VDCOLOR& operator-= (const VDCOLOR& other) { r -= other.r; g -= other.g; b -= other.b; a -= other.a; return *this; } // ���� ���� ����

	// ��Ÿ �Լ�
	static VDCOLOR Lerp(const VDCOLOR& start, const VDCOLOR& end, float t); // ���� ���� �Լ�
};