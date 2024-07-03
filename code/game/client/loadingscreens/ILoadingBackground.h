//======= Maestra Fenix, 2017 ==================================================//
//======= Cvoxalury, 2024 ======================================================//
//
// Purpose: Map load background panel
//
//==============================================================================//
#ifndef MAPLOAD_BACKGROUND_H
#define MAPLOAD_BACKGROUND_H

#ifdef _WIN32
#pragma once
#endif

#include "vgui/ISurface.h"
#include "vgui/IVGui.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/ProgressBar.h>
#include "vgui_controls/CircularProgressBar.h"
#include <vgui_controls/Label.h>
#include "ienginevgui.h"
#include <KeyValues.h>

#define ENABLE_LOADING_TIP // if defined, adds an element for tip text
#define ENABLE_CUSTOM_LOADING_BAR // if defined, overrides the loading bar, hides the original one
#define ENABLE_CUSTOM_LOADING_WHEEL // if defined, a circular progress bar will appear in bottom right corner

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CMapLoadBG : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE(CMapLoadBG, vgui::EditablePanel);
public:

	// Construction
	CMapLoadBG(char const *panelName);
	~CMapLoadBG();

	bool FindLoadingDialogBarHandle(void);
	int iLoadingBarHandle;
	float progress;

	void OnMessage(const KeyValues *params, vgui::VPANEL fromPanel);
	void OnThink(void);

	void SetNewBackgroundImage(char const *imageName);

protected:
	void ApplySchemeSettings(vgui::IScheme *pScheme);

private:
	void SelectDefaultBackground(void);
	void SelectSpecificBackground(void);
	vgui::ImagePanel *m_pBackground;
	bool m_bResetBackground; // used to know to reset background to default after choosing a map-specific one, so it doesn't persist.
#ifdef ENABLE_CUSTOM_LOADING_BAR
	vgui::ContinuousProgressBar	*m_pProgressBar;
	vgui::Label *m_pProgressPercentage;
//#if HL2MP
//	vgui::Label *m_pProgressStatusMessage; // the mp-specific message about connecting, downloading
//#endif
#endif
#ifdef ENABLE_LOADING_TIP
	vgui::Label *m_pLoadingTip;
#endif
#ifdef ENABLE_CUSTOM_LOADING_WHEEL
	vgui::CircularProgressBar *m_pProgressWheel;
#endif
};

/*
ProgressBar directions are:
0 - left to right (or 'east')
1 - right to left ('west')
2 - bottom to top ('north')
3 - top to bottom ('south')
*/

#endif	// !MAPLOAD_BACKGROUND_H
