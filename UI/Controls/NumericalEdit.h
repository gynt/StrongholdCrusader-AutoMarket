#pragma once

#include "UI/Controls/Edit.h"

namespace UI
{

class NumericalEdit : public Edit
{
public:
	NumericalEdit(unsigned int value, UI::IMaterial const* background, RECT const& rect, HWND parent = NULL);

	void SetValue(unsigned int value);
	unsigned int GetValue() const;

	std::function<void(unsigned int)> onValueChanged;

protected:
	virtual bool IsCharInvalid(int ch) override;
	virtual void OnChanged() override;
};

}
