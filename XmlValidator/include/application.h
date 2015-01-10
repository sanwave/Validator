
#pragma once

#include <UiRibbon.h>
#include <UiRibbonPropertyHelpers.h>

#include "command_handler.h"


class CApplication
    : public IUIApplication // Applications must implement IUIApplication.
{
public:

    // Static method to create an instance of the object.
    static HRESULT CreateInstance(IUIApplication **ppApplication);

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID iid, void** ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IUIApplication methods
    STDMETHOD(OnCreateUICommand)(UINT nCmdID,
        UI_COMMANDTYPE typeID,
        IUICommandHandler** ppCommandHandler);

    STDMETHOD(OnViewChanged)(UINT viewId,
        UI_VIEWTYPE typeId,
        IUnknown* pView,
        UI_VIEWVERB verb,
        INT uReasonCode);

    STDMETHOD(OnDestroyUICommand)(UINT32 commandId, 
        UI_COMMANDTYPE typeID,
		IUICommandHandler* commandHandler);

	static inline UINT RibbonHeight()
	{
		return m_ribbon_height;
	}

private:
    CApplication() 
        : m_cRef(1)
        , m_pCommandHandler(NULL)
    {
		m_ribbon_height = 0;
    }

    ~CApplication() 
    {
        if (m_pCommandHandler)
        {
            m_pCommandHandler->Release();
            m_pCommandHandler = NULL;
        }
    }

    LONG m_cRef;                            // Reference count.
    IUICommandHandler * m_pCommandHandler;  // Generic Command Handler
	static UINT m_ribbon_height;
};
