#pragma once

#include <string>

#ifdef UNICODE
using fvstring = std::wstring;
#define to_fvstring std::to_wstring
#else
using fvstring = std::string;
#define to_fvstring std::to_string
#endif



struct SFixValueFunc
{
	template<typename Ty>
	static fvstring _Call(Ty fixnum)
	{
		if (fixnum == 0) { return to_fvstring(0); }

		fvstring szFix;
		Ty count = 0;

		while (fixnum > 0)
		{
			szFix += to_fvstring(fixnum % 10);
			if ((fixnum /= 10) == 0) continue;
			if (++count % 4 != 3)    continue;
			szFix += ',';
			++count;
		}

		reverse(szFix.begin(), szFix.end());
		return szFix;
	}
};

template<typename Ty, bool isInteger = std::is_integral<Ty>::value>
struct SFixValue;

template<typename Ty>
struct SFixValue<Ty, true> : SFixValueFunc {};

template<typename Ty>
struct SFixValue<Ty, false>
{
	static fvstring _Call(Ty fixnum)
	{
		static_assert(false, "FixValue only use inteager type!");
		return fvstring();
	}
};

/// <summary>
/// 세자리 단위로 쉼표를 붙인 문자열 숫자를 반환합니다.
/// </summary>
///	<param name = "fixnum"> 변형을 가할 정수형 숫자입니다. 정수형 이외의 값은 컴파일 에러 처리됩니다. </param>
/// <returns> 프로젝트 문자 집합이 유니코드일 때 wstring, 멀티바이트일 때 string 값으로 반환됩니다. </returns>
template<typename Ty>
inline fvstring FixValue(Ty fixnum)
{
	return SFixValue<Ty>::_Call(fixnum);
}
