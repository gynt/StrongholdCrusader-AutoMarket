#pragma once

#include "UI/Controls/Image.h"

#include <functional>

namespace AutoMarket
{
class Manager;
}

namespace AutoMarket
{

class Window : public UI::Image
{
public:
	Window(Manager& market, HWND parent = NULL);
	~Window();

	std::function<void()> onExit;

protected:
	void CreateControls();
	void CreateResourceControl(size_t resource, int x, int y, HWND parent);

protected:
	Manager& m_market;
};

}
