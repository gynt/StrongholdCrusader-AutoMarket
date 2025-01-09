#pragma once

#include "UI/Controls/Control.h"

#include <functional>
#include <string>

namespace UI
{
struct IMaterial;
}

namespace UI
{

struct ButtonMaterials
{
	IMaterial const* normal;
	IMaterial const* normalHover;
	IMaterial const* pushed;
	IMaterial const* pushedHover;
};

class Button : public Control
{
public:
	Button(RECT const& rect, HWND parent = NULL);
	Button(ButtonMaterials const& bitmaps, RECT const& rect, HWND parent = NULL);
	Button(std::wstring title, ButtonMaterials const& bitmaps, RECT const& rect, HWND parent = NULL);

	void                SetMaterials(ButtonMaterials const& bitmaps);

	void                SetTitle(std::wstring title);
	std::wstring const& GetTitle() const { return m_title; }

	void                SetTitleColor(COLORREF color);
	COLORREF            GetTitleColor() const { return m_titleColor; }

	void                SetPushable(bool isPushable);
	bool                IsPushable() const { return m_isPushable; }

	bool                IsPushed() const   { return m_isPushed; }
	void                SetPushed(bool isPushed, bool sendSignal = false);

	std::function<void(bool)> onClick;

protected:
	virtual void OnPaint(HDC hdcDest, RECT const& rect) const override;
	virtual void OnMouseDown(int x, int y) override;
	virtual void OnMouseUp(int x, int y) override;
	virtual void OnMouseEnter() override;
	virtual void OnMouseLeave() override;

	IMaterial const* GetCurrentMaterial() const;

private:
	ButtonMaterials m_materials;
	std::wstring    m_title;
	COLORREF        m_titleColor;
	bool            m_isPushable;

	bool            m_isPushed;
	bool            m_isDown;
};

}
