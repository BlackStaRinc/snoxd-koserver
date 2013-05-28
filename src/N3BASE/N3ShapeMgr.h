#pragma once

#include "My_3DStruct.h"
#include <cstdio>

const int CELL_MAIN_DEVIDE = 4; // ���μ��� 4 X 4 �� ���꼿�� ������..
const int CELL_SUB_SIZE = 4; // 4 Meter �� ���꼿�� �������̴�..
const int CELL_MAIN_SIZE = CELL_MAIN_DEVIDE * CELL_SUB_SIZE; // ���μ� ũ��� ���꼿���� X ���꼿 ũ���̴�.
const int MAX_CELL_MAIN = 4096 / CELL_MAIN_SIZE; // ���μ��� �ִ� ������ ����ũ�� / ���μ�ũ�� �̴�.
const int MAX_CELL_SUB = MAX_CELL_MAIN * CELL_MAIN_DEVIDE; // ���꼿 �ִ� ������ ���μ� * ���μ������� �̴�.

class CN3ShapeMgr
{
public:
	struct __CellSub // ���� �� ������
	{
		int 	nCCPolyCount; // Collision Check Polygon Count
		uint32*	pdwCCVertIndices; // Collision Check Polygon Vertex Indices - wCCPolyCount * 3 ��ŭ �����ȴ�.

		void Load(FILE *fp)
		{
			fread(&nCCPolyCount, sizeof(int), 1, fp);
			if(nCCPolyCount != 0)
			{
				if(pdwCCVertIndices) delete [] pdwCCVertIndices;
				pdwCCVertIndices = new uint32[nCCPolyCount * 3];
				// _ASSERT(pdwCCVertIndices);
				fread(pdwCCVertIndices, nCCPolyCount * 3 * 4, 1, fp);
			}
		}

		__CellSub() { memset(this, 0, sizeof(__CellSub)); }
		~__CellSub() { delete [] pdwCCVertIndices; }
	};

	struct __CellMain // �⺻ �� ������
	{
		int		nShapeCount; // Shape Count;
		WORD*	pwShapeIndices; // Shape Indices
		__CellSub SubCells[CELL_MAIN_DEVIDE][CELL_MAIN_DEVIDE];

		void Load(FILE *fp)
		{
			fread(&nShapeCount, sizeof(int), 1, fp);
			if (nShapeCount != 0)
			{
				if(pwShapeIndices) delete [] pwShapeIndices;
				pwShapeIndices = new WORD[nShapeCount];
				fread(pwShapeIndices, nShapeCount * 2, 1, fp);
			}
			for(int z = 0; z < CELL_MAIN_DEVIDE; z++)
			{
				for(int x = 0; x < CELL_MAIN_DEVIDE; x++)
					SubCells[x][z].Load(fp);
			}
		}

		__CellMain() { nShapeCount = 0; pwShapeIndices = nullptr; }
		~__CellMain() { delete [] pwShapeIndices; }
	};

	__Vector3* 				m_pvCollisions;

protected:
	float					m_fMapWidth;	// �� �ʺ�.. ���� ����
	float					m_fMapLength;	// �� ����.. ���� ����
	int						m_nCollisionFaceCount;
	__CellMain*				m_pCells[MAX_CELL_MAIN][MAX_CELL_MAIN];

public:
	void SubCell(const __Vector3& vPos, __CellSub** ppSubCell);
	__CellSub* SubCell(float fX, float fZ) // �ش� ��ġ�� �� �����͸� �����ش�.
	{
		int x = (int)(fX / CELL_MAIN_SIZE);
		int z = (int)(fZ / CELL_MAIN_SIZE);
		
		// _ASSERT(x >= 0 && x < MAX_CELL_MAIN && z >= 0 && z < MAX_CELL_MAIN);
		if(nullptr == m_pCells[x][z]) return nullptr;

		int xx = (((int)fX)%CELL_MAIN_SIZE)/CELL_SUB_SIZE;
		int zz = (((int)fZ)%CELL_MAIN_SIZE)/CELL_SUB_SIZE;
		
		return &(m_pCells[x][z]->SubCells[xx][zz]);
	}
	float		GetHeightNearstPos(const __Vector3& vPos, __Vector3* pvNormal = nullptr);  // ���� ����� ������ �����ش�. ������ -FLT_MAX �� �����ش�.
	float		GetHeight(float fX, float fZ, __Vector3* pvNormal = nullptr);  // ���� �������� ���� ���� ���� �����ش�. ������ -FLT_MAX �� �����ش�.
	int			SubCellPathThru(const __Vector3& vFrom, const __Vector3& vAt, __CellSub** ppSubCells); // ���� ���̿� ��ģ �������� �����ش�..
	float		Width() { return m_fMapWidth; } // ���� �ʺ�. ������ �����̴�.
	float		Height() { return m_fMapWidth; } // ���� �ʺ�. ������ �����̴�.

	bool		CheckCollision(	const __Vector3& vPos,			// �浹 ��ġ
								const __Vector3& vDir,			// ���� ����
								float fSpeedPerSec,				// �ʴ� �����̴� �ӵ�
								__Vector3* pvCol = nullptr,		// �浹 ����
								__Vector3* pvNormal = nullptr,		// �浹�Ѹ��� ��������
								__Vector3* pVec = nullptr);		// �浹�� �� �� ������ __Vector3[3]

	bool		Create(float fMapWidth, float fMapLength); // ���� �ʺ�� ���̸� ���� ������ �ִ´�..
	bool		LoadCollisionData(FILE *fp);

	void Release();
	CN3ShapeMgr();
	virtual ~CN3ShapeMgr();
};