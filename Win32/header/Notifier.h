#pragma once

#include <type_traits>

template <typename Ty>
class Notifier
{
public:
	explicit Notifier() noexcept
		: Val{}
	{
	}

	explicit Notifier(const Ty& val) noexcept
		: Val{ val }
	{
	}

	explicit Notifier(Ty&& val) noexcept
		: Val{ std::move(val) }
	{
	}

	void SetNotify() { notify = true; }
	bool CheckNotify(bool RemoveNotify = true) { if (!notify) return false; if (RemoveNotify) notify = false; return true; }

	Ty* operator->() { return &Val; }
	Ty& operator*() const { return Val; }

	Notifier& operator=(const Ty& other) const { Val = other; return *this; }
	Notifier& operator=(Ty&& other) const { Val = std::move(other); return *this; }

	Notifier& operator=(const Notifier& other) const { Val = other.Val; notify = other.notify; return *this; }

private:
	Ty Val;
	bool notify = false;
};
