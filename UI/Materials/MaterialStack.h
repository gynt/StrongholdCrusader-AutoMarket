#pragma once

#include "UI/Materials/Material.h"
#include "UI/Materials/Utils.h"

namespace UI
{

template<size_t X, size_t Y, typename TMaterial = Material>
struct MaterialStack
{
	static_assert(X != 0);
	static_assert(Y != 0);

	MaterialStack(HBITMAP bitmap, RECT const& rect = {})
		: MaterialStack(bitmap, NULL, rect)
	{
	}

	MaterialStack(HBITMAP bitmap, HBITMAP mask, RECT const& rect = {})
		: MaterialStack(bitmap, rect, mask, rect)
	{
	}

	MaterialStack(HBITMAP bitmap, RECT const& bitmapRect, HBITMAP mask, RECT const& maskRect)
		: materials()
	{
		if (mask)
		{
			InitStack(mask, maskRect, SRCAND);
		}
		if (bitmap)
		{
			InitStack(bitmap, bitmapRect, SRCPAINT);
		}
	}

	void InitStack(HBITMAP bitmap, RECT rect, DWORD rop)
	{
		if (IsRectEmpty(&rect))
		{
			rect = GetBitmapRect(bitmap);
		}

		int w = (rect.right - rect.left) / X;
		int h = (rect.bottom - rect.top) / Y;

		int x = rect.left;
		for (size_t i = 0; i < X; ++i)
		{
			int y = rect.top;
			for (size_t j = 0; j < Y; ++j)
			{
				RECT newRect{ x, y, x + w, y + w };
				materials[i][j].GetEntries().push_back({ bitmap, newRect, rop });
				y += h;
			}
			x += w;
		}
	}

	TMaterial materials[X][Y];
};

}
