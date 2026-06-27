// classfactory.cpp — IClassFactory for CCrimsonContextMenu.

#include "shellext.h"
#include <new>          // std::nothrow

// Forward decl from contextmenu.cpp.
extern "C" HRESULT CreateCrimsonContextMenu(REFIID riid, void** ppv);

class CClassFactory : public IClassFactory
{
public:
	CClassFactory() : m_cRef(1) { InterlockedIncrement(&g_cRefDll); }

	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override
	{
		if (ppv == NULL) return E_POINTER;
		if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory)) {
			*ppv = static_cast<IClassFactory*>(this);
			AddRef();
			return S_OK;
		}
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	IFACEMETHODIMP_(ULONG) AddRef() override
	{
		return InterlockedIncrement(&m_cRef);
	}
	IFACEMETHODIMP_(ULONG) Release() override
	{
		ULONG c = InterlockedDecrement(&m_cRef);
		if (c == 0) delete this;
		return c;
	}

	// IClassFactory
	IFACEMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv) override
	{
		if (ppv == NULL) return E_POINTER;
		*ppv = NULL;
		if (pUnkOuter != NULL) return CLASS_E_NOAGGREGATION;
		return CreateCrimsonContextMenu(riid, ppv);
	}
	IFACEMETHODIMP LockServer(BOOL fLock) override
	{
		if (fLock) InterlockedIncrement(&g_cLockDll);
		else       InterlockedDecrement(&g_cLockDll);
		return S_OK;
	}

private:
	~CClassFactory() { InterlockedDecrement(&g_cRefDll); }
	LONG m_cRef;
};

extern "C" HRESULT CreateCrimsonClassFactory(REFIID riid, void** ppv)
{
	CClassFactory* p = new (std::nothrow) CClassFactory();
	if (!p) return E_OUTOFMEMORY;
	HRESULT hr = p->QueryInterface(riid, ppv);
	p->Release();
	return hr;
}
