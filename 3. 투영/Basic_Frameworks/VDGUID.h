#pragma once
#include <vcruntime_string.h>
#include <Windows.h>

// VDGUID 정의
struct VDGUID // 00000000-0000-0000-0000-000000000000
{
	unsigned long Data1; // 32비트 정수
	unsigned short Data2; // 16비트 정수
	unsigned short Data3; // 16비트 정수
	unsigned char Data4[8]; // 8바이트 배열
	VDGUID() : Data1(0), Data2(0), Data3(0) // 기본 생성자
	{
		for (int i = 0; i < 8; ++i) // Data4 배열을 0으로 초기화
			Data4[i] = 0;
	}
	VDGUID(unsigned long d1, unsigned short d2, unsigned short d3, const unsigned char* d4) // 매개변수를 받는 생성자
		: Data1(d1), Data2(d2), Data3(d3)
	{
		for (int i = 0; i < 8; ++i) // Data4 배열을 초기화
			Data4[i] = d4[i];
	}
};

// 인터페이스 식별자
struct VDIID : public VDGUID
{
	VDIID() : VDGUID() {} // 기본 생성자
	VDIID(unsigned long d1, unsigned short d2, unsigned short d3, const unsigned char* d4) // 매개변수를 받는 생성자
		: VDGUID(d1, d2, d3, d4) {} // VDGUID 생성자를 호출
};
// 클래스 식별자
struct VDCLSID : public VDGUID
{
	VDCLSID() : VDGUID() {} // 기본 생성자
	VDCLSID(unsigned long d1, unsigned short d2, unsigned short d3, const unsigned char* d4) // 매개변수를 받는 생성자
		: VDGUID(d1, d2, d3, d4) {} // VDGUID 생성자를 호출
};

// == 연산자
inline bool operator==(const VDGUID& lhs, const VDGUID& rhs)
{
	return lhs.Data1 == rhs.Data1 && lhs.Data2 == rhs.Data2 && lhs.Data3 == rhs.Data3 &&
		memcmp(lhs.Data4, rhs.Data4, sizeof(lhs.Data4)) == 0; // memcmp를 사용하여 Data4 배열을 비교
}

// VooDoo 인터페이스 식별자 정의 // 기본 생성자
const VDIID VDIID_VDIUnknown;
const VDIID VDIID_IVooDoo;
const VDIID VDIID_IVooDooDevice;
const VDIID VDIID_IVooDooVertexBuffer;

// VooDoo 클래스 식별자 정의 // 기본 생성자
const VDCLSID VDCLSID_VooDoo;
const VDCLSID VDCLSID_VooDooDevice;
const VDCLSID VDCLSID_VooDooVertexBuffer;

inline BOOL VDIsEqualVDIID(const VDIID& VDIID1, const VDIID& VDIID2)
{
	return !memcmp(&VDIID1, &VDIID2, sizeof(VDIID)); // 두 VDIID를 비교하여 같으면 TRUE 반환
}