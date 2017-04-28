#pragma once

#include <codecvt>
#include <locale>
#include <string>

/// <summary>
/// std::string ��ü�� ���ڿ��� std::wstring ��ü�� ���ڿ��� ��ȯ�մϴ�.
/// </summary>
///	<param name = "str"> ��ȯ�� std::string ��ü�Դϴ�. </param>
///	<param name = "Loc"> ��ȯ�� ���������� locale ���Դϴ�. �ѱ��� locale("ko") �� ����մϴ�. </param>
inline std::wstring s_to_ws(const std::string& str, std::locale Loc = std::locale("ko"))
{
	using convert_typeX = std::codecvt_utf8_utf16<wchar_t>;
	return std::wstring_convert<convert_typeX, wchar_t>
	{ &std::use_facet<convert_typeX>(Loc) }.from_bytes(str);
}

/// <summary>
/// std::wstring ��ü�� ���ڿ��� std::string ��ü�� ���ڿ��� ��ȯ�մϴ�.
/// </summary>
///	<param name = "str"> ��ȯ�� std::wstring ��ü�Դϴ�. </param>
///	<param name = "Loc"> ��ȯ�� ���������� locale ���Դϴ�. �ѱ��� locale("ko") �� ����մϴ�. </param>
inline std::string ws_to_s(const std::wstring& wstr, std::locale Loc = std::locale("ko"))
{
	using conv_Ty = std::codecvt_utf8_utf16<wchar_t>;
	return std::wstring_convert<conv_Ty, wchar_t>
	{ &std::use_facet<conv_Ty>(Loc) }.to_bytes(wstr);
}
