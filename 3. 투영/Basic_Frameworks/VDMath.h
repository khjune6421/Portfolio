#pragma once
#include "DirectXMath.h"
#include <Windows.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define rad(x) (x * 3.14159265358979323846 / 180.0f)

template<typename T>
constexpr T pi = T(3.14159265358979323846);

class VDMATRIX; // 전방 선언
class VDVIEWPORT; // 전방 선언

class VDVECTOR
{
public:
	float x, y, z, w = 1.0f;

	VDVECTOR() : x(0.0f), y(0.0f), z(0.0f) {} // 기본 생성자
	VDVECTOR(float x, float y, float z) : x(x), y(y), z(z) {} // 매개변수를 받는 생성자

	// 실수 연산자 오버로딩
	VDVECTOR operator* (float scalar) const { return VDVECTOR(x * scalar, y * scalar, z * scalar); }
	VDVECTOR operator/ (float scalar) const { return VDVECTOR(x / scalar, y / scalar, z / scalar); }

	// 벡터 연산자 오버로딩
	VDVECTOR operator+ (const VDVECTOR& other) const { return VDVECTOR(x + other.x, y + other.y, z + other.z); }
	VDVECTOR& operator+= (const VDVECTOR& other) { x += other.x; y += other.y; z += other.z; return *this; } // 벡터 덧셈
	VDVECTOR operator- (const VDVECTOR& other) const { return VDVECTOR(x - other.x, y - other.y, z - other.z); }
	VDVECTOR& operator-= (const VDVECTOR& other) { x -= other.x; y -= other.y; z -= other.z; return *this; } // 벡터 뺄셈

	// 내적, 외적 연산자 오버로딩
	float operator* (const VDVECTOR& other) const { return x * other.x + y * other.y + z * other.z; } // 내적
	VDVECTOR operator^ (const VDVECTOR& other) const { return VDVECTOR(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x); } // 외적

	// 기타 연산자 오버로딩, 함수
	VDVECTOR operator= (const VDVECTOR& other) { x = other.x; y = other.y; z = other.z; return *this; } // 벡터 복사
	VDVECTOR operator- () const { return VDVECTOR(-x, -y, -z); } // 벡터의 부호 반전 // 필요하려나?
	bool operator== (const VDVECTOR& other) const { return x == other.x && y == other.y && z == other.z; } // 벡터 비교

	float Length() const { return sqrtf(x * x + y * y + z * z); } // 벡터의 길이
	VDVECTOR Normalize() const { float len = Length(); if (len == 0.0f) return VDVECTOR(0.0f, 0.0f, 0.0f); return VDVECTOR(x / len, y / len, z / len); } // 정규화

	VDVECTOR Transform(const VDMATRIX& matrix) const; // 벡터 행렬 변환
	VDVECTOR& VectorProjection(const VDVIEWPORT* viewport, const VDMATRIX* pProjection, const VDMATRIX* pView); // 벡터 투영
};

class VDMATRIX
{
	float m_matrix[4][4];
	bool m_isRowMajor = true;

	// VDVECTOR 와 VDMATRIX 만이 VDMATRIX의 행렬에 접근할 수 있게
	friend class VDVECTOR;
	friend class VDMATRIX;

public:
	VDMATRIX() { for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) m_matrix[i][j] = (i == j) ? 1.0f : 0.0f; } // 기본 생성자, 단위 행렬로 초기화
	VDMATRIX(bool isRowMajor, float matrix[4][4]); // 매개변수를 받는 생성자, 행렬 초기화

	// 행렬 연산자 오버로딩
	VDMATRIX operator* (const VDMATRIX& other) const; // 행렬 곱셈
	VDMATRIX& operator*= (const VDMATRIX& other) { *this = *this * other; return *this; } // 행렬 대입 곱셈

	// 행렬 기타 함수
	void Transposition(); // 행렬 전치
	void Inverse(); // 행렬 역행렬 // 이거 어케 만들지?

	// 행렬 벡터 연산 함수
	void VDMatrixLookAtLH(const VDVECTOR& eye, const VDVECTOR& target, const VDVECTOR& up); // LookAt LH 행렬 생성
	void VDMatrixPerspectiveFovLH(float fovY, float aspect, float nearZ, float farZ); // 원근 투영 행렬 생성
};

class VDCOLOR
{
public:
	float r, g, b, a;

	VDCOLOR() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {} // 기본 생성자
	VDCOLOR(float _r, float _g, float _b, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {} // 매개변수를 받는 생성자
	VDCOLOR(DWORD color); // DWORD 색상 값으로 초기화
	operator DWORD();

	// 색상 연산자 오버로딩
	VDCOLOR operator* (float scalar) const { return VDCOLOR(r * scalar, g * scalar, b * scalar, a * scalar); } // 색상 스칼라 곱셈
	VDCOLOR& operator*= (float scalar) { r *= scalar; g *= scalar; b *= scalar; a *= scalar; return *this; } // 색상 스칼라 곱셈 대입
	VDCOLOR operator* (const VDCOLOR& other) const { return VDCOLOR(r * other.r, g * other.g, b * other.b, a * other.a); } // 색상 곱셈
	VDCOLOR& operator*= (const VDCOLOR& other) { r *= other.r; g *= other.g; b *= other.b; a *= other.a; return *this; } // 색상 곱셈 대입
	VDCOLOR operator+ (const VDCOLOR& other) const { return VDCOLOR(r + other.r, g + other.g, b + other.b, a + other.a); } // 색상 덧셈
	VDCOLOR& operator+= (const VDCOLOR& other) { r += other.r; g += other.g; b += other.b; a += other.a; return *this; } // 색상 덧셈 대입
	VDCOLOR operator- (const VDCOLOR& other) const { return VDCOLOR(r - other.r, g - other.g, b - other.b, a - other.a); } // 색상 뺄셈
	VDCOLOR& operator-= (const VDCOLOR& other) { r -= other.r; g -= other.g; b -= other.b; a -= other.a; return *this; } // 색상 뺄셈 대입

	// 기타 함수
	static VDCOLOR Lerp(const VDCOLOR& start, const VDCOLOR& end, float t); // 선형 보간 함수
};