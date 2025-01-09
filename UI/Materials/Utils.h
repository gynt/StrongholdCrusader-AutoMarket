#pragma once

#include <windef.h>
#include <optional>

namespace UI
{

HBITMAP CreateBitmapMask(HBITMAP bitmap, std::optional<COLORREF> transparentColor = {});
RECT    GetBitmapRect(HBITMAP bitmap);

}
