#pragma once

#include <windef.h>

namespace UI
{

struct IMaterial
{
	virtual      ~IMaterial() = default;
	virtual void Paint(HDC hdc, RECT const& rect) const = 0;
};

}
