#pragma once

#include "UI/Materials/IMaterial.h"

#include <vector>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace UI
{

class Material : public IMaterial
{
public:
	struct Entry
	{
		HBITMAP bitmap;
		RECT    rect;
		DWORD   rop;
	};

	Material(std::vector<Entry> entries = {});
	Material(HBITMAP bitmap, RECT const& rect = {}, DWORD rop = SRCCOPY);
	Material(HBITMAP bitmap, HBITMAP mask, RECT const& rect = {});

	std::vector<Entry>&       GetEntries()       { return m_entries; }
	std::vector<Entry> const& GetEntries() const { return m_entries; }

	virtual void Paint(HDC hdc, RECT const& rect) const override;

protected:
	virtual void PaintEntry(HDC hdcDest, RECT const& to, HDC hdcSrc, RECT const& from, DWORD rop) const;

	std::vector<Entry> m_entries;
};

}
