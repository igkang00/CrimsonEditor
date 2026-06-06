#include "stdafx.h"
#include "cedtHeader.h"
#include "PrefDialog.h"


void CPreferenceDialog::InitOutputPage()
{
}

void CPreferenceDialog::SizeOutputPage()
{
}

void CPreferenceDialog::ShowOutputPage()
{
	INT nCmdShow = (m_nActiveCategory == PREF_CATEGORY_OUTPUT) ? SW_SHOW : SW_HIDE;
}

BOOL CPreferenceDialog::LoadOutputSettings()
{
	return TRUE;
}

BOOL CPreferenceDialog::SaveOutputSettings()
{
	return TRUE;
}
