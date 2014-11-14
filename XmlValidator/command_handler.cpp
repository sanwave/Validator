

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

	HRESULT hr = E_FAIL;

	switch (nCmdID)
	{

	case IDR_NEW:
        Matrix::SciEditor::CurrentPtr()->New();
		break;

	case IDR_OPEN:
        Matrix::SciEditor::CurrentPtr()->OpenFileDlg(MainFrame::Win);
		break;

	case IDR_SAVE:
        Matrix::SciEditor::CurrentPtr()->Save();
		break;

	case IDR_SAVEAS:
        Matrix::SciEditor::CurrentPtr()->SaveAs();
		break;

	case IDR_PRINT:
		break;

	case IDR_FIND:
        Matrix::SciEditor::CurrentPtr()->Search(NULL);
		break;

	case IDR_WRAP:
		hr = g_pFramework->InvalidateUICommand(IDR_WRAP, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_BooleanValue);
		if (FAILED(hr))
		{
			return hr;
		}
		break;

	case IDR_VALIDATE:
        Matrix::SciEditor::CurrentPtr()->ValidateXml();
		break;

	case IDR_AUTOVALIDATE:
		hr = g_pFramework->InvalidateUICommand(IDR_AUTOVALIDATE, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_BooleanValue);
		if (FAILED(hr))
		{
			return hr;
		}
		break;
		
	case IDR_EXIT:
		PostMessage(MainFrame::Win, WM_CLOSE, NULL, NULL);
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
		break;

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
		break;*/

	default:
		break;
	}

    return S_OK;
}


STDMETHODIMP CCommandHandler::UpdateProperty(
	UINT nCmdID,
	__in REFPROPERTYKEY key,
	__in_opt const PROPVARIANT* ppropvarCurrentValue,
	__out PROPVARIANT* ppropvarNewValue)
{
	UNREFERENCED_PARAMETER(nCmdID);
	UNREFERENCED_PARAMETER(key);
	UNREFERENCED_PARAMETER(ppropvarCurrentValue);
	UNREFERENCED_PARAMETER(ppropvarNewValue);

	HRESULT hr = E_FAIL;
	PROPVARIANT var;

	if (key == UI_PKEY_BooleanValue)
	{		
		switch (nCmdID)
		{
		case IDR_WRAP:
            if (NULL != Matrix::SciEditor::CurrentPtr())
			{
				hr = g_pFramework->GetUICommandProperty(IDR_WRAP, UI_PKEY_BooleanValue, &var);
				if (FAILED(hr))
				{
					return hr;
				}
                Matrix::SciEditor::CurrentPtr()->SetWrap(var.boolVal);
			}
			break;

		case IDR_AUTOVALIDATE:
            if (NULL != Matrix::SciEditor::CurrentPtr())
			{
				hr = g_pFramework->GetUICommandProperty(IDR_AUTOVALIDATE, UI_PKEY_BooleanValue, &var);
				if (FAILED(hr))
				{
					return hr;
				}
                Matrix::SciEditor::CurrentPtr()->SetAutoValidate(var.boolVal);
			}
			break;

		default:
			break;
		}
	}

	return E_NOTIMPL;
}