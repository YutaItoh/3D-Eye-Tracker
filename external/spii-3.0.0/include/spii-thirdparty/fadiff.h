// Copyright (C) 1996-2007 Ole Stauning & Claus Bendtsen (fadbad@uning.dk)
// All rights reserved.

// This code is provided "as is", without any warranty of any kind,
// either expressed or implied, including but not limited to, any implied
// warranty of merchantibility or fitness for any purpose. In no event
// will any party who distributed the code be liable for damages or for
// any claim(s) by any other party, including but not limited to, any
// lost profits, lost monies, lost data or data rendered inaccurate,
// losses sustained by third parties, or any other special, incidental or
// consequential damages arising out of the use or inability to use the
// program, even if the possibility of such damages has been advised
// against. The entire risk as to the quality, the performance, and the
// fitness of the program for any particular purpose lies with the party
// using the code.

// This code, and any derivative of this code, may not be used in a
// commercial package without the prior explicit written permission of
// the authors. Verbatim copies of this code may be made and distributed
// in any medium, provided that this copyright notice is not removed or
// altered in any way. No fees may be charged for distribution of the
// codes, other than a fee to cover the cost of the media and a
// reasonable handling fee.

// ***************************************************************
// ANY USE OF THIS CODE CONSTITUTES ACCEPTANCE OF THE TERMS OF THE
//                         COPYRIGHT NOTICE
// ***************************************************************

#ifndef _FADIFF_H
#define _FADIFF_H

#include "fadbad.h"

namespace fadbad
{

template <typename T, unsigned int N=0>
class FTypeName // STACK-BASED
{
	T m_val;
	T m_diff[N];
	bool m_depend;
public:
	typedef T UnderlyingType;
	FTypeName():m_depend(false){}
	FTypeName(const FTypeName<T,N>& val):m_val(val.m_val),m_depend(val.m_depend)
	{
		if (m_depend) for(unsigned int i=0;i<N;++i) m_diff[i]=val.m_diff[i];
	}
	template <class U> /*explicit*/ FTypeName(const U& val):m_val(val),m_depend(false)
	{
	}
	template <class U> FTypeName<T,N>& operator=(const U& val)
	{
		m_val=val;
		m_depend=false;
		return *this;
	}
	FTypeName<T,N>& operator=(const FTypeName<T,N>& val)
	{
		if (this==&val) return *this;
		m_val=val.m_val;
		m_depend=val.m_depend;
		if (m_depend) for(unsigned int i=0;i<N;++i) m_diff[i]=val.m_diff[i];
		return *this;
	}
	unsigned int size() const { return m_depend?N:0; }
	const T& operator[](const unsigned int i) const
	{
		USER_ASSERT(i<N && m_depend,"Index "<<i<<" out of bounds [0,"<<N<<"]")
		return m_diff[i];
	}
	T& operator[](const unsigned int i)
	{
		USER_ASSERT(i<N && m_depend,"Index "<<i<<" out of bounds [0,"<<N<<"]")
		return m_diff[i];
	}
	const T& val() const { return m_val; }
	T& x() { return m_val; }
	const T& deriv(const unsigned int i) const
	{
		USER_ASSERT(i<N,"Index "<<i<<" out of bounds [0,"<<N<<"]")
		if (m_depend) return m_diff[i];
		static T zero;
		zero=Op<T>::myZero();
		return zero;
	}
	T& d(const unsigned int i)
	{
		USER_ASSERT(i<N,"Index "<<i<<" out of bounds [0,"<<N<<"]")
		if (m_depend) return m_diff[i];
		static T zero;
		zero=Op<T>::myZero();
		return zero;
	}

	T& diff(unsigned int idx)
	{
		USER_ASSERT(idx<N,"index out of bounds: "<<idx);
		unsigned int i;
		for(i=0;i<idx;++i) m_diff[i]=Op<T>::myZero();
		m_diff[i++]=Op<T>::myOne();
		for(   ;i<N;++i) m_diff[i]=Op<T>::myZero();
		m_depend=true;
		return m_diff[idx];
	}
	bool depend() const { return m_depend; }
	void setDepend(const FTypeName<T,N>&) { m_depend=true; }
	void setDepend(const FTypeName<T,N>&, const FTypeName<T,N>&) { m_depend=true; }

	FTypeName<T,N>& operator+=(const FTypeName<T,N>& val);
	FTypeName<T,N>& operator-=(const FTypeName<T,N>& val);
	FTypeName<T,N>& operator*=(const FTypeName<T,N>& val);
	FTypeName<T,N>& operator/=(const FTypeName<T,N>& val);
	template <typename V> FTypeName<T,N>& operator+=(const V& val);
	template <typename V> FTypeName<T,N>& operator-=(const V& val);
	template <typename V> FTypeName<T,N>& operator*=(const V& val);
	template <typename V> FTypeName<T,N>& operator/=(const V& val);

};

template <typename T>
class FTypeName<T,0> // HEAP-BASED
{
	T m_val;
	unsigned int m_size;
	T* m_diff;
public:
	typedef T UnderlyingType;
	FTypeName():m_val(),m_size(0),m_diff(0){}
	FTypeName(const FTypeName<T>& val):m_val(val.m_val),m_size(val.m_size),m_diff(m_size==0?0:new T[m_size])
	{
		for(unsigned int i=0;i<m_size;++i) m_diff[i]=val.m_diff[i];
	}
	template <class U> /*explicit*/ FTypeName(const U& val):m_val(val),m_size(0),m_diff(0){}
	~FTypeName(){ delete[] m_diff; }
	template <class U> FTypeName<T>& operator=(const U& val)
	{
		m_val=val;
		m_size=0;
		delete[] m_diff;
		m_diff=0;
		return *this;
	}
	FTypeName<T>& operator=(const FTypeName<T>& val)
	{
		if (this==&val) return *this;
		m_val=val.m_val;
		if (val.m_size>0)
		{
			if (m_size==0)
			{
				m_size = val.m_size;
				m_diff = new T[m_size];
			}
			USER_ASSERT(m_size==val.m_size,"derivative vectors not of same size");
			for(unsigned int i=0;i<val.m_size;++i) m_diff[i]=val.m_diff[i];
		}
		else if (m_size>0)
		{
			for(unsigned int i=0;i<m_size;++i) m_diff[i]=0;
		}
		return *this;
	}
	unsigned int size() const { return m_size; }
	const T& operator[](const unsigned int i) const
	{
		USER_ASSERT(i<m_size,"index out of bounds: "<<i);
		return m_diff[i];
	}
	T& operator[](const unsigned int i)
	{
		USER_ASSERT(i<m_size,"index out of bounds: "<<i);
		return m_diff[i];
	}
	const T& val() const { return m_val; }
	T& x() { return m_val; }
	const T& deriv(const unsigned int i) const
	{
		if (i<m_size) return m_diff[i];
		static T zero;
		zero=Op<T>::myZero();
		return zero;
	}
	T& d(const unsigned int i)
	{
		if (i<m_size) return m_diff[i];
		static T zero;
		zero=Op<T>::myZero();
		return zero;
	}

	T& diff(unsigned int idx, unsigned int N)
	{
		USER_ASSERT(idx<N,"Index "<<idx<<" out of bounds [0,"<<N<<"]")
		if (m_size==0)
		{
			m_size = N;
			m_diff = new T[m_size];
		}
		else
		{
			USER_ASSERT(m_size==N,"derivative vectors not of same size "<<m_size<<","<<N);
		}
		unsigned int i;
		for(i=0;i<idx;++i) m_diff[i]=Op<T>::myZero();
		m_diff[i++]=Op<T>::myOne();
		for(   ;i<N;++i) m_diff[i]=Op<T>::myZero();
		return m_diff[idx];
	}
	bool depend() const { return m_size!=0; }
	void setDepend(const FTypeName<T>& val)
	{
		INTERNAL_ASSERT(val.m_size>0,"input is not a dependent variable")
		if (m_size==0)
		{
			m_size = val.m_size;
			m_diff = new T[m_size];
		}
		else
		{
			USER_ASSERT(m_size==val.m_size,"derivative vectors not of same size "<<m_size<<","<<val.m_size);
		}
	}
	void setDepend(const FTypeName<T>& val1, const FTypeName<T>& val2)
	{
		USER_ASSERT(val1.m_size==val2.m_size,"derivative vectors not of same size "<<val1.m_size<<","<<val2.m_size);
		INTERNAL_ASSERT(val1.m_size>0,"lhs-input is not a dependent variable")
		INTERNAL_ASSERT(val2.m_size>0,"rhs-input is not a dependent variable")
		if (m_size==0)
		{
			m_size=val1.m_size;
			m_diff = new T[m_size];
		}
		else
		{
			USER_ASSERT(m_size==val1.m_size,"derivative vectors not of same size "<<m_size<<","<<val1.m_size);
		}
	}

	FTypeName<T>& operator+=(const FTypeName<T>& val);
	FTypeName<T>& operator-=(const FTypeName<T>& val);
	FTypeName<T>& operator*=(const FTypeName<T>& val);
	FTypeName<T>& operator/=(const FTypeName<T>& val);
	template <typename V> FTypeName<T>& operator+=(const V& val);
	template <typename V> FTypeName<T>& operator-=(const V& val);
	template <typename V> FTypeName<T>& operator*=(const V& val);
	template <typename V> FTypeName<T>& operator/=(const V& val);

};

template <typename T, unsigned int N> bool operator==(const FTypeName<T,N>& val1, const FTypeName<T,N>& val2) { return Op<T>::myEq(val1.val(),val2.val()); }
template <typename T, unsigned int N> bool operator!=(const FTypeName<T,N>& val1, const FTypeName<T,N>& val2) { return Op<T>::myNe(val1.val(),val2.val()); }
template <typename T, unsigned int N> bool operator<(const FTypeName<T,N>& val1, const FTypeName<T,N>& val2) { return Op<T>::myLt(val1.val(),val2.val()); }
template <typename T, unsigned int N> bool operator<=(const FTypeName<T,N>& val1, const FTypeName<T,N>& val2) { return Op<T>::myLe(val1.val(),val2.val()); }
template <typename T, unsigned int N> bool operator>(const FTypeName<T,N>& val1, const FTypeName<T,N>& val2) { return Op<T>::myGt(val1.val(),val2.val()); }
template <typename T, unsigned int N> bool operator>=(const FTypeName<T,N>& val1, const FTypeName<T,N>& val2) { return Op<T>::myGe(val1.val(),val2.val()); }
template <typename T, unsigned int N, typename U> bool operator==(const FTypeName<T,N>& val1, const U& val2) { return Op<T>::myEq(val1.val(),val2); }
template <typename T, unsigned int N, typename U> bool operator==(const U& val1, const FTypeName<T,N>& val2) { return Op<T>::myEq(val1,val2.val()); }
template <typename T, unsigned int N, typename U> bool operator!=(const FTypeName<T,N>& val1, const U& val2) { return Op<T>::myNe(val1.val(),val2); }
template <typename T, unsigned int N, typename U> bool operator!=(const U& val1, const FTypeName<T,N>& val2) { return Op<T>::myNe(val1,val2.val()); }
template <typename T, unsigned int N, typename U> bool operator<(const FTypeName<T,N>& val1, const U& val2) { return Op<T>::myLt(val1.val(),val2); }
template <typename T, unsigned int N, typename U> bool operator<(const U& val1, const FTypeName<T,N>& val2) { return Op<T>::myLt(val1,val2.val()); }
template <typename T, unsigned int N, typename U> bool operator<=(const FTypeName<T,N>& val1, const U& val2) { return Op<T>::myLe(val1.val(),val2); }
template <typename T, unsigned int N, typename U> bool operator<=(const U& val1, const FTypeName<T,N>& val2) { return Op<T>::myLe(val1,val2.val()); }
template <typename T, unsigned int N, typename U> bool operator>(const FTypeName<T,N>& val1, const U& val2) { return Op<T>::myGt(val1.val(),val2); }
template <typename T, unsigned int N, typename U> bool operator>(const U& val1, const FTypeName<T,N>& val2) { return Op<T>::myGt(val1,val2.val()); }
template <typename T, unsigned int N, typename U> bool operator>=(const FTypeName<T,N>& val1, const U& val2) { return Op<T>::myGe(val1.val(),val2); }
template <typename T, unsigned int N, typename U> bool operator>=(const U& val1, const FTypeName<T,N>& val2) { return Op<T>::myGe(val1,val2.val()); }

template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T,N> add1(const U& a, const FTypeName<T,N>& b)
{
	FTypeName<T,N> c(a+b.val());
	if (!b.depend()) return c;
	c.setDepend(b);
	for(unsigned int i=0;i<N;++i) c[i]=b[i];
	return c;
}
template <typename T, class U>
INLINE2 FTypeName<T,0> add1(const U& a, const FTypeName<T,0>& b)
{
	FTypeName<T,0> c(a+b.val());
	if (!b.depend()) return c;
	c.setDepend(b);
	for(unsigned int i=0;i<c.size();++i) c[i]=b[i];
	return c;
}

template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T,N> add2(const FTypeName<T,N>& a, const U& b)
{
	FTypeName<T,N> c(a.val()+b);
	if (!a.depend()) return c;
	c.setDepend(a);
	for(unsigned int i=0;i<N;++i) c[i]=a[i];
	return c;
}
template <typename T, class U>
INLINE2 FTypeName<T,0> add2(const FTypeName<T,0>& a, const U& b)
{
	FTypeName<T,0> c(a.val()+b);
	if (!a.depend()) return c;
	c.setDepend(a);
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i];
	return c;
}

template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T,N> operator+ (const U& a, const FTypeName<T,N>& b)
{
	return add1(a,b);
}

template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T,N> operator+ (const FTypeName<T,N>& a, const U& b)
{
	return add2(a,b);
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> add3(const FTypeName<T,N>& a, const FTypeName<T,N>& b)
{
	FTypeName<T,N> c(a.val()+b.val());
	c.setDepend(a,b);
	for(unsigned int i=0;i<N;++i) c[i]=a[i]+b[i];
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> add3(const FTypeName<T,0>& a, const FTypeName<T,0>& b)
{
	FTypeName<T,0> c(a.val()+b.val());
	c.setDepend(a,b);
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i]+b[i];
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> operator+ (const FTypeName<T,N>& a, const FTypeName<T,N>& b)
{
	switch ((a.depend()?1:0)|(b.depend()?2:0))
	{
	case 0: return FTypeName<T,N>(a.val()+b.val());
	case 1: return add2(a,b.val());
	case 2: return add1(a.val(),b);
	}
	return add3(a,b);
}

template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T,N> sub1(const U& a, const FTypeName<T,N>& b)
{
	FTypeName<T,N> c(a-b.val());
	if (!b.depend()) return c;
	c.setDepend(b);
	for(unsigned int i=0;i<N;++i) c[i]=Op<T>::myNeg(b[i]);
	return c;
}
template <typename T, class U>
INLINE2 FTypeName<T,0> sub1(const U& a, const FTypeName<T,0>& b)
{
	FTypeName<T,0> c(a-b.val());
	if (!b.depend()) return c;
	c.setDepend(b);
	for(unsigned int i=0;i<c.size();++i) c[i]=Op<T>::myNeg(b[i]);
	return c;
}

template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T,N> sub2(const FTypeName<T,N>& a, const U& b)
{
	FTypeName<T,N> c(a.val()-b);
	if (!a.depend()) return c;
	c.setDepend(a);
	for(unsigned int i=0;i<N;++i) c[i]=a[i];
	return c;
}
template <typename T, class U>
INLINE2 FTypeName<T,0> sub2(const FTypeName<T,0>& a, const U& b)
{
	FTypeName<T,0> c(a.val()-b);
	if (!a.depend()) return c;
	c.setDepend(a);
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i];
	return c;
}

template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T,N> operator- (const U& a, const FTypeName<T,N>& b)
{
	return sub1(a,b);
}

template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T,N> operator- (const FTypeName<T,N>& a, const U& b)
{
	return sub2(a,b);
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> sub3(const FTypeName<T,N>& a, const FTypeName<T,N>& b)
{
	FTypeName<T,N> c(a.val()-b.val());
	c.setDepend(a,b);
	for(unsigned int i=0;i<N;++i) c[i]=a[i]-b[i];
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> sub3(const FTypeName<T,0>& a, const FTypeName<T,0>& b)
{
	FTypeName<T,0> c(a.val()-b.val());
	c.setDepend(a,b);
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i]-b[i];
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> operator- (const FTypeName<T,N>& a, const FTypeName<T,N>& b)
{
	switch ((a.depend()?1:0)|(b.depend()?2:0))
	{
	case 0: return FTypeName<T,N>(a.val()-b.val());
	case 1: return sub2(a,b.val());
	case 2: return sub1(a.val(),b);
	}
	return sub3(a,b);
}

template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T,N> mul1(const U& a, const FTypeName<T,N>& b)
{
	FTypeName<T,N> c(a*b.val());
	if (!b.depend()) return c;
	c.setDepend(b);
	for(unsigned int i=0;i<N;++i) c[i]=b[i]*a;
	return c;
}
template <typename T, class U>
INLINE2 FTypeName<T,0> mul1(const U& a, const FTypeName<T,0>& b)
{
	FTypeName<T,0> c(a*b.val());
	if (!b.depend()) return c;
	c.setDepend(b);
	for(unsigned int i=0;i<c.size();++i) c[i]=b[i]*a;
	return c;
}

template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T,N> mul2(const FTypeName<T,N>& a, const U& b)
{
	FTypeName<T,N> c(a.val()*b);
	if (!a.depend()) return c;
	c.setDepend(a);
	for(unsigned int i=0;i<N;++i) c[i]=a[i]*b;
	return c;
}
template <typename T, class U>
INLINE2 FTypeName<T,0> mul2(const FTypeName<T,0>& a, const U& b)
{
	FTypeName<T,0> c(a.val()*b);
	if (!a.depend()) return c;
	c.setDepend(a);
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i]*b;
	return c;
}

template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T,N> operator* (const U& a, const FTypeName<T,N>& b)
{
	return mul1(a,b);
}

template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T,N> operator* (const FTypeName<T,N>& a, const U& b)
{
	return mul2(a,b);
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> mul3 (const FTypeName<T,N>& a, const FTypeName<T,N>& b)
{
	const T& aval(a.val());
	const T& bval(b.val());
	FTypeName<T,N> c(aval*bval);
	c.setDepend(a,b);
	for(unsigned int i=0;i<N;++i) c[i]=a[i]*bval+b[i]*aval;
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> mul3 (const FTypeName<T,0>& a, const FTypeName<T,0>& b)
{
	const T& aval(a.val());
	const T& bval(b.val());
	FTypeName<T,0> c(aval*bval);
	c.setDepend(a,b);
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i]*bval+b[i]*aval;
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> operator* (const FTypeName<T,N>& a, const FTypeName<T,N>& b)
{
	switch ((a.depend()?1:0)|(b.depend()?2:0))
	{
	case 0: return FTypeName<T,N>(a.val()*b.val());
	case 1: return mul2(a,b.val());
	case 2: return mul1(a.val(),b);
	}
	return mul3(a,b);
}

template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T,N> div1(const U& a, const FTypeName<T,N>& b)
{
	FTypeName<T,N> c(a/b.val());
	if (!b.depend()) return c;
	T tmp(Op<T>::myNeg(c.val()/b.val()));
	c.setDepend(b);
	for(unsigned int i=0;i<N;++i) c[i]=tmp*b[i];
	return c;
}
template <typename T, class U>
INLINE2 FTypeName<T,0> div1(const U& a, const FTypeName<T,0>& b)
{
	FTypeName<T,0> c(a/b.val());
	if (!b.depend()) return c;
	T tmp(Op<T>::myNeg(c.val()/b.val()));
	c.setDepend(b);
	for(unsigned int i=0;i<c.size();++i) c[i]=tmp*b[i];
	return c;
}

template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T,N> div2(const FTypeName<T,N>& a, const U& b)
{
	FTypeName<T,N> c(a.val()/b);
	if (!a.depend()) return c;
	c.setDepend(a);
	for(unsigned int i=0;i<N;++i) c[i]=(a[i])/b;
	return c;
}
template <typename T, class U>
INLINE2 FTypeName<T,0> div2(const FTypeName<T,0>& a, const U& b)
{
	FTypeName<T,0> c(a.val()/b);
	if (!a.depend()) return c;
	c.setDepend(a);
	for(unsigned int i=0;i<c.size();++i) c[i]=(a[i])/b;
	return c;
}

template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T,N> operator/ (const U& a, const FTypeName<T,N>& b)
{
	return div1(a,b);
}

template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T,N> operator/ (const FTypeName<T,N>& a, const U& b)
{
	return div2(a,b);
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> div3(const FTypeName<T,N>& a, const FTypeName<T,N>& b)
{
	const T& bval(b.val());
	FTypeName<T,N> c(a.val()/bval);
	c.setDepend(a,b);
	const T& cval(c.val());
	for(unsigned int i=0;i<N;++i) c[i]=(a[i]-cval*b[i])/bval;
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> div3(const FTypeName<T,0>& a, const FTypeName<T,0>& b)
{
	const T& bval(b.val());
	FTypeName<T,0> c(a.val()/bval);
	c.setDepend(a,b);
	const T& cval(c.val());
	for(unsigned int i=0;i<c.size();++i) c[i]=(a[i]-cval*b[i])/bval;
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> operator/ (const FTypeName<T,N>& a, const FTypeName<T,N>& b)
{
	switch ((a.depend()?1:0)|(b.depend()?2:0))
	{
	case 0: return FTypeName<T,N>(a.val()/b.val());
	case 1: return div2(a,b.val());
	case 2: return div1(a.val(),b);
	}
	return div3(a,b);
}

template <typename T, unsigned int N>
FTypeName<T,N>& FTypeName<T,N>::operator+=(const FTypeName<T,N>& val)
{
	Op<T>::myCadd(m_val,val.m_val);
	if (!val.depend()) return *this;
	if (this->depend())
	{
		for(unsigned int i=0;i<N;++i) Op<T>::myCadd(m_diff[i],val[i]);
	}
	else
	{
		this->setDepend(val);
		for(unsigned int i=0;i<N;++i) m_diff[i]=val[i];
	}
	return *this;
}

template <typename T, unsigned int N>
FTypeName<T,N>& FTypeName<T,N>::operator-=(const FTypeName<T,N>& val)
{
	Op<T>::myCsub(m_val,val.m_val);
	if (!val.depend()) return *this;
	if (this->depend())
	{
		for(unsigned int i=0;i<N;++i) Op<T>::myCsub(m_diff[i],val[i]);
	}
	else
	{
		this->setDepend(val);
		for(unsigned int i=0;i<N;++i) m_diff[i]=Op<T>::myNeg(val[i]);
	}
	return *this;
}

template <typename T, unsigned int N>
FTypeName<T,N>& FTypeName<T,N>::operator*=(const FTypeName<T,N>& val)
{
	if (this->depend() && val.depend())
	{
		for(unsigned int i=0;i<N;++i) m_diff[i]=m_diff[i]*val.m_val+val.m_diff[i]*m_val;
	}
	else if (this->depend())
	{
		for(unsigned int i=0;i<N;++i) Op<T>::myCmul(m_diff[i],val.m_val);
	}
	else // (val.depend())
	{
		this->setDepend(val);
		for(unsigned int i=0;i<N;++i) m_diff[i]=val.m_diff[i]*m_val;
	}
	Op<T>::myCmul(m_val,val.m_val);
	return *this;
}

template <typename T, unsigned int N>
FTypeName<T,N>& FTypeName<T,N>::operator/=(const FTypeName<T,N>& val)
{
	Op<T>::myCdiv(m_val,val.m_val);
	if (this->depend() && val.depend())
	{
		for(unsigned int i=0;i<N;++i) m_diff[i]=(m_diff[i]-m_val*val.m_diff[i])/val.m_val;
	}
	else if (this->depend())
	{
		for(unsigned int i=0;i<N;++i) Op<T>::myCdiv(m_diff[i],val.m_val);
	}
	else // (val.depend())
	{
		this->setDepend(val);
		for(unsigned int i=0;i<N;++i) m_diff[i]=Op<T>::myNeg(m_val*val.m_diff[i]/val.m_val);
	}
	return *this;
}

template <typename T, unsigned int N>
template <typename V> FTypeName<T,N>& FTypeName<T,N>::operator+=(const V& val)
{
	Op<T>::myCadd(m_val,val);
	return *this;
}
template <typename T, unsigned int N>
template <typename V> FTypeName<T,N>& FTypeName<T,N>::operator-=(const V& val)
{
	Op<T>::myCsub(m_val,val);
	return *this;
}

template <typename T, unsigned int N>
template <typename V> FTypeName<T,N>& FTypeName<T,N>::operator*=(const V& val)
{
	Op<T>::myCmul(m_val,val);
	if (!this->depend()) return *this;
	for(unsigned int i=0;i<N;++i) Op<T>::myCmul(m_diff[i],val);
	return *this;
}

template <typename T, unsigned int N>
template <typename V> FTypeName<T,N>& FTypeName<T,N>::operator/=(const V& val)
{
	Op<T>::myCdiv(m_val,val);
	if (!this->depend()) return *this;
	for(unsigned int i=0;i<N;++i) Op<T>::myCdiv(m_diff[i],val);
	return *this;
}

template <typename T>
FTypeName<T,0>& FTypeName<T,0>::operator+=(const FTypeName<T,0>& val)
{
	Op<T>::myCadd(m_val,val.m_val);
	if (!val.depend()) return *this;
	if (this->depend())
	{
		for(unsigned int i=0;i<this->size();++i) Op<T>::myCadd(m_diff[i],val[i]);
	}
	else
	{
		this->setDepend(val);
		for(unsigned int i=0;i<this->size();++i) m_diff[i]=val[i];
	}
	return *this;
}
template <typename T>
FTypeName<T,0>& FTypeName<T,0>::operator-=(const FTypeName<T,0>& val)
{
	Op<T>::myCsub(m_val,val.m_val);
	if (!val.depend()) return *this;
	if (this->depend())
	{
		for(unsigned int i=0;i<this->size();++i) Op<T>::myCsub(m_diff[i],val[i]);
	}
	else
	{
		this->setDepend(val);
		for(unsigned int i=0;i<this->size();++i) m_diff[i]=Op<T>::myNeg(val[i]);
	}
	return *this;
}
template <typename T>
FTypeName<T,0>& FTypeName<T,0>::operator*=(const FTypeName<T,0>& val)
{
	if (this->depend() && val.depend())
	{
		for(unsigned int i=0;i<this->size();++i) m_diff[i]=m_diff[i]*val.m_val+val.m_diff[i]*m_val;
	}
	else if (this->depend())
	{
		for(unsigned int i=0;i<this->size();++i) Op<T>::myCmul(m_diff[i],val.m_val);
	}
	else // (val.depend())
	{
		this->setDepend(val);
		for(unsigned int i=0;i<this->size();++i) m_diff[i]=val.m_diff[i]*m_val;
	}
	Op<T>::myCmul(m_val,val.m_val);
	return *this;
}
template <typename T>
FTypeName<T,0>& FTypeName<T,0>::operator/=(const FTypeName<T,0>& val)
{
	Op<T>::myCdiv(m_val,val.m_val);
	if (this->depend() && val.depend())
	{
		for(unsigned int i=0;i<this->size();++i) m_diff[i]=(m_diff[i]-m_val*val.m_diff[i])/val.m_val;
	}
	else if (this->depend())
	{
		for(unsigned int i=0;i<this->size();++i) Op<T>::myCdiv(m_diff[i],val.m_val);
	}
	else // (val.depend())
	{
		this->setDepend(val);
		for(unsigned int i=0;i<this->size();++i) m_diff[i]=Op<T>::myNeg(m_val*val.m_diff[i]/val.m_val);
	}
	return *this;
}
template <typename T>
template <typename V> FTypeName<T,0>& FTypeName<T,0>::operator+=(const V& val)
{
	Op<T>::myCadd(m_val,val);
	return *this;
}
template <typename T>
template <typename V> FTypeName<T,0>& FTypeName<T,0>::operator-=(const V& val)
{
	Op<T>::myCsub(m_val,val);
	return *this;
}
template <typename T>
template <typename V> FTypeName<T,0>& FTypeName<T,0>::operator*=(const V& val)
{
	Op<T>::myCmul(m_val,val);
	if (!this->depend()) return *this;
	for(unsigned int i=0;i<this->size();++i) Op<T>::myCmul(m_diff[i],val);
	return *this;
}
template <typename T>
template <typename V> FTypeName<T,0>& FTypeName<T,0>::operator/=(const V& val)
{
	Op<T>::myCdiv(m_val,val);
	if (!this->depend()) return *this;
	for(unsigned int i=0;i<this->size();++i) Op<T>::myCdiv(m_diff[i],val);
	return *this;
}

template <typename T, typename U, unsigned int N>
INLINE2 FTypeName<T,N> pow1(const U& a, const FTypeName<T,N>& b)
{
	FTypeName<T,N> c(Op<T>::myPow(a,b.val()));
	if (!b.depend()) return c;
	T tmp(c.val()*Op<T>::myLog(a));
	c.setDepend(b);
	for(unsigned int i=0;i<N;++i) c[i]=tmp*b[i];
	return c;
}
template <typename T, typename U>
INLINE2 FTypeName<T,0> pow1(const U& a, const FTypeName<T,0>& b)
{
	FTypeName<T,0> c(Op<T>::myPow(a,b.val()));
	if (!b.depend()) return c;
	T tmp(c.val()*Op<T>::myLog(a));
	c.setDepend(b);
	for(unsigned int i=0;i<c.size();++i) c[i]=tmp*b[i];
	return c;
}

template <typename T, typename U, unsigned int N>
INLINE2 FTypeName<T,N> pow2(const FTypeName<T,N>& a, const U& b)
{
	FTypeName<T,N> c(Op<T>::myPow(a.val(),b));
	if (!a.depend()) return c;
	T tmp(b*Op<T>::myPow(a.val(),b-Op<T>::myOne()));
	c.setDepend(a);
	for(unsigned int i=0;i<N;++i) c[i]=tmp*a[i];
	return c;
}
template <typename T, typename U>
INLINE2 FTypeName<T,0> pow2(const FTypeName<T,0>& a, const U& b)
{
	FTypeName<T,0> c(Op<T>::myPow(a.val(),b));
	if (!a.depend()) return c;
	T tmp(b*Op<T>::myPow(a.val(),b-Op<T>::myOne()));
	c.setDepend(a);
	for(unsigned int i=0;i<c.size();++i) c[i]=tmp*a[i];
	return c;
}

template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T,N> pow (const U& a, const FTypeName<T,N>& b)
{
	return pow1(a,b);
}

template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T,N> pow (const FTypeName<T,N>& a,const U& b)
{
	return pow2(a,b);
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> pow3(const FTypeName<T,N>& a, const FTypeName<T,N>& b)
{
	FTypeName<T,N> c(Op<T>::myPow(a.val(),b.val()));
	T tmp(b.val()*Op<T>::myPow(a.val(),b.val()-Op<T>::myOne())),tmp1(c.val()*Op<T>::myLog(a.val()));
	c.setDepend(a,b);
	for(unsigned int i=0;i<N;++i) c[i]=tmp*a[i]+tmp1*b[i];
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> pow3(const FTypeName<T,0>& a, const FTypeName<T,0>& b)
{
	FTypeName<T,0> c(Op<T>::myPow(a.val(),b.val()));
	T tmp(b.val()*Op<T>::myPow(a.val(),b.val()-Op<T>::myOne())),tmp1(c.val()*Op<T>::myLog(a.val()));
	c.setDepend(a,b);
	for(unsigned int i=0;i<c.size();++i) c[i]=tmp*a[i]+tmp1*b[i];
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> pow (const FTypeName<T,N>& a, const FTypeName<T,N>& b)
{
	switch ((a.depend()?1:0)|(b.depend()?2:0))
	{
	case 0: return FTypeName<T,N>(Op<T>::myPow(a.val(),b.val()));
	case 1: return pow2(a,b.val());
	case 2: return pow1(a.val(),b);
	}
	return pow3(a,b);
}

/* Unary operators */
template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> operator+ (const FTypeName<T,N>& a)
{
	FTypeName<T,N> c(a.val());
	if (!a.depend()) return c;
	c.setDepend(a);
	for(unsigned int i=0;i<N;++i) c[i]=a[i];
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> operator+ (const FTypeName<T,0>& a)
{
	FTypeName<T,0> c(a.val());
	if (!a.depend()) return c;
	c.setDepend(a);
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i];
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> operator- (const FTypeName<T,N>& a)
{
	FTypeName<T,N> c(Op<T>::myNeg(a.val()));
	if (!a.depend()) return c;
	c.setDepend(a);
	for(unsigned int i=0;i<N;++i) c[i]=Op<T>::myNeg(a[i]);
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> operator- (const FTypeName<T,0>& a)
{
	FTypeName<T,0> c(Op<T>::myNeg(a.val()));
	if (!a.depend()) return c;
	c.setDepend(a);
	for(unsigned int i=0;i<c.size();++i) c[i]=Op<T>::myNeg(a[i]);
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> sqr (const FTypeName<T,N>& a)
{
	FTypeName<T,N> c(Op<T>::mySqr(a.val()));
	if (!a.depend()) return c;
	T tmp(Op<T>::myTwo()*a.val());
	c.setDepend(a);
	for(unsigned int i=0;i<N;++i) c[i]=a[i]*tmp;
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> sqr (const FTypeName<T,0>& a)
{
	FTypeName<T,0> c(Op<T>::mySqr(a.val()));
	if (!a.depend()) return c;
	T tmp(Op<T>::myTwo()*a.val());
	c.setDepend(a);
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i]*tmp;
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> exp (const FTypeName<T,N>& a)
{
	FTypeName<T,N> c(Op<T>::myExp(a.val()));
	if (!a.depend()) return c;
	c.setDepend(a);
	const T& cval(c.val());
	for(unsigned int i=0;i<N;++i) c[i]=a[i]*cval;
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> exp (const FTypeName<T,0>& a)
{
	FTypeName<T,0> c(Op<T>::myExp(a.val()));
	if (!a.depend()) return c;
	c.setDepend(a);
	const T& cval(c.val());
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i]*cval;
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> log (const FTypeName<T,N>& a)
{
	FTypeName<T,N> c(Op<T>::myLog(a.val()));
	if (!a.depend()) return c;
	c.setDepend(a);
	const T& aval(a.val());
	for(unsigned int i=0;i<N;++i) c[i]=a[i]/aval;
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> log (const FTypeName<T,0>& a)
{
	FTypeName<T,0> c(Op<T>::myLog(a.val()));
	if (!a.depend()) return c;
	c.setDepend(a);
	const T& aval(a.val());
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i]/aval;
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> sqrt (const FTypeName<T,N>& a)
{
	FTypeName<T,N> c(Op<T>::mySqrt(a.val()));
	if (!a.depend()) return c;
	T tmp(c.val()*Op<T>::myTwo());
	c.setDepend(a);
	for(unsigned int i=0;i<N;++i) c[i]=a[i]/tmp;
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> sqrt (const FTypeName<T,0>& a)
{
	FTypeName<T,0> c(Op<T>::mySqrt(a.val()));
	if (!a.depend()) return c;
	T tmp(c.val()*Op<T>::myTwo());
	c.setDepend(a);
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i]/tmp;
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> sin (const FTypeName<T,N>& a)
{
	FTypeName<T,N> c(Op<T>::mySin(a.val()));
	if (!a.depend()) return c;
	T tmp(Op<T>::myCos(a.val()));
	c.setDepend(a);
	for(unsigned int i=0;i<N;++i) c[i]=a[i]*tmp;
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> sin (const FTypeName<T,0>& a)
{
	FTypeName<T,0> c(Op<T>::mySin(a.val()));
	if (!a.depend()) return c;
	T tmp(Op<T>::myCos(a.val()));
	c.setDepend(a);
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i]*tmp;
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> cos (const FTypeName<T,N>& a)
{
	FTypeName<T,N> c(Op<T>::myCos(a.val()));
	if (!a.depend()) return c;
	T tmp(-Op<T>::mySin(a.val()));
	c.setDepend(a);
	for(unsigned int i=0;i<N;++i) c[i]=a[i]*tmp;
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> cos (const FTypeName<T,0>& a)
{
	FTypeName<T,0> c(Op<T>::myCos(a.val()));
	if (!a.depend()) return c;
	T tmp(-Op<T>::mySin(a.val()));
	c.setDepend(a);
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i]*tmp;
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> tan (const FTypeName<T,N>& a)
{
	FTypeName<T,N> c(Op<T>::myTan(a.val()));
	if (!a.depend()) return c;
	T tmp(Op<T>::myOne()+Op<T>::mySqr(c.val()));
	c.setDepend(a);
	for(unsigned int i=0;i<N;++i)  c[i]=a[i]*tmp;
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> tan (const FTypeName<T,0>& a)
{
	FTypeName<T,0> c(Op<T>::myTan(a.val()));
	if (!a.depend()) return c;
	T tmp(Op<T>::myOne()+Op<T>::mySqr(c.val()));
	c.setDepend(a);
	for(unsigned int i=0;i<c.size();++i)  c[i]=a[i]*tmp;
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> asin (const FTypeName<T,N>& a)
{
	FTypeName<T,N> c(Op<T>::myAsin(a.val()));
	if (!a.depend()) return c;
	T tmp(Op<T>::myInv(Op<T>::mySqrt(Op<T>::myOne()-Op<T>::mySqr(a.val()))));
	c.setDepend(a);
	for(unsigned int i=0;i<N;++i) c[i]=a[i]*tmp;
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> asin (const FTypeName<T,0>& a)
{
	FTypeName<T,0> c(Op<T>::myAsin(a.val()));
	if (!a.depend()) return c;
	T tmp(Op<T>::myInv(Op<T>::mySqrt(Op<T>::myOne()-Op<T>::mySqr(a.val()))));
	c.setDepend(a);
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i]*tmp;
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> acos (const FTypeName<T,N>& a)
{
	FTypeName<T,N> c(Op<T>::myAcos(a.val()));
	if (!a.depend()) return c;
	T tmp(Op<T>::myNeg(Op<T>::myInv(Op<T>::mySqrt(Op<T>::myOne()-Op<T>::mySqr(a.val())))));
	c.setDepend(a);
	for(unsigned int i=0;i<N;++i) c[i]=a[i]*tmp;
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> acos (const FTypeName<T,0>& a)
{
	FTypeName<T,0> c(Op<T>::myAcos(a.val()));
	if (!a.depend()) return c;
	T tmp(Op<T>::myNeg(Op<T>::myInv(Op<T>::mySqrt(Op<T>::myOne()-Op<T>::mySqr(a.val())))));
	c.setDepend(a);
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i]*tmp;
	return c;
}

template <typename T, unsigned int N>
INLINE2 FTypeName<T,N> atan (const FTypeName<T,N>& a)
{
	FTypeName<T,N> c(Op<T>::myAtan(a.val()));
	if (!a.depend()) return c;
	T tmp(Op<T>::myInv(Op<T>::myOne()+Op<T>::mySqr(a.val())));
	c.setDepend(a);
	for(unsigned int i=0;i<N;++i) c[i]=a[i]*tmp;
	return c;
}
template <typename T>
INLINE2 FTypeName<T,0> atan (const FTypeName<T,0>& a)
{
	FTypeName<T,0> c(Op<T>::myAtan(a.val()));
	if (!a.depend()) return c;
	T tmp(Op<T>::myInv(Op<T>::myOne()+Op<T>::mySqr(a.val())));
	c.setDepend(a);
	for(unsigned int i=0;i<c.size();++i) c[i]=a[i]*tmp;
	return c;
}

template <typename U, unsigned int N> struct Op< FTypeName<U,N> >
{
	typedef FTypeName<U,N> T;
	typedef FTypeName<U,N> Underlying;
	typedef typename Op<U>::Base Base;
	static Base myInteger(const int i) { return Base(i); }
	static Base myZero() { return myInteger(0); }
	static Base myOne() { return myInteger(1);}
	static Base myTwo() { return myInteger(2); }
	static Base myPI() { return Op<Base>::myPI(); }
	static T myPos(const T& x) { return +x; }
	static T myNeg(const T& x) { return -x; }
	template <typename V> static T& myCadd(T& x, const V& y) { return x+=y; }
	template <typename V> static T& myCsub(T& x, const V& y) { return x-=y; }
	template <typename V> static T& myCmul(T& x, const V& y) { return x*=y; }
	template <typename V> static T& myCdiv(T& x, const V& y) { return x/=y; }
	static T myInv(const T& x) { return myOne()/x; }
	static T mySqr(const T& x) { return fadbad::sqr(x); }
	template <typename X, typename Y>
	static T myPow(const X& x, const Y& y) { return fadbad::pow(x,y); }
	static T mySqrt(const T& x) { return fadbad::sqrt(x); }
	static T myLog(const T& x) { return fadbad::log(x); }
	static T myExp(const T& x) { return fadbad::exp(x); }
	static T mySin(const T& x) { return fadbad::sin(x); }
	static T myCos(const T& x) { return fadbad::cos(x); }
	static T myTan(const T& x) { return fadbad::tan(x); }
	static T myAsin(const T& x) { return fadbad::asin(x); }
	static T myAcos(const T& x) { return fadbad::acos(x); }
	static T myAtan(const T& x) { return fadbad::atan(x); }
	static bool myEq(const T& x, const T& y) { return x==y; }
	static bool myNe(const T& x, const T& y) { return x!=y; }
	static bool myLt(const T& x, const T& y) { return x<y; }
	static bool myLe(const T& x, const T& y) { return x<=y; }
	static bool myGt(const T& x, const T& y) { return x>y; }
	static bool myGe(const T& x, const T& y) { return x>=y; }
};

} // namespace fadbad

#endif
