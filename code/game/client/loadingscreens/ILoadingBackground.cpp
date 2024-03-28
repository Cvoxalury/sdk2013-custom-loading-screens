//======= Maestra Fenix, 2017 ==================================================//
//======= Cvoxalury, 2024 ======================================================//
//
// Purpose: Map load background panel
// Todo: The MP side of things. Secondary bar for when external files are needed,
// status text related to that, statis text for the first bar (connecting, etc)...
// The secondary bar in vanilla appears to be called "Progress2", but I don't
// know how to extract its text messages, and I don't have the ability to
// test downloading external files to verify its working.
//==============================================================================//

#include "cbase.h"
#include "ILoadingBackground.h"
#include "GameUI/IGameUI.h"
#include "vgui\ILocalize.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

IMaterial *mapBackground;

CMapLoadBG::CMapLoadBG(char const *panelName) : EditablePanel(NULL, panelName)
{
	VPANEL toolParent = enginevgui->GetPanel(PANEL_GAMEUIDLL);
	SetParent(toolParent);

	m_pBackground = new ImagePanel(this, "Background");

	if (m_pBackground)
	{
		m_pBackground->SetSize(ScreenWidth(), ScreenHeight());
		m_pBackground->SetShouldScaleImage(true);
		m_pBackground->SetImage("loading/_default");
	}

#ifdef ENABLE_CUSTOM_LOADING_BAR
	m_pProgressBar = new ContinuousProgressBar(this, "ProgressBar");

	if (m_pProgressBar) m_pProgressBar->SetProgressDirection(0);

	m_pProgressPercentage = new vgui::Label(this, "ProgressPercentage", "   ");

//#if HL2MP
//	m_pProgressStatusMessage = new vgui::Label(this, "ProgressStatusMessage", "   ");
//#endif
#endif

#ifdef ENABLE_CUSTOM_LOADING_WHEEL	
	m_pProgressWheel = new CircularProgressBar(this, "ProgressWheel");

	if (m_pProgressWheel) m_pProgressWheel->SetProgressDirection(vgui::CircularProgressBar::PROGRESS_CW);
#endif

#ifdef ENABLE_LOADING_TIP
	m_pLoadingTip = new vgui::Label(this, "LoadingTip", " ");
#endif

	progress = 0;
	iLoadingBarHandle = 0;
	mapBackground = NULL;
	m_bResetBackground = true;

	LoadControlSettings("resource/ui/loadingscreen.res");
}

CMapLoadBG::~CMapLoadBG()
{
	// None
}

void CMapLoadBG::ApplySchemeSettings(IScheme *pScheme)
{
	IScheme *pClientScheme = vgui::scheme()->GetIScheme(vgui::scheme()->GetScheme("ClientScheme"));

	int iWide, iTall;
	surface()->GetScreenSize(iWide, iTall);
	SetSize(iWide, iTall);

#ifdef ENABLE_CUSTOM_LOADING_BAR

	if (m_pProgressBar)
	{
		m_pProgressBar->SetAlpha(100);
		m_pProgressBar->SetPaintBorderEnabled(false);
		m_pProgressBar->SetSize(max(256, iWide * 0.66f), max(8, (iTall / 100)));
		m_pProgressBar->SetPos(iWide / 6, iTall * 0.9f);

		if (m_pProgressPercentage)
		{
			m_pProgressPercentage->SetFont(pClientScheme->GetFont("Default", true));
			m_pProgressPercentage->SetSize(32, 32);
			m_pProgressPercentage->SetPos(iWide / 6 + m_pProgressBar->GetWide() + m_pProgressPercentage->GetWide() / 2, (iTall * 0.9f) - m_pProgressBar->GetTall() / 2);
			m_pProgressPercentage->SetContentAlignment(vgui::Label::a_southwest);
		}
//#if HL2MP
//		if (m_pProgressStatusMessage)
//		{
//			m_pProgressStatusMessage->SetFont(pClientScheme->GetFont("Default", true));
//			m_pProgressStatusMessage->SetSize(32, 32);
//			m_pProgressStatusMessage->SetPos(iWide / 2 - m_pProgressStatusMessage->GetWide() / 2, iTall * 0.95);
//			m_pProgressStatusMessage->SetContentAlignment(vgui::Label::a_southwest);
//		}
//#endif
	}
#endif
#ifdef ENABLE_CUSTOM_LOADING_WHEEL
	if (m_pProgressWheel)
	{
		m_pProgressWheel->SetAlpha(100);
		m_pProgressWheel->SetPaintBorderEnabled(false);
		m_pProgressWheel->SetSize(min(64, iWide * 0.1), min(64, iWide * 0.1));
		m_pProgressWheel->SetPos((iWide)-(m_pProgressWheel->GetWide() * 1.5f), (iTall)-(m_pProgressWheel->GetTall() * 1.5f));
		m_pProgressWheel->SetBgImage("loading/loading_wheel_bg");
		m_pProgressWheel->SetFgImage("loading/loading_wheel_fg");
	}
#endif

#ifdef ENABLE_LOADING_TIP
	if (m_pLoadingTip)
	{
		m_pLoadingTip->SetFont(pClientScheme->GetFont("Default", true));
		m_pLoadingTip->SetContentAlignment(vgui::Label::a_center);
		m_pLoadingTip->SizeToContents();
		m_pLoadingTip->SetPos(iWide / 2 - m_pLoadingTip->GetWide() / 2, iTall * 0.85f);
	}
#endif

	BaseClass::ApplySchemeSettings(pScheme);
}

//-----------------------------------------------------------------------------
// Purpose: Scan from the root panel down its children to find the handle
// to the loading bar panel.
//-----------------------------------------------------------------------------
bool CMapLoadBG::FindLoadingDialogBarHandle(void)
{
	int iGameuipanel, iBasegameuipanel, iLoadingdialog;
	iGameuipanel = iBasegameuipanel = iLoadingdialog = 0;
	VPANEL searchpanel = vgui::ivgui()->HandleToPanel(1); // 1 is the parent of gameuipanel
	if (searchpanel)
	{
		for (int i = 0; i < vgui::ipanel()->GetChildCount(searchpanel); i++)
		{
			VPANEL tmppanel = vgui::ipanel()->GetChild(searchpanel, i);

			if (!Q_strcmp(vgui::ipanel()->GetName(tmppanel), "GameUI Panel"))
				iGameuipanel = vgui::ivgui()->PanelToHandle(tmppanel);
		}
	}

	if (iGameuipanel != 0) // the gameui panel, which hosts the parent of loading dialogue
	{
		//	AssertMsg(0, "iGameuipanel is %i", iGameuipanel);
		searchpanel = vgui::ivgui()->HandleToPanel(iGameuipanel);
		if (searchpanel)
		{
			for (int i = 0; i < vgui::ipanel()->GetChildCount(searchpanel); i++)
			{
				VPANEL tmppanel = vgui::ipanel()->GetChild(searchpanel, i);

				if (!Q_strcmp(vgui::ipanel()->GetName(tmppanel), "BaseGameUIPanel"))
				{
					iBasegameuipanel = vgui::ivgui()->PanelToHandle(tmppanel);
				}
			}
		}
	}

	if (iBasegameuipanel != 0) // the parent of loading dialogue
	{
		//	AssertMsg(0, "iBasegameuipanel is %i", iBasegameuipanel);
		searchpanel = vgui::ivgui()->HandleToPanel(iBasegameuipanel);
		if (searchpanel)
		{
			for (int i = 0; i < vgui::ipanel()->GetChildCount(searchpanel); i++)
			{
				VPANEL tmppanel = vgui::ipanel()->GetChild(searchpanel, i);

				if (!Q_strcmp(vgui::ipanel()->GetName(tmppanel), "LoadingDialog"))
				{
					iLoadingdialog = vgui::ivgui()->PanelToHandle(tmppanel);
					vgui::ipanel()->SetPos(tmppanel, -99999, -99999); // HACK: for some reason SetVisible doesn't work?.. push it out into the cosmic void.
				}
			}
		}
	}

	if (iLoadingdialog != 0) // the loading dialogue, parent of loading bar element
	{
		//	AssertMsg(0, "iLoadingdialog is %i", iLoadingdialog);
		searchpanel = vgui::ivgui()->HandleToPanel(iLoadingdialog);
		if (searchpanel)
		{
			for (int i = 0; i < vgui::ipanel()->GetChildCount(searchpanel); i++)
			{
				VPANEL tmppanel = vgui::ipanel()->GetChild(searchpanel, i);

				if (!Q_strcmp(vgui::ipanel()->GetName(tmppanel), "Progress"))
					iLoadingBarHandle = vgui::ivgui()->PanelToHandle(tmppanel);

				vgui::ipanel()->SetVisible(tmppanel, false); // hide the loading dialog's elements
			}
		}
	}

	if (iLoadingBarHandle != 0) // the loading bar itself
	{
		vgui::ipanel()->SetVisible(vgui::ivgui()->HandleToPanel(iLoadingBarHandle), false);
	}

	return iLoadingBarHandle != 0;
}

//-----------------------------------------------------------------------------
// Purpose: Scans through the vgui/loading materials folder and picks a random
// valid background (if any exist). Called both on activation and if a
// map-specific background can't be found.
//-----------------------------------------------------------------------------
void CMapLoadBG::SelectDefaultBackground(void)
{
#ifdef _WIN32
	char textureName[32];
#else	// !_WIN32
	char textureName[32];
#endif	// _WIN32

	int iValidMaterials = -1;

	for (int i = 0; i < 99; i++) // count the valid vmts up to a max of a hundred of them
	{
		Q_snprintf(textureName, sizeof(textureName), "vgui/loading/default%i", i);

		mapBackground = materials->FindMaterial(textureName, TEXTURE_GROUP_OTHER, false);
		if (mapBackground && !mapBackground->IsErrorMaterial())
		{
			iValidMaterials++;
		}
	}

	if (iValidMaterials > -1)
	{
		Q_snprintf(textureName, sizeof(textureName), "loading/default%i", RandomInt(0, iValidMaterials));
		SetNewBackgroundImage(textureName);

		m_bResetBackground = false;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Based on the loading level name, find a vmt to replace one of the
// default loading screen textures.
//-----------------------------------------------------------------------------
void CMapLoadBG::SelectSpecificBackground(void)
{
#ifdef _WIN32
	char mapFilename[MAX_PATH];
	char textureName[MAX_PATH];
#else	// !_WIN32
	char mapFilename[PATH_MAX];
	char textureName[PATH_MAX];
#endif	// _WIN32

	Q_FileBase(engine->GetLevelName(), mapFilename, sizeof(mapFilename));

	Q_snprintf(textureName, sizeof(textureName), "vgui/loading/%s", mapFilename);

	mapBackground = materials->FindMaterial(textureName, TEXTURE_GROUP_OTHER, false);

	if (mapBackground && !mapBackground->IsErrorMaterial())
	{
		Q_snprintf(textureName, sizeof(textureName), "loading/%s", mapFilename);
		SetNewBackgroundImage(textureName);
		m_bResetBackground = true; // this will matter on next map load when we'll want to re-pick a default background.
	}
}

//-----------------------------------------------------------------------------
// Purpose: Catch the activation message to choose one of random default 
// loading screens (this is guaranteed to happen before we know the name of the
// map we're loading, as that info cannot be made readily available, and we
// may not even have/want a separate texture for it.
//-----------------------------------------------------------------------------
void CMapLoadBG::OnMessage(const KeyValues *params, VPANEL fromPanel)
{
	if (!Q_strcmp(params->GetName(), "Activate"))
	{
		SelectDefaultBackground();
	}

#ifdef ENABLE_LOADING_TIP
	if (m_pLoadingTip)
	{
		char tipText[256];

		Q_snprintf(tipText, sizeof(tipText), "#loadingscreen_Tip_%i", RandomInt(1, 32)); // FIXME: this can be avoided if we can just enum all the valid strings instead of (1, 32) ... but is that possible?
		if (g_pVGuiLocalize->Find(tipText))
		{
			m_pLoadingTip->SetText(tipText);
		}
		else // retry with at least #1 string. Again, if we just enumerated the strings, we wouldn't need this...
		{
			if (g_pVGuiLocalize->Find("#loadingscreen_Tip_1"))
				m_pLoadingTip->SetText("#loadingscreen_Tip_1");
		}

		m_pLoadingTip->SizeToContents();
		m_pLoadingTip->SetPos(ScreenWidth() / 2 - m_pLoadingTip->GetWide() / 2, ScreenHeight() * 0.85f); // FIXME: without it, the labels is misaligned even though it should be a_center?
	}
#endif

	BaseClass::OnMessage(params, fromPanel);
}

void CMapLoadBG::OnThink(void)
{
	if (Q_strcmp(engine->GetLevelName(), "\0"))
	{
		SelectSpecificBackground();
	}
	else
	{
		if (m_bResetBackground)
			SelectDefaultBackground();
	}

	if (FindLoadingDialogBarHandle())
	{
		VPANEL panel = vgui::ivgui()->HandleToPanel(iLoadingBarHandle);
		if (panel)
		{
			ProgressBar *tempBar = dynamic_cast<vgui::ProgressBar*>(vgui::ipanel()->GetPanel(panel, "GAMEUI"));
			if (tempBar)
			{
				progress = tempBar->GetProgress();
			}
			else
			{
				Assert(0);
			}
		}
		else
		{
			Assert(0);
		}
	}

#ifdef ENABLE_CUSTOM_LOADING_BAR
	if (m_pProgressBar)
	{
		m_pProgressBar->SetProgress(progress);

		if (m_pProgressPercentage)
		{
			char percentage[10];
			itoa((int)100 * progress, percentage, 10);
			m_pProgressPercentage->SetText(percentage);
			m_pProgressPercentage->SizeToContents();
		}

//#ifdef HL2MP
//		if (m_pProgressStatusMessage)
//		{
//			char statusText[256];
//			itoa((int)100 * progress, statusText, 10);
//			m_pProgressStatusMessage->SetText(statusText);
//			m_pProgressStatusMessage->SizeToContents();
//			m_pProgressStatusMessage->SetPos(ScreenWidth() / 2 - m_pProgressStatusMessage->GetWide() / 2, ScreenHeight() * 0.95f); // FIXME: without it, the labels is misaligned even though it should be a_center?
//		}
//#endif
	}
#endif
#ifdef ENABLE_CUSTOM_LOADING_WHEEL
	if (m_pProgressWheel)
	{
		m_pProgressWheel->SetProgress(progress);
	}
#endif
	
	BaseClass::OnThink();
}

//-----------------------------------------------------------------------------
// Purpose: Sets a new background on demand
//-----------------------------------------------------------------------------
void CMapLoadBG::SetNewBackgroundImage(char const *imageName)
{
	m_pBackground->SetImage(imageName);
}