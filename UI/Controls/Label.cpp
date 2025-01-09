#include "pch.h"
#include "UI/Controls/Label.h"

namespace UI
{

static constexpr int s_defaultStockFont = DEFAULT_GUI_FONT;

static HFONT CreateFontFromExisting(int stockFont, LONG textSize)
{
    HFONT hFont = (HFONT)GetStockObject(stockFont);

    LOGFONT fnt;
    GetObject(hFont, sizeof(LOGFONT), &fnt);
    fnt.lfHeight = textSize;

    return CreateFontIndirect(&fnt);
}

Label::Label(std::wstring text, LONG textSize, COLORREF textColor, const RECT& rect, HWND parent)
    : Control(rect, parent)
    , m_text(std::move(text))
    , m_font(CreateFontFromExisting(s_defaultStockFont, textSize))
    , m_textColor(textColor)
{
}

void Label::OnPaint(HDC hdcDest, RECT const& rect) const
{
    SetBkMode(hdcDest, TRANSPARENT);
    SetTextColor(hdcDest, m_textColor);
    SelectObject(hdcDest, m_font);

    RECT drawRect = rect;
    DrawText(hdcDest, m_text.data(), m_text.size(), &drawRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

}
