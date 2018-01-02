#include "stdafx.h"
#include "Input.h"

void CInputManager::ProcessInput(bool(&arrByte)[256])
{
	if (!m_bActive) return;

	bool retval = false;
	BOOL bKeyState = GetKeyboardState(m_pKeyBuffer);
	UNREFERENCED_PARAMETER(bKeyState);

	for (auto& data : m_mRegisterKeyList)
	{
		auto& key = data.first;
		auto& btn = data.second;
		
		if (arrByte[key]) continue;
		arrByte[key] = true;

		if (1 == GetHighBit(m_pKeyBuffer[key]))
		{
			retval = true;
			btn.togleKey = 1;
			btn.down_event();
		}
		else if (1 == btn.togleKey) 
		{
			retval = true;
			btn.togleKey = 0;
			btn.up_event();
		}
	}
}

void CInputManager::bind(UCHAR key, CInputManager::Button && btn)
{
	if (m_mRegisterKeyList.find(key) != end(m_mRegisterKeyList)) m_mRegisterKeyList.erase(key);
	m_mRegisterKeyList.insert(make_pair(key, move(btn)));
}

void CInputManager::bind(UCHAR basekey, CInputManager::Button&& btn, LPCSTR AppName, LPCSTR configPath)
{
	auto tag = btn.tag.c_str();
	return bind(GetPrivateProfileIntA(AppName, tag, basekey, configPath), move(btn));
}

bool CInputManager::replace(string tag, UCHAR newkey)
{
	auto p = find_if(begin(m_mRegisterKeyList), end(m_mRegisterKeyList), [&](const auto & p)
	{
		return p.second.tag == tag;
	});

	if (p == end(m_mRegisterKeyList))
		return false;

	// erase before key
	auto btn = move(m_mRegisterKeyList.at(p->first));
	m_mRegisterKeyList.erase(p->first);

	bind(newkey, move(btn));

	return true;
}

bool CInputManager::replace(std::string tag, UCHAR newkey, LPCSTR appName, LPCSTR filePath)
{
	auto retval = replace(tag, newkey);
	SaveKey(appName, tag, filePath);

	return retval;
}

void CInputManager::SaveAllKey(LPCSTR appName, LPCSTR filePath)
{
	for (auto& data : m_mRegisterKeyList)
	{
		auto& key = data.first;
		auto& btn = data.second;
		WritePrivateProfileStringA(appName, btn.tag.c_str(), to_string(key).c_str(), filePath);
	}
}

void CInputManager::SaveKey(LPCSTR appName, string tag, LPCSTR filePath)
{
	auto data = find_if(begin(m_mRegisterKeyList), end(m_mRegisterKeyList), [&](const auto & p)
	{
		return p.second.tag == tag;
	});

	if (data == end(m_mRegisterKeyList))
		return;

	auto& key = data->first;
	auto& btn = data->second;

	WritePrivateProfileStringA(appName, btn.tag.c_str(), to_string(key).c_str(), filePath);
}
