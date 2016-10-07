
/** dirt-cheap replacement for CComPtr, which is not included in the free windows compilers */
template< class I >
class AutoComPtr
{
public:
	
	AutoComPtr()
		: p( 0 )
	{}

	AutoComPtr( const AutoComPtr< I >& o )
		: p( o.p )
	{
		if ( p )
			p->AddRef();
	}
	
	AutoComPtr( I* _p )
		: p( _p )
	{}

	~AutoComPtr()
	{ 
		if ( p )
			p->Release();
	}

	void Release()
	{
		if ( p )
			p->Release();
		p = 0;
	}
	
	I* operator->()
	{ return p; }
	
	operator I*()
	{ return p; }
	
	AutoComPtr< I >& operator=( const AutoComPtr< I >& o )
	{ 
		Release(); 
		p = o.p; 
		if ( p ) 
			p->AddRef(); 
		return *this;
	}
	
	template< class O >
	HRESULT QueryInterface( AutoComPtr< O >& o )
	{ return p->QueryInterface( __uuidof( O ), reinterpret_cast< void** >( &o.p ) ); }
	
	HRESULT CoCreateInstance( REFCLSID rclsid, LPUNKNOWN pUnkOuter = 0, DWORD dwClsContext = CLSCTX_INPROC_SERVER )
	{ return ::CoCreateInstance( rclsid, pUnkOuter, dwClsContext, __uuidof( I ), reinterpret_cast< void** >( &p ) ); }
	
	I* p;
};
