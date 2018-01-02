#pragma once

struct MESHINTERSECTINFO {
	DWORD	m_dwFaceIndex;
	float	m_fU;
	float	m_fV;
	float	m_fDistance;
};

inline bool RayIntersectTriangle(	  XMVECTOR xmvOrigin
									, XMVECTOR xmvDirection
									, XMVECTOR xmvP0
									, XMVECTOR xmvP1
									, XMVECTOR xmvP2
									, float *pfU
									, float *pfV
									, float *pfRayToTriangle
)
{
	XMVECTOR xmvEdge1 = xmvP1 - xmvP0;
	XMVECTOR xmvEdge2 = xmvP2 - xmvP0;
	XMVECTOR xmvP, xmvQ;
	
	xmvP = XMVector3Cross(xmvDirection, xmvEdge2);
	
	XMVECTOR xmvA = XMVector3Dot(xmvEdge1, xmvP);

	if (XMVector3Equal(xmvA, XMVectorZero())) return(false);

	float f = 1.0f / XMVectorGetX(xmvA);

	XMVECTOR d3dxvP0ToOrigin = xmvOrigin - xmvP0;
	*pfU = f * XMVectorGetX(XMVector3Dot(d3dxvP0ToOrigin, xmvP));
	if ((*pfU < 0.0f) || (*pfU > 1.0f)) return(false);
	xmvQ = XMVector3Cross(d3dxvP0ToOrigin, xmvEdge1);

	*pfV = f * XMVectorGetX(XMVector3Dot(xmvDirection, xmvQ));
	if ((*pfV < 0.0f) || ((*pfU + *pfV) > 1.0f)) return(false);
	*pfRayToTriangle = f * XMVectorGetX(XMVector3Dot(xmvEdge2, xmvQ));
	
	return(*pfRayToTriangle >= 0.0f);
}

/// <summary>
/// PickRay ��ġ�� PickRay ������ �����ϴ� �Լ��Դϴ�.
/// </summary>
///	<param name = "xmvPickPosition"> ������ ��ġ�Դϴ�. </param>
///	<param name = "pxmmtxModel"> 
/// <para>��ü�� �� ����Դϴ�. </para>
/// <para>nullptr�� ������ ���� ��ǥ������ PickRay�� ���մϴ�. </para>
/// </param>
///	<param name = "xmmtxView"> ���� ī�޶��� �� ����Դϴ�. </param>
///	<param name = "xmvPickRayPosition"> ��ȯ���Դϴ�. �� ���� ��ġ�� ��ȯ�մϴ�. </param>
///	<param name = "xmvPickRayDirection"> ��ȯ���Դϴ�. �� ���� ������ ��ȯ�մϴ�. </param>
/// <remarks> </remarks>
/// <returns> �� �Լ��� ������ xmvPickRayPosition �� xmvPickRayDirection �� ��ȯ�մϴ�. </returns>
inline void GenerateRayForPicking(	  FXMVECTOR xmvPickPosition
									, FXMMATRIX *pxmmtxModel
									, FXMMATRIX *xmmtxView
									, XMVECTOR& xmvPickRayPosition
									, XMVECTOR& xmvPickRayDirection) 
{
	XMMATRIX xmmtxModelView = *xmmtxView;
	if (pxmmtxModel) xmmtxModelView = XMMatrixMultiply(*pxmmtxModel, *xmmtxView);
	XMMATRIX xmmtxInverse = XMMatrixInverse(nullptr, xmmtxModelView);

	XMVECTOR xmvCameraOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	xmvPickRayPosition	= XMVector3TransformCoord(xmvCameraOrigin, xmmtxInverse);
	xmvPickRayDirection	= XMVector3TransformCoord(xmvPickPosition, xmmtxInverse);
	xmvPickRayDirection = XMVector3Normalize(xmvPickRayDirection - xmvPickRayPosition);
}

#if USING_D3D11_INDRES

/// <summary>
/// 3D ��ġ�� �ִ� ���� ���������� ȭ���� ��� ��ǥ�� �׷��� �� ����մϴ�.
/// </summary>
///	<param name = "xmvPosition"> ������ ��ġ�Դϴ�. </param>
///	<param name = "xmmtxView"> ���� ī�޶��� �� ����Դϴ� </param>
///	<param name = "xmmtxProjection"> ���� ī�޶��� ���� ����Դϴ�. </param>
///	<param name = "d3dViewport"> 3d viewport�Դϴ�. Ŭ���̾�Ʈ ũ��� ������ǥ(left, top)�� �ʿ��մϴ�. </param>
/// <remarks> </remarks>
/// <returns> �� �Լ��� ������ 2D ��ǥ�� ��ȯ�մϴ�. </returns>
inline D2D_POINT_2F Get2DPointBy3DPosition(	  FXMVECTOR xmvPosition
											, FXMMATRIX xmmtxView
											, FXMMATRIX xmmtxProjection
											, D3D11_VIEWPORT d3dViewport) 
{
	XMMATRIX xmmtxPosition = XMMatrixTranslationFromVector(xmvPosition);
	xmmtxPosition = XMMatrixMultiply(xmmtxPosition, xmmtxView * xmmtxProjection);

	XMVECTOR xmvpos = xmmtxPosition.r[3];
	xmvpos /= XMVectorGetW(xmvpos);
	
	XMFLOAT3 xmfPos;
	XMStoreFloat3(&xmfPos, xmvpos);

	return 
	{
		  (+xmfPos.x + 1.f) * d3dViewport.Width  * 0.5f + d3dViewport.TopLeftX
		, (-xmfPos.y + 1.f) * d3dViewport.Height * 0.5f + d3dViewport.TopLeftY
	};
}

#endif
