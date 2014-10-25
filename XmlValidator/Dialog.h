
/*
*
*
*
*	Dialog Class		In Matrix
*
*	Created by Bonbon	2014.09.27
*
*/


#ifndef _DIALOG_H_
#define _DIALOG_H_

#include "common.h"
#include <shobjidl.h> 

//template <class DERIVED_TYPE>
class FileDialog
{
public:
	FileDialog(){}
	
	LPCWSTR FileDialog::Open()
	{
		LPWSTR fileName = NULL;
		IFileOpenDialog *pFileOpen;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
		if (SUCCEEDED(hr))
		{
			hr = pFileOpen->Show(NULL);

			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					if (SUCCEEDED(hr))
					{
						fileName = pszFilePath;
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();

		return fileName;
	}

private:
};


#endif