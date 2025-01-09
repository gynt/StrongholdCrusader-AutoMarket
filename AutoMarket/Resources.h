#pragma once

#include "UI/Controls/Button.h"

namespace UI
{
struct IMaterial;
}

namespace AutoMarket::Resources
{

UI::IMaterial const* GetWindowBackground();
UI::IMaterial const* GetEditBackground();
UI::ButtonMaterials  GetExitButton();
UI::ButtonMaterials  GetResourceButton(size_t index);
UI::ButtonMaterials  GetButton();

}
