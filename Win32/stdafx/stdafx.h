// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once
#pragma warning(disable:4100)

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#define NOMINMAX

// 파일 경로 관련 헤더
#include <shlwapi.h>
#pragma comment(lib, "shlwapi")

// Windows 헤더 파일:
#include <windowsx.h>


// C++11
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>
#include <list>
#include <deque>
#include <random>
#include <map>
#include <set>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>

#include "Framework/IndRes/DirectXVersion.h"

// DirectX Math
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

// U W P
#include <wrl.h>
#include <wrl/client.h>
#include <wrl/internal.h>

// Texture Loader : in Utils

using namespace std;
using namespace std::chrono;
using namespace D2D1;
using namespace DirectX;
using namespace DirectX::PackedVector;


using Microsoft::WRL::ComPtr;

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include "header/D2D_Operator.h"
#include "header/LoadD2DBitmap.h"
#include "header/FixValue.h"
#include "header/LusWrite.h"
#include "header/MeasureFunctionTimeElapsed.h"
#include "header/scroll.h"
#include "header/IO.h"
#include "header/stringhash.h"
#include "header/Vector2D.h"
#include "header/Picking.h"
#include "header/CreateHwndRenderTarget.h"
#include "header/WS_S_Conv.h"

using namespace D2DBitmap;



// 가변 배열
template <typename Ty> using TyArr = Ty[];

// TODO: 프로그램 전반에 필요한 함수는 여기에서 참조합니다.

template<typename Enum>	// Enum class의 선언형을 알려주어 인자와 대응하는 값을 반환하는 함수입니다.
inline constexpr auto GetEnumValueByType(Enum enumerator) noexcept	// enum class E : int { a,b,c }; 일 때,
{																	// auto data = GetEnumValueByType(E::a);
	return static_cast<std::underlying_type_t<Enum>>(enumerator);	// data의 형식은 int 이고, 값은 0 입니다.
}

/// <summary>
/// 현재 윈도우 프로시저와 연결된 클래스 인스턴스를 가져옵니다.
/// </summary>
///	<param name = "hWnd"> 윈도우 핸들입니다. 해당 윈도우 핸들과 연결된 클래스 인스턴스를 반환합니다. </param>
/// <returns> 연결된 클래스 인스턴스 포인터를 반환합니다. </returns>
template <typename Frmwk>
inline Frmwk GetUserDataPtr(HWND hWnd) noexcept
{
	return reinterpret_cast<Frmwk>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}

/// <summary>
/// 안전하게 COM 객체를 1회 해제합니다. 해제한 뒤 객체를 가지고 있던 포인터를 nullptr로 초기화합니다.
/// </summary>
///	<param name = "hWnd"> 윈도우 핸들입니다. 이 핸들 값으로 데이터를 찾을 수 있게 됩니다. </param>
///	<param name = "ptr"> 윈도우 핸들과 연결될 데이터입니다. 대체로 클래스 인스턴스의 포인터를 사용합니다. </param>
/// <returns> LONG_PTR 형식으로 결과값을 반환합니다. </returns>
inline auto SetUserDataPtr(HWND hWnd, LPVOID ptr) noexcept
{
	return ::SetWindowLongPtr(
		hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));
}

/// <summary>
/// 안전하게 COM 객체를 1회 해제합니다. 해제한 뒤 객체를 가지고 있던 포인터를 nullptr로 초기화합니다.
/// </summary>
///	<param name = "obj"> 해제 후 초기화할 COM 객체입니다. 오로지 COM 객체여야만 합니다. </param>
/// <remarks> obj는 반드지 COM 객체여야만 합니다. </remarks>
/// <returns> 반환값은 존재하지 않습니다. </returns>
template<typename Ty>
inline void SafeRelease(Ty& obj) { if (obj != nullptr) { obj->Release(); obj = nullptr; } }

/// <summary>
/// HRESULT가 S_FALSE 일 경우 입력된 메시지를 입력한 메시지 박스를 출력하며 true를 반환합니다.
/// </summary>
///	<param name = "hr"> 평가할 HRSULT 객체입니다. S_FALSE이면 메시지 박스를 출력합니다. </param>
///	<param name = "msg"> 메시지입니다. 생략 가능합니다. </param>
///	<param name = "captionMsg"> 캡션 문장입니다. 생략 가능합니다. </param>
/// <returns> HRESULT 값이 S_FALSE일 경우 true를 반환합니다. </returns>
inline bool FailureMessage(HRESULT hr, LPCTSTR msg = TEXT(""), LPCTSTR captionMsg = TEXT("Error!"))
{
	if (FAILED(hr))
	{
		MessageBox(NULL, msg, captionMsg, MB_OK);
		return true;
	}
	return false;
}

/// <summary>
/// 클라이언트에서 실제 그려지는 길이를 제외한 시스템 사용 가로 길이입니다.
/// </summary>
int GetMarginWidth();

/// <summary>
/// 클라이언트에서 실제 그려지는 길이를 제외한 시스템 사용 세로 길이입니다.
/// </summary>
int GetMarginHeight();

/// <summary>
/// 클라이언트에서 실제 그려지는 길이를 제외한 시스템 사용 세로 길이입니다.
/// </summary>
HINSTANCE GetHInstance();

/// <summary>
/// 인자로 받은 bool 변수의 값을 변경하되, 현재 값을 반환합니다.
/// 예를 들어, 현재 true인 변수 b를 이 함수에 넣으면 true를 반환하고 b의 값은 false가 됩니다.
/// </summary>
///	<param name = "b"> bool 변수입니다. 현재 값을 반환합니다. 값이 반전됩니다. </param>
inline bool Switch(bool& b) noexcept
{
	bool retval = b;
	b = !b;
	return retval;
}

/// <summary>
/// 인자로 받은 bool 변수의 값이 true일 때만 값을 변경합니다. 현재의 값을 반환합니다.
/// 예를 들어, 현재 true인 변수 b를 이 함수에 넣으면 true를 반환하고 b의 값은 false가 됩니다.
/// </summary>
///	<param name = "b"> bool 변수입니다. 현재 값을 반환합니다. 현재 값이 true일 경우, 값이 반전됩니다. </param>
inline bool SwitchIfTrue(bool& b) noexcept
{
	bool retval = b;
	if(b) b = !b;
	return retval;
}

/// <summary>
/// 인자로 받은 bool 변수의 값이 true일 때만 값을 변경합니다. 현재의 값을 반환합니다.
/// 예를 들어, 현재 true인 변수 b를 이 함수에 넣으면 true를 반환하고 b의 값은 false가 됩니다.
/// </summary>
///	<param name = "b"> bool 변수입니다. 현재 값을 반환합니다. 현재 값이 true일 경우, 값이 반전됩니다. </param>
template<typename Ty>
constexpr inline int upper(Ty val)
{
	static_assert(_Is_RealType<Ty>::value, "'Upper' is must be real type number.");
	return (val > 0.f) ? (val > int(val)) ? int(val) + 1 : int(val) : (val < int(val)) ? int(val) - 1 : int(val);
}

/// <summary>
/// 인자로 받은 두 값을 t 값만큼 선형 보간합니다.
/// </summary>
///	<param name = "start"> 시작 값입니다. </param>
///	<param name = "end"> 끝 값입니다. </param>
///	<param name = "t"> 보간 값입니다. 대체로 0부터 1 사이의 숫자입니다. </param>
template<typename Ty>
constexpr inline Ty Interpolation(Ty start, Ty end, float t)
{
	return static_cast<Ty>(start * (1.f - t) + end * t);
}

/// <summary>
/// 인자로 받은 두 값을 t 값만큼 선형 보간합니다.
/// </summary>
///	<param name = "start"> 시작 값입니다. </param>
///	<param name = "end"> 끝 값입니다. </param>
///	<param name = "t"> 보간 값입니다. 대체로 0부터 1 사이의 숫자입니다. </param>
inline D2D1::ColorF Interpolation(D2D1::ColorF start, D2D1::ColorF end, float t)
{
	return D2D1::ColorF
	{
		  start.r * (1.f - t) + end.r * t
		, start.g * (1.f - t) + end.g * t
		, start.b * (1.f - t) + end.b * t
		, start.a * (1.f - t) + end.a * t
	};
}

// TODO: 프로그램에 필요한 전처리기는 여기에서 참조합니다.

#include "SystemDefine.h"
