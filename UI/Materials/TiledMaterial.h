#pragma once

#include "UI/Materials/Material.h"

namespace UI
{

class TiledMaterial : public Material
{
public:
	using Material::Material;

protected:
	virtual void PaintEntry(HDC hdcDest, RECT const& to, HDC hdcSrc, RECT const& from, DWORD rop) const override;
};

}
