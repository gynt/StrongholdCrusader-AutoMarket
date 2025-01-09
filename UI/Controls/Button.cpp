#include "pch.h"
#include "UI/Controls/Button.h"
#include "UI/Materials/IMaterial.h"

namespace UI
{

static constexpr COLORREF s_defaultTitleColor = RGB(255, 255, 255);

Button::Button(RECT const& rect, HWND parent)
	: Button({}, rect, parent)
{
}

Button::Button(ButtonMaterials const& materials, RECT const& rect, HWND parent)
	: Button({}, materials, rect, parent)
{
}

Button::Button(std::wstring title, ButtonMaterials const& materials, RECT const& rect, HWND parent)
	: Control(rect, parent)
	, m_materials(materials)
	, m_title(std::move(title))
	, m_titleColor(s_defaultTitleColor)
	, m_isPushable(false)
	, m_isPushed(false)
	, m_isDown(false)
{
}

void Button::SetMaterials(ButtonMaterials const& materials)
{
	m_materials = materials;
	Redraw();
}

void Button::SetTitle(std::wstring title)
{
	m_title = std::move(title);
	Redraw();
}

void Button::SetTitleColor(COLORREF color)
{
	m_titleColor = color;
	Redraw();
}

void Button::SetPushable(bool isPushable)
{
	if (m_isPushable == isPushable)
	{
		return;
	}

	m_isPushable = isPushable;
	if (m_isPushed)
	{
		m_isPushed = false;
		Redraw();
	}
}

void Button::SetPushed(bool isPushed, bool sendSignal)
{
	if (!m_isPushable || m_isPushed == isPushed)
	{
		return;
	}
	m_isPushed = isPushed;
	Redraw();

	if (sendSignal && onClick)
	{
		onClick(m_isPushed);
	}
}

void Button::OnPaint(HDC hdcDest, RECT const& rect) const
{
	if (IMaterial const* currentMat = GetCurrentMaterial())
	{
		currentMat->Paint(hdcDest, rect);
	}

	if (!m_title.empty())
	{
		SetBkMode(hdcDest, TRANSPARENT);
		SetTextColor(hdcDest, m_titleColor);

		RECT drawRect = rect;
		if (m_isDown)
		{
			// Small "click-down" animation.
			drawRect.left += 2;
			drawRect.top += 2;
		}
		DrawText(hdcDest, m_title.data(), m_title.size(), &drawRect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	}
}

void Button::OnMouseDown(int x, int y)
{
	m_isDown = true;
	if (m_isPushable)
	{
		m_isPushed = !m_isPushed;
	}
	Redraw();

	if (onClick)
	{
		onClick(m_isPushed);
	}
}

void Button::OnMouseUp(int x, int y)
{
	m_isDown = false;
	Redraw();
}

void Button::OnMouseEnter()
{
	Redraw(); // Update hover effect.
}

void Button::OnMouseLeave()
{
	m_isDown = false;
	Redraw(); // Update hover effect.
}

IMaterial const* Button::GetCurrentMaterial() const
{
	if (IsPushed())
	{
		if (IsHovered() && m_materials.pushedHover)
		{
			return m_materials.pushedHover;
		}
		else if (m_materials.pushed)
		{
			return m_materials.pushed;
		}
	}

	if (IsHovered() && m_materials.normalHover)
	{
		return m_materials.normalHover;
	}

	return m_materials.normal;
}

}
