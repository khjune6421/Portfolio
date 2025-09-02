#include <iostream>
#include <algorithm>
#include "VDMath.h"
#include "VDX9types.h"

VDMATRIX::VDMATRIX(bool isRowMajor, float matrix[4][4])
{
	m_isRowMajor = isRowMajor;
	if (m_isRowMajor)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				m_matrix[i][j] = matrix[i][j];
			}
		}
	}
	else
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				m_matrix[j][i] = matrix[i][j];
			}
		}
	}
}

VDMATRIX VDMATRIX::operator*(const VDMATRIX& other) const
{
	VDMATRIX result;
	if (!other.m_isRowMajor)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				result.m_matrix[i][j] = 0.0f;
				for (int k = 0; k < 4; ++k)
				{
					result.m_matrix[i][j] += m_matrix[i][k] * other.m_matrix[j][k];
				}
			}
		}
	}
	return result;
}

void VDMATRIX::Transposition()
{
	if (m_isRowMajor)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = i + 1; j < 4; ++j)
			{
				std::swap(m_matrix[i][j], m_matrix[j][i]);
			}
		}
	}
	else
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = i + 1; j < 4; ++j)
			{
				std::swap(m_matrix[j][i], m_matrix[i][j]);
			}
		}
	}
	m_isRowMajor = !m_isRowMajor;
}

void VDMATRIX::Inverse()
{
}

void VDMATRIX::VDMatrixLookAtLH(const VDVECTOR& eye, const VDVECTOR& target, const VDVECTOR& up) // 사실 뭐가 어떻게 되는지 잘 모르겠음
{
	m_isRowMajor = true;

	VDVECTOR zAxis = (target - eye).Normalize();
	VDVECTOR xAxis = (up ^ zAxis).Normalize();
	VDVECTOR yAxis = zAxis ^ xAxis;

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			m_matrix[i][j] = 0.0f;

	if (m_isRowMajor)
	{
		m_matrix[0][0] = xAxis.x;
		m_matrix[0][1] = yAxis.x;
		m_matrix[0][2] = zAxis.x;
		m_matrix[1][0] = xAxis.y;
		m_matrix[1][1] = yAxis.y;
		m_matrix[1][2] = zAxis.y;
		m_matrix[2][0] = xAxis.z;
		m_matrix[2][1] = yAxis.z;
		m_matrix[2][2] = zAxis.z;
		m_matrix[3][0] = -xAxis * eye;
		m_matrix[3][1] = -yAxis * eye;
		m_matrix[3][2] = -zAxis * eye;
		m_matrix[3][3] = 1.0f;
	}
	else
	{
		m_matrix[0][0] = xAxis.x;
		m_matrix[1][0] = xAxis.y;
		m_matrix[2][0] = xAxis.z;
		m_matrix[3][0] = -xAxis * eye;
		m_matrix[0][1] = yAxis.x;
		m_matrix[1][1] = yAxis.y;
		m_matrix[2][1] = yAxis.z;
		m_matrix[3][1] = -yAxis * eye;
		m_matrix[0][2] = zAxis.x;
		m_matrix[1][2] = zAxis.y;
		m_matrix[2][2] = zAxis.z;
		m_matrix[3][2] = -zAxis * eye;
		m_matrix[0][3] = 0.0f;
		m_matrix[1][3] = 0.0f;
		m_matrix[2][3] = 0.0f;
		m_matrix[3][3] = 1.0f;
	}
}

void VDMATRIX::VDMatrixPerspectiveFovLH(float fovY, float aspect, float nearZ, float farZ) // 예도 마찬가지
{
	m_isRowMajor = true;

	float yScale = 1.0f / tanf(fovY * 0.5f);
	float xScale = yScale / aspect;

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			m_matrix[i][j] = 0.0f;

	m_matrix[0][0] = xScale;
	m_matrix[1][1] = yScale;
	m_matrix[2][2] = farZ / (farZ - nearZ);
	m_matrix[2][3] = 1.0f;
	m_matrix[3][2] = -nearZ * farZ / (farZ - nearZ);
	m_matrix[3][3] = 0.0f;
}

VDCOLOR::VDCOLOR(DWORD color)
{
	r = static_cast<float>((color & 0xFF0000) >> 16) / 255.0f; // R 채널 추출
	g = static_cast<float>((color & 0x00FF00) >> 8) / 255.0f; // G 채널 추출
	b = static_cast<float>(color & 0x0000FF) / 255.0f; // B 채널 추출
	a = static_cast<float>((color & 0xFF000000) >> 24) / 255.0f; // A 채널 추출
}

VDCOLOR::operator DWORD()
{
	COLORREF color = RGB(static_cast<int>(r * 255.0f), static_cast<int>(g * 255.0f), static_cast<int>(b * 255.0f));

	return color;
}

VDCOLOR VDCOLOR::Lerp(const VDCOLOR& start, const VDCOLOR& end, float t)
{
	return VDCOLOR
	(
		start.r + (end.r - start.r) * t,
		start.g + (end.g - start.g) * t,
		start.b + (end.b - start.b) * t,
		start.a + (end.a - start.a) * t
	);
}

VDVECTOR VDVECTOR::Transform(const VDMATRIX& matrix) const
{
	VDVECTOR result;
	float w;

	if (matrix.m_isRowMajor)
	{
		result.x = this->x * matrix.m_matrix[0][0] + this->y * matrix.m_matrix[1][0] + this->z * matrix.m_matrix[2][0] + matrix.m_matrix[3][0];
		result.y = this->x * matrix.m_matrix[0][1] + this->y * matrix.m_matrix[1][1] + this->z * matrix.m_matrix[2][1] + matrix.m_matrix[3][1];
		result.z = this->x * matrix.m_matrix[0][2] + this->y * matrix.m_matrix[1][2] + this->z * matrix.m_matrix[2][2] + matrix.m_matrix[3][2];
		w = this->x * matrix.m_matrix[0][3] + this->y * matrix.m_matrix[1][3] + this->z * matrix.m_matrix[2][3] + matrix.m_matrix[3][3];
	}
	else
	{
		result.x = this->x * matrix.m_matrix[0][0] + this->y * matrix.m_matrix[0][1] + this->z * matrix.m_matrix[0][2] + matrix.m_matrix[0][3];
		result.y = this->x * matrix.m_matrix[1][0] + this->y * matrix.m_matrix[1][1] + this->z * matrix.m_matrix[1][2] + matrix.m_matrix[1][3];
		result.z = this->x * matrix.m_matrix[2][0] + this->y * matrix.m_matrix[2][1] + this->z * matrix.m_matrix[2][2] + matrix.m_matrix[2][3];
		w = this->x * matrix.m_matrix[3][0] + this->y * matrix.m_matrix[3][1] + this->z * matrix.m_matrix[3][2] + matrix.m_matrix[3][3];
	}

	if (w != 0.0f)
	{
		result.x /= w;
		result.y /= w;
		result.z /= w;
	}

	return result;
}

VDVECTOR& VDVECTOR::VectorProjection(const VDVIEWPORT* viewport, const VDMATRIX* pProjection, const VDMATRIX* pView)
{
	if (!viewport || !pProjection || !pView) return *this; // nullptr 체크

	VDVECTOR transformed = this->Transform(*pView).Transform(*pProjection); // 월드, 뷰, 프로젝션 변환
	transformed.x = transformed.x * (viewport->Width * 0.5f) + (viewport->X + viewport->Width * 0.5f);
	transformed.y = -transformed.y * (viewport->Height * 0.5f) + (viewport->Y + viewport->Height * 0.5f);

	this->x = transformed.x;
	this->y = transformed.y;

	return *this;
}
