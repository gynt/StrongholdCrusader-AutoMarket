#include "pch.h"
#include "UI/Materials/TiledMaterial.h"

namespace UI
{

void TiledMaterial::PaintEntry(HDC hdcDest, RECT const& inTo, HDC hdcSrc, RECT const& inFrom, DWORD rop) const
{
	int wTile = (inFrom.right - inFrom.left) / 3; // left, center, right
	int hTile = (inFrom.bottom - inFrom.top) / 3; // top, center, bottom

	RECT from{};
	RECT to{};
	for (size_t x = 0; x < 3; ++x)
	{
		switch (x)
		{
		case 0: // left
			from.left = inFrom.left;
			from.right = inFrom.left + wTile;
			to.left = inTo.left;
			to.right = inTo.left + wTile;
			break;
		case 1: // center
			from.left = inFrom.left + wTile;
			from.right = inFrom.right - wTile;
			to.left = inTo.left + wTile;
			to.right = inTo.right - wTile;
			break;
		case 2: // right
			from.left = inFrom.right - wTile;
			from.right = inFrom.right;
			to.left = inTo.right - wTile;
			to.right = inTo.right;
			break;
		}

		for (size_t y = 0; y < 3; ++y)
		{
			switch (y)
			{
			case 0: // top
				from.top = inFrom.top;
				from.bottom = inFrom.top + hTile;
				to.top = inTo.top;
				to.bottom = inTo.top + hTile;
				break;
			case 1: // center
				from.top = inFrom.top + hTile;
				from.bottom = inFrom.bottom - hTile;
				to.top = inTo.top + hTile;
				to.bottom = inTo.bottom - hTile;
				break;
			case 2: // bottom
				from.top = inFrom.bottom - hTile;
				from.bottom = inFrom.bottom;
				to.top = inTo.bottom - hTile;
				to.bottom = inTo.bottom;
				break;
			}

			Material::PaintEntry(hdcDest, to, hdcSrc, from, rop);
		}
	}
}

}
