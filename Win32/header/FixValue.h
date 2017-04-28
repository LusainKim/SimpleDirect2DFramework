#pragma once

#include <string>

#ifdef UNICODE
using FixValueString = std::wstring;
#define to_FixValueString std::to_wstring
#else
using FixValueString = std::string;
#define to_FixValueString std::to_string
#endif



struct SFixValueFunc
{
	template<typename Ty>
	static FixValueString _Call(Ty fixnum)
	{
		if (fixnum == 0) { return to_FixValueString(0); }

		FixValueString szFix;
		Ty count = 0;

		while (fixnum > 0)
		{
			szFix += to_FixValueString(fixnum % 10);
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
	static FixValueString _Call(Ty fixnum)
	{
		static_assert(false, "FixValue only use inteager type!");
		return FixValueString();
	}
};

/// <summary>
/// ���ڸ� ������ ��ǥ�� ���� ���ڿ� ���ڸ� ��ȯ�մϴ�.
/// </summary>
///	<param name = "fixnum"> ������ ���� ������ �����Դϴ�. ������ �̿��� ���� ������ ���� ó���˴ϴ�. </param>
/// <returns> ������Ʈ ���� ������ �����ڵ��� �� wstring, ��Ƽ����Ʈ�� �� string ������ ��ȯ�˴ϴ�. </returns>
template<typename Ty>
inline FixValueString FixValue(Ty fixnum)
{
	return SFixValue<Ty>::_Call(fixnum);
}
