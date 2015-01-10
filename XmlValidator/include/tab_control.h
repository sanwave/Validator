/*
*
*
*
*	Tab Control Class		In Matrix
*
*	Created by Bonbon	2014.11.19
*
*	Updated by Bonbon	2014.11.19
*
*/


#ifndef _TAB_CONTROL_H_
#define _TAB_CONTROL_H_

#include "common.h"
#include <CommCtrl.h>

class TabControl
{
public:
    TabControl():m_hwnd(NULL)
    {}
    TabControl(const TabControl&){}
    ~TabControl(){}

    int Init(HINSTANCE hinst, HWND parent)
    {
        m_parent = parent;

        int style = WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE |\
            TCS_TOOLTIPS | TCS_FOCUSNEVER | TCS_TABS;

        style |= TCS_OWNERDRAWFIXED;

        m_hwnd = ::CreateWindowEx(
            0,
            WC_TABCONTROL,
            TEXT("Tab"),
            style,
            0, 0, 0, 0,
            m_parent,
            NULL,
            hinst,
            0);
    }

private:
    HWND m_hwnd;
    HWND m_parent;
};

#endif
