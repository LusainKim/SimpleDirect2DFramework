#pragma once

#include <functional>
#include <memory>
#include <map>

inline void emptyFunc() {}

class CInputManager
{
public:
	struct Button
	{
		using void_fn = std::function<void()>;

		void_fn			up_event	{ emptyFunc }	;
		void_fn			down_event	{ emptyFunc }	;
//		void_fn			doing_event	{ emptyFunc }	;

		const string	tag							;

		BYTE			togleKey	{ 0 }			;

		Button() = delete;
		Button(string&& tag) noexcept : tag{ std::move(tag) } {}
		Button(string&& tag, void_fn&& up, void_fn&& down) noexcept
			: tag{ std::move(tag) }, up_event{ std::move(up) }, down_event{ std::move(down) } {}
	};

	void ProcessInput(bool (&arrByte)[256]);

	void bind(UCHAR key, CInputManager::Button&& btn);
	void bind(UCHAR basekey, CInputManager::Button&& btn, LPCSTR AppName, LPCSTR configPath);
	bool replace(std::string tag, UCHAR newkey);
	bool replace(std::string tag, UCHAR newkey, LPCSTR appName, LPCSTR filePath);

	void SaveAllKey(LPCSTR appName, LPCSTR filePath);
	void SaveKey(LPCSTR appName, std::string tag, LPCSTR filePath);

	void On() { m_bActive = true; }
	void Off() { m_bActive = false; }
	bool CurrentState() const { return m_bActive; }

private:

	bool	m_bActive{ true };
	UCHAR	m_pKeyBuffer[256];
	
	map<UCHAR, Button> m_mRegisterKeyList;

	constexpr static BYTE GetHighBit(BYTE key) { return key >> 7; }
	constexpr static BYTE GetLowBit(BYTE key) { return key - (GetHighBit(key) << 7); }
};

namespace InputManager {
	#define make_button(name) CInputManager::Button name { #name };
}
