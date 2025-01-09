#pragma once

#include "UI/Controls/Image.h"

#include <functional>
#include <string>
#include <optional>

namespace UI
{

class Edit : public Image
{
public:
	Edit(std::wstring text, IMaterial const* background, RECT const& rect, HWND parent = NULL);

	void                SetText(std::wstring text);
	std::wstring const& GetText() const { return m_text; }
				   
	void                SetTextMargins(RECT const& margins) { m_textMargins = margins; }
	RECT const&         GetTextMargins() const              { return m_textMargins; }

	void                SetTextColor(COLORREF color);
	COLORREF            GetTextColor() const { return m_textColor; }

	void                SetCursorBrush(HBRUSH brush);
	HBRUSH              GetCursorBrush() const { return m_cursorBrush; }

	void                SetCharLimit(size_t limit);
	size_t              GetCharLimit() const { return m_charLimit; }

	std::function<void(std::wstring const&)> onChanged;

protected:
	virtual void OnPaint(HDC hdcDest, RECT const& rect) const override;
	virtual void OnMouseDown(int x, int y) override;
	virtual void OnMouseMove(int x, int y) override;
	virtual void OnMouseUp(int x, int y) override;
	virtual void OnMouseLeave() override;
	virtual bool OnKeyDown(int key) override;
	virtual bool OnChar(int ch) override;

	virtual void OnSetFocus() override;
	virtual void OnLoseFocus() override;
	
	virtual bool IsCharInvalid(int ch);
	virtual void OnChanged();

	RECT         CalcTextRect() const;
	size_t       MouseToTextPos(int x, int y);

	void EraseChar();
	void InsertChar(int ch);
	void MoveCursor(int dist);

protected:
	std::wstring m_text;
	RECT         m_textMargins;
	COLORREF     m_textColor;
	HBRUSH       m_cursorBrush;
	size_t       m_charLimit;

	size_t       m_cursorPos;
	bool         m_isDown;
	std::optional<size_t> m_selectionStart;
};

}
