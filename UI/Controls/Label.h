#pragma once

#include "UI/Controls/Control.h"
#include "UI/UniqueObject.h"

#include <string>

namespace UI
{

class Label : public Control
{
public:
	Label(std::wstring text, LONG textSize, COLORREF textColor, const RECT& rect, HWND parent = NULL);

protected:
	virtual void OnPaint(HDC hdcDest, RECT const& rect) const override;

protected:
	std::wstring        m_text;
	UniqueObject<HFONT> m_font;
	COLORREF            m_textColor;
};

}
