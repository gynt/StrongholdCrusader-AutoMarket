#include "pch.h"
#include "UI/Controls/Image.h"
#include "UI/Materials/IMaterial.h"

namespace UI
{

Image::Image(IMaterial const* material, RECT const& rect, HWND parent)
    : Control(rect, parent)
    , m_material(material)
{
}

void Image::SetMaterial(IMaterial const* material)
{
	m_material = material;
	Redraw();
}

void Image::OnPaint(HDC hdcDest, RECT const& rect) const
{
	if (m_material)
	{
		m_material->Paint(hdcDest, rect);
	}
}

}
