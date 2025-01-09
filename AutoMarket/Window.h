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
	virtual bool OnKeyDown(int key) override;

	void CreateControls();
	void CreateResourceControl(size_t resource, int x, int y, HWND parent);
	void Close() const;

	void RegisterFocusListener();
	void DeregisterFocusListener();

protected:
	Manager& m_market;
};

}
