#include <iostream>
#include <Windows.h>
#include <shlobj.h>

using namespace std;


int CreateLnk(const wchar_t* TARGET, const wchar_t* LNKFILE)
{
	if (S_OK != CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))
		return 1;
	IShellLinkW* psl;
	HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&psl));
	if (SUCCEEDED(hr))
	{
		psl->SetPath(TARGET);

		IPersistFile* ppf;

		hr = psl->QueryInterface(&ppf);
		if (SUCCEEDED(hr))
		{
			hr = ppf->Save(LNKFILE, TRUE);
			ppf->Release();
			if (!SUCCEEDED(hr))
				return 2;
		}
		else
		{
			psl->Release();
			return 3;
		}
	}
	else
		return 4;
	CoUninitialize();
	return 0;
}


int ResolveLnk(wchar_t* TARGET, const wchar_t* LNKFILE)
{
	if (CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE) != S_OK)
	{
		//初始化COM库失败
		return 5; 
	}
	IShellLinkW* psl;
	HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&psl));
	if (SUCCEEDED(hr))
	{
		IPersistFile* ppf;
		hr = psl->QueryInterface(&ppf);
		if (SUCCEEDED(hr))
		{
			hr = ppf->Load(LNKFILE, STGM_READ);
			if (SUCCEEDED(hr))
			{
				//CHAR sz_args[MAX_PATH];
				hr = psl->GetPath(TARGET, MAX_PATH, NULL, SLGP_RAWPATH);
				if (SUCCEEDED(hr))
				{
					//wcout << L"Link to: " << TARGET << endl;
					CoUninitialize();
					return 0;
				}
				else
				{
					cout << "Get Link to failed" << endl;
					CoUninitialize();
					return 1;
				}
			}
			else
			{
				cout << "Open file failed" << endl;
				CoUninitialize();
				return 2;
			}
			ppf->Release();
		}
		else
		{
			cout << "System Error When read file" << endl;
			CoUninitialize();
			return 3;
		}
		psl->Release();
	}
	else
	{
		cout << "Operation failure" << endl;
		CoUninitialize();
		return 4;
	}
}


int main()
{
	wcout.imbue(std::locale("chs"));  //wcout显示中文
    cout << "Hello World!\n";

	wchar_t CurrectPath[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, CurrectPath, MAX_PATH);
	*(wcsrchr(CurrectPath, L'\\') + 1) = L'\0';
	wcscat_s(CurrectPath, L"test.lnk");

	cout << "生成快捷方式：" << ((0 == CreateLnk(L"C:\\Windows\\system32\\calc.exe", CurrectPath)) ? "成功" : "失败") << endl;

	wchar_t TargetPath[MAX_PATH] = { 0 };
	int ResolveCode = 0;
	cout << "解析快捷方式：" << ((0 == (ResolveCode = ResolveLnk(TargetPath, CurrectPath))) ? "成功" : "失败") << endl;
	if (0 == ResolveCode)
		wcout << L"指向的文件：" << TargetPath << endl;

	return 0;
}

