#include "pch.h"
#include "UI/Controls/NumericalEdit.h"

namespace UI
{
NumericalEdit::NumericalEdit(unsigned int value, UI::IMaterial const* background, RECT const& rect, HWND parent)
	: Edit(std::to_wstring(value), background, rect, parent)
{
}

void UI::NumericalEdit::SetValue(unsigned int value)
{
	SetText(std::to_wstring(value));
}

unsigned int NumericalEdit::GetValue() const
{
	return wcstoul(m_text.data(), nullptr, 10);
}

bool NumericalEdit::IsCharInvalid(int ch)
{
	return ch < L'0' || ch > L'9';
}

void NumericalEdit::OnChanged()
{
	if (onValueChanged)
	{
		onValueChanged(GetValue());
	}

	Edit::OnChanged();
}

}
