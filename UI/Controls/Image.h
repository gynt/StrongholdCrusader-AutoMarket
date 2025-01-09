#pragma once

#include "UI/Controls/Control.h"

namespace UI
{
struct IMaterial;
}

namespace UI
{

class Image : public Control
{
public:
	Image(IMaterial const* material, RECT const& rect, HWND parent = NULL);

	void             SetMaterial(IMaterial const* material);
	IMaterial const* GetMaterial() const { return m_material; }

protected:
	virtual void OnPaint(HDC hdcDest, RECT const& rect) const override;

	IMaterial const* m_material;
};

}
