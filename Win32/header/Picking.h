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
/// PickRay 위치와 PickRay 방향을 생성하는 함수입니다.
/// </summary>
///	<param name = "xmvPickPosition"> 선택한 위치입니다. </param>
///	<param name = "pxmmtxModel"> 
/// <para>객체의 모델 행렬입니다. </para>
/// <para>nullptr이 들어오면 전역 좌표에서의 PickRay를 구합니다. </para>
/// </param>
///	<param name = "xmmtxView"> 현재 카메라의 뷰 행렬입니다. </param>
///	<param name = "xmvPickRayPosition"> 반환값입니다. 이 값은 위치를 반환합니다. </param>
///	<param name = "xmvPickRayDirection"> 반환값입니다. 이 값은 방향을 반환합니다. </param>
/// <remarks> </remarks>
/// <returns> 이 함수는 생성된 xmvPickRayPosition 과 xmvPickRayDirection 을 반환합니다. </returns>
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
/// 3D 위치에 있는 점이 최종적으로 화면의 어느 좌표에 그려질 지 계산합니다.
/// </summary>
///	<param name = "xmvPosition"> 선택한 위치입니다. </param>
///	<param name = "xmmtxView"> 현재 카메라의 뷰 행렬입니다 </param>
///	<param name = "xmmtxProjection"> 현재 카메라의 투영 행렬입니다. </param>
///	<param name = "d3dViewport"> 3d viewport입니다. 클라이언트 크기와 시작좌표(left, top)이 필요합니다. </param>
/// <remarks> </remarks>
/// <returns> 이 함수는 생성된 2D 좌표를 반환합니다. </returns>
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
