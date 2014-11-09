// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#include "command_handler.h"

// Static method to create an instance of the object.
HRESULT CCommandHandler::CreateInstance(IUICommandHandler **ppCommandHandler)
{
    if (!ppCommandHandler)
    {
        return E_POINTER;
    }

    *ppCommandHandler = NULL;

    HRESULT hr = S_OK;
   
    CCommandHandler* pCommandHandler = new CCommandHandler();

    if (pCommandHandler != NULL)
    {
        *ppCommandHandler = static_cast<IUICommandHandler *>(pCommandHandler);
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

// IUnknown method implementations.
STDMETHODIMP_(ULONG) CCommandHandler::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CCommandHandler::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
        delete this;
    }

    return cRef;
}

STDMETHODIMP CCommandHandler::QueryInterface(REFIID iid, void** ppv)
{
    if (iid == __uuidof(IUnknown))
    {
        *ppv = static_cast<IUnknown*>(this);
    }
    else if (iid == __uuidof(IUICommandHandler))
    {
        *ppv = static_cast<IUICommandHandler*>(this);
    }
    else 
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

//
//  FUNCTION: UpdateProperty()
//
//  PURPOSE: Called by the Ribbon framework when a command property (PKEY) needs to be updated.
//
//  COMMENTS:
//
//    This function is used to provide new command property values, such as labels, icons, or
//    tooltip information, when requested by the Ribbon framework.  
//    
//    In this SimpleRibbon sample, the method is not implemented.  
//
STDMETHODIMP CCommandHandler::UpdateProperty(
    UINT nCmdID,
    REFPROPERTYKEY key,
    const PROPVARIANT* ppropvarCurrentValue,
    PROPVARIANT* ppropvarNewValue)
{
    UNREFERENCED_PARAMETER(nCmdID);
    UNREFERENCED_PARAMETER(key);
    UNREFERENCED_PARAMETER(ppropvarCurrentValue);
    UNREFERENCED_PARAMETER(ppropvarNewValue);

    return E_NOTIMPL;
}

//
//  FUNCTION: Execute()
//
//  PURPOSE: Called by the Ribbon framework when a command is executed by the user.  For example, when
//           a button is pressed.
//
STDMETHODIMP CCommandHandler::Execute(
    UINT nCmdID,
    UI_EXECUTIONVERB verb,
    const PROPERTYKEY* key,
    const PROPVARIANT* ppropvarValue,
    IUISimplePropertySet* pCommandExecutionProperties)
{
    UNREFERENCED_PARAMETER(pCommandExecutionProperties);
    UNREFERENCED_PARAMETER(ppropvarValue);
    UNREFERENCED_PARAMETER(key);
    UNREFERENCED_PARAMETER(verb);
    UNREFERENCED_PARAMETER(nCmdID);

	
	bool checked = true;
	switch (nCmdID)
	{

	case IDR_NEW:
		MainFrame::Editor->New();
		break;

	case IDR_OPEN:
		MainFrame::Editor->OpenFileDlg(MainFrame::Win);
		break;

	case IDR_SAVE:
		MainFrame::Editor->Save();
		break;

	case IDR_SAVEAS:
		MainFrame::Editor->SaveAs();
		break;

	case IDR_PRINT:
		break;

	case IDR_FIND:
		MainFrame::Editor->Search(NULL);
		break;

	case IDR_WRAP:
		//checked = MF_CHECKED == GetMenuState(h_menu, IDR_WRAP, MF_BYCOMMAND);
		//editor->SetWrap(!checked);
		//CheckMenuItem(h_menu, IDR_WRAP, (!checked ? MF_CHECKED : MF_UNCHECKED));
		break;

	case IDR_VALIDATE:
		MainFrame::Editor->ValidateXml(NULL);
		break;

	case IDR_AUTOVALIDATE:
		//m_auto_validate = MF_CHECKED != GetMenuState(h_menu, IDR_AUTOVALIDATE, MF_BYCOMMAND);
		//CheckMenuItem(h_menu, IDM_AUTOVALIDATE, m_auto_validate ? MF_CHECKED : MF_UNCHECKED);
		break;
		
	case IDR_EXIT:
		MainFrame::DestroyFramework();
		PostQuitMessage(0);
		break;

	case IDR_ABOUT:
		//DialogBox(nullptr, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		break;

	/*case IDR_MINIMIZE:
		if (g_pFramework)
		{
			IPropertyStore *pPropertyStore = NULL;
			/*HRESULT hr = g_pApplication->QueryInterface(__uuidof(IPropertyStore), (void**)&pPropertyStore);
			if (SUCCEEDED(hr))
			{
				if (ppropvarValue != NULL)
				{
					// Is the ToggleButton state on or off?
					BOOL fToggled;
					hr = UIPropertyToBoolean(*key, *ppropvarValue, &fToggled);

					if (SUCCEEDED(hr))
					{
						// Set the ribbon display state based on the toggle state.
						PROPVARIANT propvar;
						PropVariantInit(&propvar);
						UIInitPropertyFromBoolean(UI_PKEY_Minimized,
							fToggled,
							&propvar);
						hr = pPropertyStore->SetValue(UI_PKEY_Minimized,
							propvar);
						pPropertyStore->Commit();
					}
					pPropertyStore->Release();
				}
			}
		}
		break;*/

	case IDR_SWITCH_BACKCOLOR:
		if (g_pFramework)
		{
			IPropertyStore *pPropertyStore = NULL;
			HRESULT hr = g_pApplication->QueryInterface(__uuidof(IPropertyStore), (void**)&pPropertyStore);
			if (SUCCEEDED(hr))
			{
				PROPVARIANT propvarBackground;
				PROPVARIANT propvarHighlight;
				PROPVARIANT propvarText;

				// UI_HSBCOLOR is a type defined in UIRibbon.h that is composed of 
				// three component values: hue, saturation and brightness, respectively.
				UI_HSBCOLOR BackgroundColor = UI_HSB(0x14, 0x38, 0x54);
				UI_HSBCOLOR HighlightColor = UI_HSB(0x00, 0x36, 0x87);
				UI_HSBCOLOR TextColor = UI_HSB(0x2B, 0xD6, 0x00);

				InitPropVariantFromUInt32(BackgroundColor, &propvarBackground);
				InitPropVariantFromUInt32(HighlightColor, &propvarHighlight);
				InitPropVariantFromUInt32(TextColor, &propvarText);

				pPropertyStore->SetValue(UI_PKEY_GlobalBackgroundColor, propvarBackground);
				pPropertyStore->SetValue(UI_PKEY_GlobalTextColor, propvarText);

				pPropertyStore->Commit();
				pPropertyStore->Release();
			}
		}
		break;



	default:
		break;
	}

    return S_OK;
}
