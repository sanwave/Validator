
#pragma once

#include <UiRibbon.h>
#include <UiRibbonPropertyHelpers.h>

#include "ribbon_framework.h"
#include "ribbon_ui.h"


class CCommandHandler
    : public IUICommandHandler // Command handlers must implement IUICommandHandler.
{
public:

    // Static method to create an instance of the object.
    static HRESULT CreateInstance(IUICommandHandler **ppCommandHandler);

    // IUnknown methods.
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv);

    // IUICommandHandler methods
    STDMETHOD(UpdateProperty)(UINT nCmdID,
        REFPROPERTYKEY key,
        const PROPVARIANT* ppropvarCurrentValue,
        PROPVARIANT* ppropvarNewValue);

    STDMETHOD(Execute)(UINT nCmdID,
        UI_EXECUTIONVERB verb, 
        const PROPERTYKEY* key,
        const PROPVARIANT* ppropvarValue,
        IUISimplePropertySet* pCommandExecutionProperties);

private:
    CCommandHandler()
        : m_cRef(1) 
    {
    }

    LONG m_cRef;                        // Reference count.
};
