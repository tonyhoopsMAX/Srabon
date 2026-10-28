#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <msctf.h>
#include <objbase.h>

#include <cstdio>

namespace {
constexpr LANGID kBanglaBangladesh = 0x0845;
const CLSID CLSID_SrabonTextService = {0x6ec5a759,0x5d5b,0x4f28,{0x9e,0x9d,0x2a,0x50,0x11,0x41,0x73,0x20}};
const GUID GUID_SrabonBanglaProfile = {0x3a50fc20,0x2685,0x452a,{0x8a,0xdb,0xd2,0x37,0x1c,0x60,0x75,0x52}};
}

int wmain() {
    const HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(init)) { std::fwprintf(stderr, L"COM initialization failed: 0x%08lx\n", init); return 1; }
    ITfInputProcessorProfileMgr* profiles = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_TF_InputProcessorProfileMgr, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_ITfInputProcessorProfileMgr, reinterpret_cast<void**>(&profiles));
    if (SUCCEEDED(hr)) {
        hr = profiles->ActivateProfile(TF_PROFILETYPE_INPUTPROCESSOR, kBanglaBangladesh,
            CLSID_SrabonTextService, GUID_SrabonBanglaProfile, nullptr,
            TF_IPPMF_FORSESSION | TF_IPPMF_ENABLEPROFILE | TF_IPPMF_DONTCARECURRENTINPUTLANGUAGE);
        profiles->Release();
    }
    CoUninitialize();
    if (FAILED(hr)) { std::fwprintf(stderr, L"Could not activate Srabon TSF profile: 0x%08lx\n", hr); return 2; }
    std::wprintf(L"Srabon Bangla profile activated for the current desktop session.\n");
    return 0;
}
