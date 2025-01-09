#include "pch.h"
#include "UI/Controls/Edit.h"
#include "UI/ControlManager.h"
#include "UI/UniqueObject.h"

namespace UI
{

static constexpr RECT       s_defaultTextMargins = { 10, 5, 10, 5 };
static constexpr COLORREF   s_defaultTextColor   = RGB(255, 255, 255);
static UniqueObject<HBRUSH> const s_defaultCursorBrush = CreateSolidBrush(RGB(79, 123, 47));

Edit::Edit(std::wstring text, IMaterial const* background, RECT const& rect, HWND parent)
	: Image(background, rect, parent)
	, m_text(std::move(text))
	, m_textMargins(s_defaultTextMargins)
	, m_textColor(s_defaultTextColor)
	, m_cursorBrush(s_defaultCursorBrush)
	, m_cursorPos(0)
{
}

void Edit::SetText(std::wstring text)
{
	if (text == m_text)
	{
		return;
	}

	m_text = std::move(text);
	Redraw();
}

void Edit::SetTextColor(COLORREF color)
{
	m_textColor = color;
	Redraw();
}

void Edit::SetCursorBrush(HBRUSH brush)
{
	m_cursorBrush = brush;
	Redraw();
}

void Edit::OnPaint(HDC hdcDest, RECT const& rect) const
{
	// Draw background
	Image::OnPaint(hdcDest, rect);

	// Draw Text
	RECT textRect = CalcTextRect();
	{
		RECT emptyRect{}; // for text height
		DrawText(hdcDest, L"", 0, &emptyRect, DT_SINGLELINE | DT_CALCRECT);
		int rectHeight = textRect.bottom - textRect.top;
		int textHeight = emptyRect.bottom - emptyRect.top;

		// Center text. Don't use DT_VCENTER bc in combination with DT_CALCRECT (needed for cursor) it returns the box height instead of the text height.
		RECT drawRect = {
			textRect.left,
			std::max(textRect.top, textRect.top + (rectHeight - textHeight) / 2),
			textRect.right,
			std::min(textRect.bottom, textRect.bottom - (rectHeight - textHeight) / 2),
		};

		::SetTextColor(hdcDest, m_textColor);
		SetBkMode(hdcDest, TRANSPARENT);
		DrawText(hdcDest, m_text.data(), m_text.size(), &drawRect, DT_SINGLELINE);
	}

	// Draw cursor
	if (HasFocus() && m_cursorBrush)
	{
		RECT cursorRect{};
		DrawText(hdcDest, m_text.data(), m_cursorPos, &cursorRect, DT_SINGLELINE | DT_CALCRECT);

		RECT drawRect{
			textRect.left + cursorRect.right - 1,
			textRect.top,
			textRect.left + cursorRect.right + 1,
			textRect.bottom
		};
		FillRect(hdcDest, &drawRect, m_cursorBrush);
	}
}

void Edit::OnMouseDown(int x, int y)
{
	m_cursorPos = MouseToTextPos(x, y);

	Redraw();
}

bool Edit::OnKeyDown(int key)
{
	switch (key)
	{
	case VK_ESCAPE:
		return false; // Let the parent window handle this one.
	case VK_LEFT:
		if (m_cursorPos > 0)
		{
			--m_cursorPos;
		}
		break;
	case VK_RIGHT:
		if (m_cursorPos < m_text.size())
		{
			++m_cursorPos;
		}
		break;
	case VK_DELETE:
		if (m_cursorPos < m_text.size())
		{
			m_text.erase(m_text.begin() + m_cursorPos);
		}
		break;
	case VK_BACK:
		if (m_cursorPos > 0)
		{
			m_text.erase(m_text.begin() + (m_cursorPos - 1));
			--m_cursorPos;
		}
		break;
	case VK_RETURN:
	case VK_TAB:
		GetControlManager().SetFocus(nullptr);
		return true;
	default:
		return true;
	}
	Redraw();
	return true;
}

bool Edit::OnChar(int ch)
{
	if (!IsCharInvalid(ch))
	{
		m_text.insert(m_text.begin() + m_cursorPos, ch);
		++m_cursorPos;

		Redraw();

		OnChanged();
	}
	return true;
}

void Edit::OnSetFocus()
{
	m_cursorPos = m_text.size();
	Redraw();
}

void Edit::OnLoseFocus()
{
	m_cursorPos = 0;
	Redraw();
}

bool Edit::IsCharInvalid(int ch)
{
	return ch <= L'\x1F' || (ch >= L'\x7F' && ch <= L'\x9F');
}

void Edit::OnChanged()
{
	if (onChanged)
	{
		onChanged(m_text);
	}
}

RECT Edit::CalcTextRect() const
{
	RECT rect = GetLocalRect();
	rect.left   += m_textMargins.left;
	rect.top    += m_textMargins.top;
	rect.right  -= m_textMargins.right;
	rect.bottom -= m_textMargins.bottom;
	return rect;
}

size_t Edit::MouseToTextPos(int x, int y)
{
	size_t result = m_text.size();
	if (HDC const hdc = CreateCompatibleDC(NULL))
	{
		RECT const textRect = CalcTextRect();
		x -= textRect.left;

		int prev = 0;
		for (size_t i = 1; i <= m_text.size(); ++i)
		{
			RECT rect{};
			DrawText(hdc, m_text.data(), i, &rect, DT_SINGLELINE | DT_CALCRECT);

			if (x < prev + (rect.right - prev)/2)
			{
				result = i - 1;
				break;
			}
			else if (x < rect.right)
			{
				result = i;
				break;
			}

			prev = rect.right;
		}
		DeleteDC(hdc);
	}
	return result;
}

}
