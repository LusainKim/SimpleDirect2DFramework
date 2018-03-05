#include <windows.h>
#include <vssym32.h>
#include <uxtheme.h>
#include <string>

#pragma comment(lib, "Uxtheme")

#include "GetThemeData.h"

std::wstring GetCaptionFontName()
{
	
	LOGFONT logFont;
	GetThemeSysFont(NULL, TMT_CAPTIONFONT, &logFont);

	return std::wstring{ logFont.lfFaceName };
}

COLORREF GetCaptionColor()
{
	return GetThemeSysColor(NULL, GetSysColor(COLOR_ACTIVECAPTION));;
	COLORREF color;

	GetThemeColor(NULL, WP_CAPTION, CS_ACTIVE, TMT_COLOR, &color);

	return color;
}

COLORREF GetCaptionTextColor()
{
	return GetThemeSysColor(NULL, GetSysColor(COLOR_CAPTIONTEXT));;
}
