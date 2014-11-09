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

	default:
		break;
	}

    return S_OK;
}
