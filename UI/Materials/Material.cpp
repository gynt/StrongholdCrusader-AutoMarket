#include "pch.h"
#include "UI/Materials/Material.h"

namespace UI
{

Material::Material(std::vector<Entry> entries)
	: m_entries(std::move(entries))
{
}

Material::Material(HBITMAP bitmap, RECT const& rect, DWORD rop)
	: Material(std::vector<Entry>{ { bitmap, rect, rop } })
{
}

Material::Material(HBITMAP bitmap, HBITMAP mask, RECT const& rect)
	: Material(std::vector<Entry>{ { mask, rect, SRCAND }, { bitmap, rect, SRCPAINT } })
{
}

void Material::Paint(HDC hdcDest, const RECT& to) const
{
	if (m_entries.empty())
	{
		return;
	}

	HDC hdcSrc = CreateCompatibleDC(hdcDest);
	if (!hdcSrc)
	{
		return;
	}

	for (Entry const& entry : m_entries)
	{
		if (entry.bitmap == NULL)
		{
			continue;
		}

		RECT from = entry.rect;
		if (IsRectEmpty(&from))
		{
			BITMAP bm;
			GetObject(entry.bitmap, sizeof(BITMAP), &bm);
			from = { 0, 0, bm.bmWidth, bm.bmHeight };
		}

		SelectObject(hdcSrc, entry.bitmap);
		PaintEntry(hdcDest, to, hdcSrc, from, entry.rop);
	}

	DeleteDC(hdcSrc);
}

void Material::PaintEntry(HDC hdcDest, RECT const& to, HDC hdcSrc, RECT const& from, DWORD rop) const
{
	StretchBlt(
		hdcDest,
		to.left, to.top, to.right - to.left, to.bottom - to.top,
		hdcSrc,
		from.left, from.top, from.right - from.left, from.bottom - from.top,
		rop
	);
}

}
