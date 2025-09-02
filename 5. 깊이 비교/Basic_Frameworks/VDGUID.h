#pragma once
#include <vcruntime_string.h>
#include <Windows.h>

// VDGUID ����
struct VDGUID // 00000000-0000-0000-0000-000000000000
{
	unsigned long Data1; // 32��Ʈ ����
	unsigned short Data2; // 16��Ʈ ����
	unsigned short Data3; // 16��Ʈ ����
	unsigned char Data4[8]; // 8����Ʈ �迭
	VDGUID() : Data1(0), Data2(0), Data3(0) // �⺻ ������
	{
		for (int i = 0; i < 8; ++i) // Data4 �迭�� 0���� �ʱ�ȭ
			Data4[i] = 0;
	}
	VDGUID(unsigned long d1, unsigned short d2, unsigned short d3, const unsigned char* d4) // �Ű������� �޴� ������
		: Data1(d1), Data2(d2), Data3(d3)
	{
		for (int i = 0; i < 8; ++i) // Data4 �迭�� �ʱ�ȭ
			Data4[i] = d4[i];
	}
};

// �������̽� �ĺ���
struct VDIID : public VDGUID
{
	VDIID() : VDGUID() {} // �⺻ ������
	VDIID(unsigned long d1, unsigned short d2, unsigned short d3, const unsigned char* d4) // �Ű������� �޴� ������
		: VDGUID(d1, d2, d3, d4) {} // VDGUID �����ڸ� ȣ��
};
// Ŭ���� �ĺ���
struct VDCLSID : public VDGUID
{
	VDCLSID() : VDGUID() {} // �⺻ ������
	VDCLSID(unsigned long d1, unsigned short d2, unsigned short d3, const unsigned char* d4) // �Ű������� �޴� ������
		: VDGUID(d1, d2, d3, d4) {} // VDGUID �����ڸ� ȣ��
};

// == ������
inline bool operator==(const VDGUID& lhs, const VDGUID& rhs)
{
	return lhs.Data1 == rhs.Data1 && lhs.Data2 == rhs.Data2 && lhs.Data3 == rhs.Data3 &&
		memcmp(lhs.Data4, rhs.Data4, sizeof(lhs.Data4)) == 0; // memcmp�� ����Ͽ� Data4 �迭�� ��
}

// VooDoo �������̽� �ĺ��� ���� // �⺻ ������
const VDIID VDIID_VDIUnknown;
const VDIID VDIID_IVooDoo;
const VDIID VDIID_IVooDooDevice;
const VDIID VDIID_IVooDooVertexBuffer;

// VooDoo Ŭ���� �ĺ��� ���� // �⺻ ������
const VDCLSID VDCLSID_VooDoo;
const VDCLSID VDCLSID_VooDooDevice;
const VDCLSID VDCLSID_VooDooVertexBuffer;

inline BOOL VDIsEqualVDIID(const VDIID& VDIID1, const VDIID& VDIID2)
{
	return !memcmp(&VDIID1, &VDIID2, sizeof(VDIID)); // �� VDIID�� ���Ͽ� ������ TRUE ��ȯ
}