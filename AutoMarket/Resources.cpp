#include "pch.h"
#include "AutoMarket/Resources.h"

#include "Game/Resources.h"
#include "resource.h"
#include "UI/Materials/Material.h"
#include "UI/Materials/TiledMaterial.h"
#include "UI/Materials/MaterialStack.h"
#include "UI/UniqueObject.h"
#include "UI/Materials/Utils.h"

#include <memory>

namespace AutoMarket::Resources
{

UI::IMaterial const* GetWindowBackground()
{
	static UI::UniqueObject<HBITMAP> s_bitmap;
	static std::unique_ptr<UI::TiledMaterial> s_windowBackground;

	if (!s_windowBackground)
	{
		s_bitmap = LoadBitmap(g_module, MAKEINTRESOURCE(IDB_BACKGROUND));
		s_windowBackground = std::make_unique<UI::TiledMaterial>(s_bitmap);
	}
	return s_windowBackground.get();
}

UI::IMaterial const* GetEditBackground()
{
	static UI::UniqueObject<HBITMAP> s_bitmap;
	static UI::UniqueObject<HBITMAP> s_mask;
	static std::unique_ptr<UI::TiledMaterial> s_editBackground;

	if (!s_editBackground)
	{
		s_bitmap = LoadBitmap(g_module, MAKEINTRESOURCE(IDB_EDITBOX));
		s_mask   = UI::CreateBitmapMask(s_bitmap);
		s_editBackground = std::make_unique<UI::TiledMaterial>(s_bitmap, s_mask);
	}
	return s_editBackground.get();
}

UI::ButtonMaterials GetExitButton()
{
	static UI::UniqueObject<HBITMAP> s_bitmap;
	static std::unique_ptr<UI::MaterialStack<2, 1>> s_exitButtons;

	if (!s_exitButtons)
	{
		s_bitmap = LoadBitmap(g_module, MAKEINTRESOURCE(IDB_EXITBUTTON));
		s_exitButtons = std::make_unique<UI::MaterialStack<2, 1>>(s_bitmap);
	}

	return { 
		&s_exitButtons->materials[0][0],
		&s_exitButtons->materials[1][0],
		nullptr,
		nullptr
	};
}

UI::ButtonMaterials GetResourceButton(size_t index)
{
	static UI::UniqueObject<HBITMAP> s_bitmap;
	static UI::UniqueObject<HBITMAP> s_mask;
	static std::unique_ptr<UI::MaterialStack<Game::Resource::Max, 4>> s_resourceButtons;

	if (!s_resourceButtons)
	{
		s_bitmap = LoadBitmap(g_module, MAKEINTRESOURCE(IDB_ITEMS));
		s_mask   = UI::CreateBitmapMask(s_bitmap);
		s_resourceButtons = std::make_unique<UI::MaterialStack<Game::Resource::Max, 4>>(s_bitmap, s_mask);
	}

	return { 
		&s_resourceButtons->materials[index][0],
		&s_resourceButtons->materials[index][1],
		&s_resourceButtons->materials[index][2],
		&s_resourceButtons->materials[index][3]
	};	
}

UI::ButtonMaterials GetButton()
{
	static UI::UniqueObject<HBITMAP> s_bitmap;
	static std::unique_ptr<UI::MaterialStack<1, 4, UI::TiledMaterial>> s_buttons;

	if (!s_buttons)
	{
		s_bitmap = LoadBitmap(g_module, MAKEINTRESOURCE(IDB_BUTTON));
		RECT rect = UI::GetBitmapRect(s_bitmap);
		RECT bitmapRect = { rect.left,    rect.top, rect.right/2, rect.bottom };
		RECT maskRect   = { rect.right/2, rect.top, rect.right,   rect.bottom };
		s_buttons = std::make_unique<UI::MaterialStack<1, 4, UI::TiledMaterial>>(
			s_bitmap, bitmapRect,
			s_bitmap, maskRect
		);
	}

	return {
		&s_buttons->materials[0][0],
		&s_buttons->materials[0][1],
		&s_buttons->materials[0][2],
		&s_buttons->materials[0][3]
	};
}

}
