#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <msctf.h>
#include <objbase.h>
#include <strsafe.h>

#include "converter.hpp"

#include <algorithm>
#include <atomic>
#include <cwchar>
#include <iterator>
#include <new>
#include <string>
#include <vector>

namespace {
constexpr LANGID kBanglaBangladesh = 0x0845;
const CLSID CLSID_SrabonTextService = {0x6ec5a759,0x5d5b,0x4f28,{0x9e,0x9d,0x2a,0x50,0x11,0x41,0x73,0x20}};
const GUID GUID_SrabonBanglaProfile = {0x3a50fc20,0x2685,0x452a,{0x8a,0xdb,0xd2,0x37,0x1c,0x60,0x75,0x52}};
HMODULE g_module = nullptr;
std::atomic<long> g_objects{0};
std::atomic<long> g_locks{0};

template<class T> void SafeRelease(T*& p) { if (p) { p->Release(); p = nullptr; } }

std::wstring GuidString(REFGUID guid) {
    wchar_t text[40]{};
    StringFromGUID2(guid, text, static_cast<int>(std::size(text)));
    return text;
}

class TextService;

class EditSession final : public ITfEditSession {
public:
    enum class Action { Append, Backspace, Previous, Next, Commit };
    EditSession(TextService* service, ITfContext* context, Action action, wchar_t value = 0);
    ~EditSession();
    STDMETHODIMP QueryInterface(REFIID riid, void** object) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;
    STDMETHODIMP DoEditSession(TfEditCookie cookie) override;
private:
    std::atomic<ULONG> refs_{1};
    TextService* service_;
    ITfContext* context_;
    Action action_;
    wchar_t value_;
};

class TextService final : public ITfTextInputProcessor, public ITfKeyEventSink, public ITfCompositionSink {
public:
    TextService() { ++g_objects; }
    ~TextService() { ResetComposition(); --g_objects; }

    STDMETHODIMP QueryInterface(REFIID riid, void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (riid == IID_IUnknown || riid == IID_ITfTextInputProcessor) *object = static_cast<ITfTextInputProcessor*>(this);
        else if (riid == IID_ITfKeyEventSink) *object = static_cast<ITfKeyEventSink*>(this);
        else if (riid == IID_ITfCompositionSink) *object = static_cast<ITfCompositionSink*>(this);
        if (!*object) return E_NOINTERFACE;
        AddRef();
        return S_OK;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return ++refs_; }
    STDMETHODIMP_(ULONG) Release() override {
        const auto count = --refs_;
        if (!count) delete this;
        return count;
    }

    STDMETHODIMP Activate(ITfThreadMgr* manager, TfClientId clientId) override {
        if (!manager) return E_INVALIDARG;
        m_clientId = clientId;
        ITfKeystrokeMgr* keys = nullptr;
        HRESULT hr = manager->QueryInterface(IID_ITfKeystrokeMgr, reinterpret_cast<void**>(&keys));
        if (FAILED(hr)) return hr;
        hr = keys->AdviseKeyEventSink(clientId, static_cast<ITfKeyEventSink*>(this), TRUE);
        if (SUCCEEDED(hr)) {
            m_managerKeys = keys;
            m_active = true;
        } else {
            keys->Release();
        }
        return hr;
    }
    STDMETHODIMP Deactivate() override {
        if (m_active && m_managerKeys) {
            m_managerKeys->UnadviseKeyEventSink(m_clientId);
            SafeRelease(m_managerKeys);
        }
        m_active = false;
        ResetComposition();
        m_roman.clear();
        return S_OK;
    }

    // ITfKeyEventSink. The TSF manager calls OnTestKeyDown before dispatching a key.
    STDMETHODIMP OnSetFocus(BOOL) override { return S_OK; }
    STDMETHODIMP OnTestKeyDown(ITfContext*, WPARAM key, LPARAM, BOOL* eaten) override {
        if (!eaten) return E_POINTER;
        if ((GetKeyState(VK_CONTROL) & 0x8000) != 0 || (GetKeyState(VK_MENU) & 0x8000) != 0) {
            *eaten = FALSE;
            return S_OK;
        }
        *eaten = IsRomanKey(key) || (HasComposition() && IsCompositionControl(key));
        return S_OK;
    }
    STDMETHODIMP OnTestKeyUp(ITfContext*, WPARAM, LPARAM, BOOL* eaten) override {
        if (!eaten) return E_POINTER;
        *eaten = FALSE;
        return S_OK;
    }
    STDMETHODIMP OnKeyDown(ITfContext* context, WPARAM key, LPARAM, BOOL* eaten) override {
        if (!eaten) return E_POINTER;
        *eaten = FALSE;
        if (!context) return E_INVALIDARG;
        EditSession::Action action;
        wchar_t value = 0;
        if (IsRomanKey(key)) { action = EditSession::Action::Append; value = RomanCharacter(key); }
        else if (key == VK_BACK && HasComposition()) action = EditSession::Action::Backspace;
        else if (key == VK_UP && HasComposition()) action = EditSession::Action::Previous;
        else if (key == VK_DOWN && HasComposition()) action = EditSession::Action::Next;
        else if (HasComposition() && IsCommitKey(key)) { action = EditSession::Action::Commit; value = CommitCharacter(key); }
        else return S_OK;
        const HRESULT hr = RequestEdit(context, action, value);
        *eaten = SUCCEEDED(hr);
        return hr;
    }
    STDMETHODIMP OnKeyUp(ITfContext*, WPARAM, LPARAM, BOOL* eaten) override {
        if (!eaten) return E_POINTER;
        *eaten = FALSE;
        return S_OK;
    }
    STDMETHODIMP OnPreservedKey(ITfContext*, REFGUID, BOOL* eaten) override {
        if (!eaten) return E_POINTER;
        *eaten = FALSE;
        return S_OK;
    }
    STDMETHODIMP OnCompositionTerminated(TfEditCookie, ITfComposition*) override {
        SafeRelease(m_composition);
        m_roman.clear();
        m_candidates.clear();
        m_selected = 0;
        return S_OK;
    }

private:
    friend class EditSession;
    std::atomic<ULONG> refs_{1};
    TfClientId m_clientId{};
    ITfKeystrokeMgr* m_managerKeys{};
    ITfComposition* m_composition{};
    bool m_active{};
    std::wstring m_roman;
    std::vector<std::wstring> m_candidates;
    std::size_t m_selected{};

    bool HasComposition() const { return m_composition != nullptr; }
    static bool IsRomanKey(WPARAM key) { return key >= 'A' && key <= 'Z'; }
    bool IsCompositionControl(WPARAM key) const {
        return key == VK_BACK || key == VK_UP || key == VK_DOWN || IsCommitKey(key);
    }
    static bool IsCommitKey(WPARAM key) {
        return key == VK_SPACE || key == VK_RETURN || (key >= '0' && key <= '9') ||
               key == VK_OEM_COMMA || key == VK_OEM_PERIOD || key == VK_OEM_1 || key == VK_OEM_2 || key == VK_OEM_3 || key == VK_OEM_4 ||
               key == VK_OEM_5 || key == VK_OEM_6 || key == VK_OEM_7 || key == VK_OEM_MINUS || key == VK_OEM_PLUS;
    }
    static wchar_t RomanCharacter(WPARAM key) {
        wchar_t value = static_cast<wchar_t>(key);
        const bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        const bool caps = (GetKeyState(VK_CAPITAL) & 1) != 0;
        if (shift != caps) value = static_cast<wchar_t>(value - L'A' + L'a');
        return value;
    }
    static wchar_t CommitCharacter(WPARAM key) {
        if (key == VK_SPACE) return L' ';
        if (key == VK_RETURN) return L'\n';
        if (key >= '0' && key <= '9') {
            constexpr wchar_t shiftedDigits[] = L")!@#$%^&*(";
            return (GetKeyState(VK_SHIFT) & 0x8000) ? shiftedDigits[key - '0'] : static_cast<wchar_t>(key);
        }
        if (key == VK_OEM_COMMA) return (GetKeyState(VK_SHIFT) & 0x8000) ? L'<' : L',';
        if (key == VK_OEM_PERIOD) return (GetKeyState(VK_SHIFT) & 0x8000) ? L'>' : L'.';
        if (key == VK_OEM_1) return (GetKeyState(VK_SHIFT) & 0x8000) ? L':' : L';';
        if (key == VK_OEM_2) return (GetKeyState(VK_SHIFT) & 0x8000) ? L'?' : L'/';
        if (key == VK_OEM_3) return (GetKeyState(VK_SHIFT) & 0x8000) ? L'~' : L'`';
        if (key == VK_OEM_4) return (GetKeyState(VK_SHIFT) & 0x8000) ? L'{' : L'[';
        if (key == VK_OEM_5) return (GetKeyState(VK_SHIFT) & 0x8000) ? L'|' : L'\\';
        if (key == VK_OEM_6) return (GetKeyState(VK_SHIFT) & 0x8000) ? L'}' : L']';
        if (key == VK_OEM_7) return (GetKeyState(VK_SHIFT) & 0x8000) ? L'"' : L'\'';
        if (key == VK_OEM_MINUS) return (GetKeyState(VK_SHIFT) & 0x8000) ? L'_' : L'-';
        if (key == VK_OEM_PLUS) return (GetKeyState(VK_SHIFT) & 0x8000) ? L'+' : L'=';
        return 0;
    }
    HRESULT RequestEdit(ITfContext* context, EditSession::Action action, wchar_t value) {
        auto* session = new (std::nothrow) EditSession(this, context, action, value);
        if (!session) return E_OUTOFMEMORY;
        HRESULT sessionResult = E_FAIL;
        const HRESULT hr = context->RequestEditSession(m_clientId, session, TF_ES_SYNC | TF_ES_READWRITE, &sessionResult);
        session->Release();
        return FAILED(hr) ? hr : sessionResult;
    }
    HRESULT Apply(ITfContext* context, TfEditCookie cookie, EditSession::Action action, wchar_t value) {
        if (action == EditSession::Action::Append) m_roman.push_back(value);
        else if (action == EditSession::Action::Backspace) {
            if (!m_roman.empty()) m_roman.pop_back();
            if (m_roman.empty()) {
                if (m_composition) {
                    ITfRange* range = nullptr;
                    if (SUCCEEDED(m_composition->GetRange(&range))) {
                        range->SetText(cookie, 0, L"", 0);
                        range->Release();
                    }
                    m_composition->EndComposition(cookie);
                    SafeRelease(m_composition);
                }
                m_candidates.clear();
                m_selected = 0;
                return S_OK;
            }
        } else if (action == EditSession::Action::Previous || action == EditSession::Action::Next) {
            if (m_candidates.size() > 1) {
                const auto n = m_candidates.size();
                m_selected = action == EditSession::Action::Previous ? (m_selected + n - 1) % n : (m_selected + 1) % n;
            }
        }

        if (action == EditSession::Action::Append || action == EditSession::Action::Backspace) {
            m_candidates = srabon::Candidates(m_roman, 8);
            m_selected = 0;
        }
        HRESULT hr = UpdateComposition(context, cookie);
        if (FAILED(hr)) return hr;
        if (action == EditSession::Action::Commit) {
            if (m_composition) {
                hr = m_composition->EndComposition(cookie);
                SafeRelease(m_composition);
                if (FAILED(hr)) return hr;
            }
            m_roman.clear();
            m_candidates.clear();
            m_selected = 0;
            if (value != 0) {
                ITfInsertAtSelection* inserter = nullptr;
                hr = context->QueryInterface(IID_ITfInsertAtSelection, reinterpret_cast<void**>(&inserter));
                if (SUCCEEDED(hr)) {
                    const wchar_t text[] = {value};
                    const wchar_t newline[] = L"\r\n";
                    const wchar_t* insertText = value == L'\n' ? newline : text;
                    const LONG count = value == L'\n' ? 2 : 1;
                    hr = inserter->InsertTextAtSelection(cookie, TF_IAS_NOQUERY, insertText, count, nullptr);
                    inserter->Release();
                }
            }
        }
        return hr;
    }
    HRESULT UpdateComposition(ITfContext* context, TfEditCookie cookie) {
        if (m_candidates.empty()) return S_OK;
        const auto& text = m_candidates[std::min(m_selected, m_candidates.size() - 1)];
        if (m_composition) {
            ITfRange* range = nullptr;
            HRESULT hr = m_composition->GetRange(&range);
            if (SUCCEEDED(hr)) hr = range->SetText(cookie, 0, text.c_str(), static_cast<LONG>(text.size()));
            SafeRelease(range);
            return hr;
        }
        ITfInsertAtSelection* inserter = nullptr;
        HRESULT hr = context->QueryInterface(IID_ITfInsertAtSelection, reinterpret_cast<void**>(&inserter));
        if (FAILED(hr)) return hr;
        ITfRange* range = nullptr;
        hr = inserter->InsertTextAtSelection(cookie, TF_IAS_NO_DEFAULT_COMPOSITION, text.c_str(), static_cast<LONG>(text.size()), &range);
        inserter->Release();
        if (SUCCEEDED(hr)) {
            ITfContextComposition* composer = nullptr;
            hr = context->QueryInterface(IID_ITfContextComposition, reinterpret_cast<void**>(&composer));
            if (SUCCEEDED(hr)) {
                hr = composer->StartComposition(cookie, range, static_cast<ITfCompositionSink*>(this), &m_composition);
                composer->Release();
            }
        }
        SafeRelease(range);
        return hr;
    }
    void ResetComposition() {
        if (m_composition) {
            // TSF owns termination during context teardown. Release our reference only.
            SafeRelease(m_composition);
        }
        m_roman.clear();
        m_candidates.clear();
        m_selected = 0;
    }
};

EditSession::EditSession(TextService* service, ITfContext* context, Action action, wchar_t value)
    : service_(service), context_(context), action_(action), value_(value) {
    service_->AddRef();
    context_->AddRef();
}
STDMETHODIMP EditSession::QueryInterface(REFIID riid, void** object) {
    if (!object) return E_POINTER;
    *object = nullptr;
    if (riid == IID_IUnknown || riid == IID_ITfEditSession) *object = static_cast<ITfEditSession*>(this);
    if (!*object) return E_NOINTERFACE;
    AddRef();
    return S_OK;
}
STDMETHODIMP_(ULONG) EditSession::AddRef() { return ++refs_; }
STDMETHODIMP_(ULONG) EditSession::Release() {
    const auto count = --refs_;
    if (!count) delete this;
    return count;
}
STDMETHODIMP EditSession::DoEditSession(TfEditCookie cookie) { return service_->Apply(context_, cookie, action_, value_); }
EditSession::~EditSession() { context_->Release(); service_->Release(); }

class ClassFactory final : public IClassFactory {
public:
    ClassFactory() { ++g_objects; }
    ~ClassFactory() { --g_objects; }
    STDMETHODIMP QueryInterface(REFIID riid, void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (riid == IID_IUnknown || riid == IID_IClassFactory) *object = static_cast<IClassFactory*>(this);
        if (!*object) return E_NOINTERFACE;
        AddRef(); return S_OK;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return ++refs_; }
    STDMETHODIMP_(ULONG) Release() override { auto n = --refs_; if (!n) delete this; return n; }
    STDMETHODIMP CreateInstance(IUnknown* outer, REFIID riid, void** object) override {
        if (outer) return CLASS_E_NOAGGREGATION;
        auto* service = new (std::nothrow) TextService();
        if (!service) return E_OUTOFMEMORY;
        const HRESULT hr = service->QueryInterface(riid, object);
        service->Release();
        return hr;
    }
    STDMETHODIMP LockServer(BOOL lock) override { if (lock) ++g_locks; else --g_locks; return S_OK; }
private:
    std::atomic<ULONG> refs_{1};
};

HRESULT WriteComRegistration(bool install) {
    const auto clsid = GuidString(CLSID_SrabonTextService);
    const std::wstring path = L"Software\\Classes\\CLSID\\" + clsid + L"\\InprocServer32";
    if (!install) {
        const std::wstring parent = L"Software\\Classes\\CLSID\\" + clsid;
        const auto status = RegDeleteTreeW(HKEY_CURRENT_USER, parent.c_str());
        return status == ERROR_SUCCESS || status == ERROR_FILE_NOT_FOUND ? S_OK : HRESULT_FROM_WIN32(status);
    }
    wchar_t modulePath[MAX_PATH]{};
    if (!GetModuleFileNameW(g_module, modulePath, MAX_PATH)) return HRESULT_FROM_WIN32(GetLastError());
    HKEY key{};
    LONG status = RegCreateKeyExW(HKEY_CURRENT_USER, path.c_str(), 0, nullptr, 0, KEY_SET_VALUE, nullptr, &key, nullptr);
    if (status != ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);
    status = RegSetValueExW(key, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(modulePath), static_cast<DWORD>((wcslen(modulePath) + 1) * sizeof(wchar_t)));
    if (status == ERROR_SUCCESS) {
        constexpr wchar_t threading[] = L"Apartment";
        status = RegSetValueExW(key, L"ThreadingModel", 0, REG_SZ, reinterpret_cast<const BYTE*>(threading), sizeof(threading));
    }
    RegCloseKey(key);
    return status == ERROR_SUCCESS ? S_OK : HRESULT_FROM_WIN32(status);
}

using InstallLayoutOrTipFn = BOOL (WINAPI*)(LPCWSTR, DWORD);
BOOL SetLayoutTip(bool install) {
    wchar_t systemDir[MAX_PATH]{};
    if (!GetSystemDirectoryW(systemDir, MAX_PATH)) return FALSE;
    std::wstring dllPath = systemDir;
    dllPath += L"\\input.dll";
    HMODULE input = LoadLibraryW(dllPath.c_str());
    if (!input) return FALSE;
    auto fn = reinterpret_cast<InstallLayoutOrTipFn>(GetProcAddress(input, "InstallLayoutOrTip"));
    wchar_t profile[100]{};
    const auto clsid = GuidString(CLSID_SrabonTextService);
    const auto guid = GuidString(GUID_SrabonBanglaProfile);
    StringCchPrintfW(profile, std::size(profile), L"0845:%s%s", clsid.c_str(), guid.c_str());
    const BOOL result = fn ? fn(profile, install ? 0 : 0x00000001) : FALSE;
    FreeLibrary(input);
    return result;
}

HRESULT UnregisterTip();

HRESULT RegisterTip() {
    HRESULT hr = WriteComRegistration(true);
    if (FAILED(hr)) return hr;
    ITfCategoryMgr* category = nullptr;
    hr = CoCreateInstance(CLSID_TF_CategoryMgr, nullptr, CLSCTX_INPROC_SERVER, IID_ITfCategoryMgr, reinterpret_cast<void**>(&category));
    if (SUCCEEDED(hr)) {
        hr = category->RegisterCategory(CLSID_SrabonTextService, GUID_TFCAT_TIP_KEYBOARD, CLSID_SrabonTextService);
        category->Release();
    }
    if (FAILED(hr)) { UnregisterTip(); return hr; }
    ITfInputProcessorProfileMgr* profiles = nullptr;
    hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, nullptr, CLSCTX_INPROC_SERVER, IID_ITfInputProcessorProfileMgr, reinterpret_cast<void**>(&profiles));
    if (SUCCEEDED(hr)) {
        constexpr wchar_t description[] = L"Srabon Bangla Phonetic";
        hr = profiles->RegisterProfile(CLSID_SrabonTextService, kBanglaBangladesh, GUID_SrabonBanglaProfile,
            description, static_cast<ULONG>(std::size(description) - 1), nullptr, 0, 0, nullptr, 0, TRUE, 0);
        profiles->Release();
    }
    if (FAILED(hr)) { UnregisterTip(); return hr; }
    if (!SetLayoutTip(true)) { UnregisterTip(); return E_FAIL; }
    return S_OK;
}

HRESULT UnregisterTip() {
    SetLayoutTip(false);
    ITfInputProcessorProfiles* profiles = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, nullptr, CLSCTX_INPROC_SERVER, IID_ITfInputProcessorProfiles, reinterpret_cast<void**>(&profiles));
    if (SUCCEEDED(hr)) { hr = profiles->Unregister(CLSID_SrabonTextService); profiles->Release(); }
    ITfCategoryMgr* category = nullptr;
    HRESULT categoryHr = CoCreateInstance(CLSID_TF_CategoryMgr, nullptr, CLSCTX_INPROC_SERVER, IID_ITfCategoryMgr, reinterpret_cast<void**>(&category));
    if (SUCCEEDED(categoryHr)) {
        categoryHr = category->UnregisterCategory(CLSID_SrabonTextService, GUID_TFCAT_TIP_KEYBOARD, CLSID_SrabonTextService);
        category->Release();
    }
    const HRESULT registryHr = WriteComRegistration(false);
    return FAILED(registryHr) ? registryHr : (FAILED(hr) && hr != REGDB_E_CLASSNOTREG ? hr : S_OK);
}
} // namespace

extern "C" BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) { g_module = instance; DisableThreadLibraryCalls(instance); }
    return TRUE;
}

extern "C" HRESULT WINAPI SrabonDllGetClassObject(REFCLSID clsid, REFIID riid, void** object) {
    if (clsid != CLSID_SrabonTextService) return CLASS_E_CLASSNOTAVAILABLE;
    auto* factory = new (std::nothrow) ClassFactory();
    if (!factory) return E_OUTOFMEMORY;
    const HRESULT hr = factory->QueryInterface(riid, object);
    factory->Release();
    return hr;
}
extern "C" HRESULT WINAPI SrabonDllCanUnloadNow() { return g_objects == 0 && g_locks == 0 ? S_OK : S_FALSE; }
extern "C" HRESULT WINAPI SrabonDllRegisterServer() {
    const HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const HRESULT hr = RegisterTip();
    if (SUCCEEDED(init)) CoUninitialize();
    return hr;
}
extern "C" HRESULT WINAPI SrabonDllUnregisterServer() {
    const HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const HRESULT hr = UnregisterTip();
    if (SUCCEEDED(init)) CoUninitialize();
    return hr;
}

// The keystroke sink keeps the TSF manager alive through the active processor.
// Obtain the manager's keystroke interface once after activation.
