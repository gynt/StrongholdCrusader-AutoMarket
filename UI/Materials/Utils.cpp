#include "pch.h"
#include "UI/Materials/Utils.h"

namespace UI
{

HBITMAP CreateBitmapMask(HBITMAP bitmap, std::optional<COLORREF> transparentColor)
{
    if (!bitmap)
    {
        return NULL;
    }

    // Create monochrome (1 bit) mask bitmap.
    BITMAP bm{};
    GetObject(bitmap, sizeof(BITMAP), &bm);
    HBITMAP mask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

    // Get some HDCs that are compatible with the display driver
    HDC hdcBitmap = CreateCompatibleDC(NULL);
    HDC hdcMask = CreateCompatibleDC(NULL);

    SelectObject(hdcBitmap, bitmap); // SelectBitmap
    SelectObject(hdcMask, mask); // SelectBitmap

    if (!transparentColor.has_value())
    {
        transparentColor = GetPixel(hdcBitmap, 0, 0);
    }

    // Set the background color of the bitmap to the color that represents transparency.
    SetBkColor(hdcBitmap, transparentColor.value());

    // Copy the bits from the color image to the B+W mask.
    // Everything with the background color ends up white while everything else ends up black.
    BitBlt(hdcMask, 0, 0, bm.bmWidth, bm.bmHeight, hdcBitmap, 0, 0, SRCCOPY);

    // Take the mask and use it to turn the transparent color in our original bitmap to black so the transparency effect will work.
    BitBlt(hdcBitmap, 0, 0, bm.bmWidth, bm.bmHeight, hdcMask, 0, 0, SRCINVERT);

    // Clean up.
    DeleteDC(hdcBitmap);
    DeleteDC(hdcMask);
    return mask;
}

RECT GetBitmapRect(HBITMAP bitmap)
{
    BITMAP bm;
    GetObject(bitmap, sizeof(BITMAP), &bm);
    return { 0, 0, bm.bmWidth, bm.bmHeight };
}

}
