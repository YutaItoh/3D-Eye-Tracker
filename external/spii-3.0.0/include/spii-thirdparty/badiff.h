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

#ifndef _BADIFF_H
#define _BADIFF_H

#include "fadbad.h"

#include <vector>
#include <stack>

namespace fadbad
{

template <typename U>
class Derivatives
{
public:
	class RecycleBin
	{
		std::stack< std::vector<U>* > m_recycle;
		RecycleBin(const RecycleBin&){/*illegal*/}
	public:
		RecycleBin(){}
		std::vector<U>* popRecycle(const unsigned int n)
		{
			if (m_recycle.empty()) return new std::vector<U>(n);
			std::vector<U>* elm=m_recycle.top();
			m_recycle.pop();
			USER_ASSERT(elm->size()==n,"Size mismatch "<<elm->size()<<"!="<<n)
			return elm;
		}
		void pushRecycle(std::vector<U>* elm)
		{
			m_recycle.push(elm);
		}
		~RecycleBin()
		{
			while(!m_recycle.empty())
			{
				delete m_recycle.top();
				m_recycle.pop();
			}
		}
	};
private:
	std::vector<U>* m_values;

	unsigned int size() const { return (unsigned int) m_values->size(); }

public:
	Derivatives():m_values(0){}

	void recycle(RecycleBin& bin)
	{
		USER_ASSERT(m_values!=0,"Nothing to recycle")
		bin.pushRecycle(m_values);
		m_values=0;
	}
	
	bool haveValues() const { return m_values!=0; }

	U& diff(RecycleBin& bin, const unsigned int i, const unsigned int n)
	{
		USER_ASSERT(i<n,"Index "<<i<<" out of range [0,"<<n<<"]")
		if (m_values==0)
		{
			m_values=bin.popRecycle(n);
			for(unsigned int j=0;j<n;++j) (*m_values)[j]=Op<U>::myZero();
		}
		USER_ASSERT(m_values->size()==n,"Size mismatch "<<m_values->size()<<"!="<<n)
		return (*m_values)[i]=Op<U>::myOne();
	}
	void add(RecycleBin& bin, const Derivatives<U>& d)
	{
		USER_ASSERT(d.size()>0,"Propagating node with no derivatives")
		if (m_values==0)
		{
			m_values=bin.popRecycle(d.size());
			for(unsigned int i=0;i<m_values->size();++i) (*m_values)[i]=(*d.m_values)[i];
		}
		else
		{
			USER_ASSERT(m_values->size()==d.size(),"Size mismatch "<<m_values->size()<<"!="<<d.size())
			for(unsigned int i=0;i<m_values->size();++i) Op<U>::myCadd((*m_values)[i],(*d.m_values)[i]);
		}
	}
	void sub(RecycleBin& bin, const Derivatives<U>& d)
	{
		USER_ASSERT(d.size()>0,"Propagating node with no derivatives")
		if (m_values==0)
		{
			m_values=bin.popRecycle(d.size());
			for(unsigned int i=0;i<m_values->size();++i) (*m_values)[i]=Op<U>::myNeg((*d.m_values)[i]);
		}
		else
		{
			USER_ASSERT(m_values->size()==d.size(),"Size mismatch "<<m_values->size()<<"!="<<d.size())
			for(unsigned int i=0;i<m_values->size();++i) Op<U>::myCsub((*m_values)[i],(*d.m_values)[i]);
		}
	}
	template <typename V>
	void add(RecycleBin& bin, const V& a, const Derivatives<U>& d)
	{
		USER_ASSERT(d.size()>0,"Propagating node with no derivatives")
		if (m_values==0)
		{
			m_values=bin.popRecycle(d.size());
			for(unsigned int i=0;i<m_values->size();++i) (*m_values)[i]=a*(*d.m_values)[i];
		}
		else
		{
			USER_ASSERT(m_values->size()==d.size(),"Size mismatch "<<m_values->size()<<"!="<<d.size())
			for(unsigned int i=0;i<m_values->size();++i) Op<U>::myCadd((*m_values)[i],a*(*d.m_values)[i]);
		}
	}
	template <typename V>
	void sub(RecycleBin& bin, const V& a, const Derivatives<U>& d)
	{
		USER_ASSERT(d.size()>0,"Propagating node with no derivatives")
		if (m_values==0)
		{
			m_values=bin.popRecycle(d.size());
			for(unsigned int i=0;i<m_values->size();++i) (*m_values)[i]=Op<U>::myNeg(a*(*d.m_values)[i]);
		}
		else
		{
			USER_ASSERT(m_values->size()==d.size(),"Size mismatch "<<m_values->size()<<"!="<<d.size())
			for(unsigned int i=0;i<m_values->size();++i) Op<U>::myCsub((*m_values)[i],a*(*d.m_values)[i]);
		}
	}

	U& operator[](const unsigned int i) 
	{
		if (m_values!=0)
		{
			USER_ASSERT(i<m_values->size(),"Index "<<i<<" out of bounds [0,"<<m_values->size()<<"]")
			return (*m_values)[i];
		}
		else
		{
			static U zero;
			zero=Op<U>::myZero();
			return zero;
		}
	}
	const U& operator[](const unsigned int i) const 
	{
		if (m_values!=0)
		{
			USER_ASSERT(i<m_values->size(),"Index "<<i<<" out of bounds [0,"<<m_values->size()<<"]")
			return (*m_values)[i];
		}
		else
		{
			static U zero;
			zero=Op<U>::myZero();
			return zero;
		}
	}
};


template <typename U>
class BTypeNameHV // Heap Value
{
	U m_val;
	mutable unsigned int m_rc;
	
protected:
	mutable Derivatives<U> m_derivatives;
	virtual ~BTypeNameHV(){}
public:
	BTypeNameHV():m_rc(0),m_derivatives(){}
	template <typename V> explicit BTypeNameHV(const V& val):m_val(val),m_rc(0),m_derivatives(){}
	const U& val() const { return m_val; }
	U& val() { return m_val; }
	void decRef(BTypeNameHV<U>*& pBTypeNameHV) 
	{
		INTERNAL_ASSERT(m_rc>0,"Resource counter negative");
		if (--m_rc==0)
		{
			if (m_derivatives.haveValues())
			{
				typename Derivatives<U>::RecycleBin bin;
				propagate(bin);
				m_derivatives.recycle(bin);
				propagateChildren(bin);
			}
			delete this;
		}
		pBTypeNameHV=0;
	}
	void decRef(typename Derivatives<U>::RecycleBin& bin, BTypeNameHV<U>*& pBTypeNameHV) 
	{
		INTERNAL_ASSERT(m_rc>0,"Resource counter negative");
		if (--m_rc==0)
		{
			if (m_derivatives.haveValues())
			{
				propagate(bin);
				m_derivatives.recycle(bin);
				propagateChildren(bin);
			}
			delete this;
		}
		pBTypeNameHV=0;
	}
	void incRef() const {++m_rc;}

	U& diff(typename Derivatives<U>::RecycleBin& bin, const unsigned int idx, const unsigned int size)
	{
		return m_derivatives.diff(bin,idx,size);
	}
	virtual void propagate(typename Derivatives<U>::RecycleBin&) {}
	virtual void propagateChildren(typename Derivatives<U>::RecycleBin&) {}
	void add(typename Derivatives<U>::RecycleBin& bin, const Derivatives<U>& d) { m_derivatives.add(bin,d); }
	void sub(typename Derivatives<U>::RecycleBin& bin, const Derivatives<U>& d) { m_derivatives.sub(bin,d); }
	void add(typename Derivatives<U>::RecycleBin& bin, const U& a, const Derivatives<U>& d) { m_derivatives.add(bin,a,d); }
	void sub(typename Derivatives<U>::RecycleBin& bin, const U& a, const Derivatives<U>& d) { m_derivatives.sub(bin,a,d); }
	U& deriv(const unsigned int i) 
	{
		USER_ASSERT(m_rc==1,"Still non-propagated dependencies ("<<m_rc-1<<"), the derivative might be wrong")
		return m_derivatives[i]; 
	}
};

template <typename U>
class BTypeName
{
	struct SV // Stack Value refers to reference-counted Heap Value:
	{
		mutable BTypeNameHV<U>* m_pBTypeNameHV;
		SV(BTypeNameHV<U>* pBTypeNameHV):m_pBTypeNameHV(pBTypeNameHV){ m_pBTypeNameHV->incRef(); }
		SV(const typename BTypeName<U>::SV& sv):m_pBTypeNameHV(sv.m_pBTypeNameHV){ m_pBTypeNameHV->incRef(); }
		~SV(){ m_pBTypeNameHV->decRef(m_pBTypeNameHV); }
		BTypeNameHV<U>* getBTypeNameHV() const { return m_pBTypeNameHV; }
		void setBTypeNameHV(BTypeNameHV<U>* pBTypeNameHV)
		{
			if (m_pBTypeNameHV!=pBTypeNameHV) 
			{
				m_pBTypeNameHV->decRef(m_pBTypeNameHV);
				m_pBTypeNameHV=pBTypeNameHV;
				m_pBTypeNameHV->incRef();
			}
		}
		const U& val() const { return m_pBTypeNameHV->val(); }
		U& val() { return m_pBTypeNameHV->val(); }
		const U& deriv(const unsigned int i) const { return m_pBTypeNameHV->deriv(i); }
		U& deriv(const unsigned int i) { return m_pBTypeNameHV->deriv(i); }

		U& diff(const unsigned int idx, const unsigned int size) 
		{
			typename Derivatives<U>::RecycleBin bin;
			U& res(m_pBTypeNameHV->diff(bin,idx,size));
			BTypeNameHV<U>* pHV=new BTypeNameHV<U>(this->val());
			m_pBTypeNameHV->decRef(bin,m_pBTypeNameHV);
			m_pBTypeNameHV=pHV;
			m_pBTypeNameHV->incRef();
			return res;
		}
	} m_sv;
public:
	typedef U UnderlyingType;
	BTypeName():m_sv(new BTypeNameHV<U>()){}
	BTypeName(BTypeNameHV<U>* pBTypeNameHV):m_sv(pBTypeNameHV){}
	explicit BTypeName(const typename BTypeName<U>::SV& sv):m_sv(sv){}
	template <typename V> /*explicit*/ BTypeName(const V& val):m_sv(new BTypeNameHV<U>(val)){}
	BTypeName<U>& operator=(const BTypeName<U>& val) 
	{
		if (this==&val) return *this;
		m_sv.setBTypeNameHV(val.m_sv.getBTypeNameHV());
		return *this; 
	}
	template <typename V> BTypeName<U>& operator=(const V& val) { m_sv.setBTypeNameHV(new BTypeNameHV<U>(val)); return *this; }
	BTypeNameHV<U>* getBTypeNameHV() const { return m_sv.getBTypeNameHV(); }
	void setBTypeNameHV(const BTypeNameHV<U>* pBTypeNameHV) { m_sv.setBTypeNameHV(pBTypeNameHV); }
	const U& val() const { return m_sv.val(); }
	U& x() { return m_sv.val(); }
	const U& deriv(const unsigned int i) const { return m_sv.deriv(i); }
	U& d(const unsigned int i) { return m_sv.deriv(i); }
	U& diff(const unsigned int idx, const unsigned int size) { return m_sv.diff(idx,size); }
	
	BTypeName<U>& operator+=(const BTypeName<U>& val);
	BTypeName<U>& operator-=(const BTypeName<U>& val);
	BTypeName<U>& operator*=(const BTypeName<U>& val);
	BTypeName<U>& operator/=(const BTypeName<U>& val);
	template <typename V> BTypeName<U>& operator+=(const V& val);
	template <typename V> BTypeName<U>& operator-=(const V& val);
	template <typename V> BTypeName<U>& operator*=(const V& val);
	template <typename V> BTypeName<U>& operator/=(const V& val);
};

template <typename U> bool operator==(const BTypeName<U>& val1, const BTypeName<U>& val2) { return Op<U>::myEq(val1.val(),val2.val()); }
template <typename U> bool operator!=(const BTypeName<U>& val1, const BTypeName<U>& val2) { return Op<U>::myNe(val1.val(),val2.val()); }
template <typename U> bool operator<(const BTypeName<U>& val1, const BTypeName<U>& val2) { return Op<U>::myLt(val1.val(),val2.val()); }
template <typename U> bool operator<=(const BTypeName<U>& val1, const BTypeName<U>& val2) { return Op<U>::myLe(val1.val(),val2.val()); }
template <typename U> bool operator>(const BTypeName<U>& val1, const BTypeName<U>& val2) { return Op<U>::myGt(val1.val(),val2.val()); }
template <typename U> bool operator>=(const BTypeName<U>& val1, const BTypeName<U>& val2) { return Op<U>::myGe(val1.val(),val2.val()); }
template <typename U, typename V> bool operator==(const BTypeName<U>& val1, const V& val2) { return Op<U>::myEq(val1.val(),val2); }
template <typename U, typename V> bool operator==(const V& val1, const BTypeName<U>& val2) { return Op<U>::myEq(val1,val2.val()); }
template <typename U, typename V> bool operator!=(const BTypeName<U>& val1, const V& val2) { return Op<U>::myNe(val1.val(),val2); }
template <typename U, typename V> bool operator!=(const V& val1, const BTypeName<U>& val2) { return Op<U>::myNe(val1,val2.val()); }
template <typename U, typename V> bool operator<(const BTypeName<U>& val1, const V& val2) { return Op<U>::myLt(val1.val(),val2); }
template <typename U, typename V> bool operator<(const V& val1, const BTypeName<U>& val2) { return Op<U>::myLt(val1,val2.val()); }
template <typename U, typename V> bool operator<=(const BTypeName<U>& val1, const V& val2) { return Op<U>::myLe(val1.val(),val2); }
template <typename U, typename V> bool operator<=(const V& val1, const BTypeName<U>& val2) { return Op<U>::myLe(val1,val2.val()); }
template <typename U, typename V> bool operator>(const BTypeName<U>& val1, const V& val2) { return Op<U>::myGt(val1.val(),val2); }
template <typename U, typename V> bool operator>(const V& val1, const BTypeName<U>& val2) { return Op<U>::myGt(val1,val2.val()); }
template <typename U, typename V> bool operator>=(const BTypeName<U>& val1, const V& val2) { return Op<U>::myGe(val1.val(),val2); }
template <typename U, typename V> bool operator>=(const V& val1, const BTypeName<U>& val2) { return Op<U>::myGe(val1,val2.val()); }

// Binary operator base class:

template <typename U>
class BinBTypeNameHV : public BTypeNameHV<U>
{
	BTypeNameHV<U>* m_pOp1;
	BTypeNameHV<U>* m_pOp2;
public:
	BinBTypeNameHV(const U& val, BTypeNameHV<U>* pOp1, BTypeNameHV<U>* pOp2):BTypeNameHV<U>(val),m_pOp1(pOp1),m_pOp2(pOp2)
	{
		m_pOp1->incRef();
		m_pOp2->incRef();
	}
	virtual void propagateChildren(typename Derivatives<U>::RecycleBin& bin)
	{
		m_pOp1->decRef(bin,m_pOp1);
		m_pOp2->decRef(bin,m_pOp2);
	}
	virtual ~BinBTypeNameHV()
	{
		if (m_pOp1) m_pOp1->decRef(m_pOp1);
		if (m_pOp2) m_pOp2->decRef(m_pOp2);
	}
	BTypeNameHV<U>* op1() { return m_pOp1; }
	BTypeNameHV<U>* op2() { return m_pOp2; }
};

// Unary operator base class:

template <typename U>
class UnBTypeNameHV : public BTypeNameHV<U>
{
	BTypeNameHV<U>* m_pOp;
public:
	UnBTypeNameHV(const U& val, BTypeNameHV<U>* pOp):BTypeNameHV<U>(val),m_pOp(pOp)
	{
		m_pOp->incRef();
	}
	virtual void propagateChildren(typename Derivatives<U>::RecycleBin& bin)
	{
		m_pOp->decRef(bin,m_pOp);
	}
	virtual ~UnBTypeNameHV()
	{
		if (m_pOp) m_pOp->decRef(m_pOp);
	}
	BTypeNameHV<U>* op() { return m_pOp; }
};

// ADDITION:

template <typename U>
struct BTypeNameADD : public BinBTypeNameHV<U>
{
	BTypeNameADD(const U& val, BTypeNameHV<U>* pOp1, BTypeNameHV<U>* pOp2):BinBTypeNameHV<U>(val,pOp1,pOp2){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		this->op1()->add(bin,this->m_derivatives);
		this->op2()->add(bin,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameADD<U>&){} // not allowed
};
template <typename U, typename V>
struct BTypeNameADD1 : public UnBTypeNameHV<U>
{
	const V m_a;
	BTypeNameADD1(const U& val, const V& a, BTypeNameHV<U>* pOp2):UnBTypeNameHV<U>(val,pOp2),m_a(a){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		this->op()->add(bin,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameADD1<U,V>&){} // not allowed
};
template <typename U, typename V>
struct BTypeNameADD2 : public UnBTypeNameHV<U>
{
	const V m_b;
	BTypeNameADD2(const U& val, BTypeNameHV<U>* pOp1, const V& b):UnBTypeNameHV<U>(val,pOp1),m_b(b){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		this->op()->add(bin,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameADD2<U,V>&){} // not allowed
};
template <typename U>
BTypeName<U> operator+(const BTypeName<U>& val1, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNameADD<U>(val1.val()+val2.val(),val1.getBTypeNameHV(),val2.getBTypeNameHV())));
}
/*
template <typename U>
BTypeName<U> operator+(const typename Op<U>::Underlying& a, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameADD1<U,typename Op<U>::Underlying>(a+val2.val(), a, val2.getBTypeNameHV())
	));
}
template <typename U>
BTypeName<U> operator+(const BTypeName<U>& val1, const typename Op<U>::Underlying& b)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameADD2<U,typename Op<U>::Underlying>(val1.val()+b, val1.getBTypeNameHV(), b)
	));
}
template <typename U>
BTypeName<U> operator+(const typename Op<U>::Base& a, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameADD1<U,typename Op<U>::Base>(a+val2.val(), a, val2.getBTypeNameHV())
	));
}
template <typename U>
BTypeName<U> operator+(const BTypeName<U>& val1, const typename Op<U>::Base& b)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameADD2<U,typename Op<U>::Base>(val1.val()+b, val1.getBTypeNameHV(), b)
	));
}
*/
template <typename U, typename V>
BTypeName<U> operator+(const V& a, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameADD1<U,V>(a+val2.val(), a, val2.getBTypeNameHV())
	));
}
template <typename U, typename V>
BTypeName<U> operator+(const BTypeName<U>& val1, const V& b)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameADD2<U,V>(val1.val()+b, val1.getBTypeNameHV(), b)
	));
}

// SUBTRACTION:

template <typename U>
struct BTypeNameSUB : public BinBTypeNameHV<U>
{
	BTypeNameSUB(const U& val, BTypeNameHV<U>* pOp1, BTypeNameHV<U>* pOp2):BinBTypeNameHV<U>(val,pOp1,pOp2){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		this->op1()->add(bin,this->m_derivatives);
		this->op2()->sub(bin,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameSUB<U>&){} // not allowed
};
template <typename U, typename V>
struct BTypeNameSUB1 : public UnBTypeNameHV<U>
{
	const V m_a;
	BTypeNameSUB1(const U& val, const V& a, BTypeNameHV<U>* pOp2):UnBTypeNameHV<U>(val,pOp2),m_a(a){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		this->op()->sub(bin,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameSUB1<U,V>&){} // not allowed
};
template <typename U, typename V>
struct BTypeNameSUB2 : public UnBTypeNameHV<U>
{
	const V m_b;
	BTypeNameSUB2(const U& val, BTypeNameHV<U>* pOp1, const V& b):UnBTypeNameHV<U>(val,pOp1),m_b(b){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		this->op()->add(bin,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameSUB2<U,V>&){} // not allowed
};
template <typename U>
BTypeName<U> operator-(const BTypeName<U>& val1, const BTypeName<U>& val2)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNameSUB<U>(val1.val()-val2.val(),val1.getBTypeNameHV(),val2.getBTypeNameHV())));
}
/*
template <typename U>
BTypeName<U> operator-(const typename Op<U>::Underlying& a, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameSUB1<U,typename Op<U>::Underlying>(a-val2.val(), a, val2.getBTypeNameHV())
	));
}
template <typename U>
BTypeName<U> operator-(const BTypeName<U>& val1, const typename Op<U>::Underlying& b)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameSUB2<U,typename Op<U>::Underlying>(val1.val()-b, val1.getBTypeNameHV(), b)
	));
}
template <typename U>
BTypeName<U> operator-(const typename Op<U>::Base& a, const BTypeName<U>& val2)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameSUB1<U,typename Op<U>::Base>(a-val2.val(), a, val2.getBTypeNameHV())
	));
}
template <typename U>
BTypeName<U> operator-(const BTypeName<U>& val1, const typename Op<U>::Base& b)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameSUB2<U,typename Op<U>::Base>(val1.val()-b, val1.getBTypeNameHV(), b)
	));
}
*/
template <typename U, typename V>
BTypeName<U> operator-(const V& a, const BTypeName<U>& val2)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameSUB1<U,V>(a-val2.val(), a, val2.getBTypeNameHV())
	));
}
template <typename U, typename V>
BTypeName<U> operator-(const BTypeName<U>& val1, const V& b)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameSUB2<U,V>(val1.val()-b, val1.getBTypeNameHV(), b)
	));
}

// MULTIPLICATION:

template <typename U>
struct BTypeNameMUL : public BinBTypeNameHV<U>
{
	BTypeNameMUL(const U& val, BTypeNameHV<U>* pOp1, BTypeNameHV<U>* pOp2):BinBTypeNameHV<U>(val,pOp1,pOp2){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		this->op1()->add(bin,this->op2()->val(),this->m_derivatives);
		this->op2()->add(bin,this->op1()->val(),this->m_derivatives);
	}
private:
	void operator=(const BTypeNameMUL<U>&){} // not allowed
};
template <typename U, typename V>
struct BTypeNameMUL1 : public UnBTypeNameHV<U>
{
	const V m_a;
	BTypeNameMUL1(const U& val, const V& a, BTypeNameHV<U>* pOp2):UnBTypeNameHV<U>(val,pOp2),m_a(a){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		this->op()->add(bin,m_a,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameMUL1<U,V>&){} // not allowed
};
template <typename U, typename V>
struct BTypeNameMUL2 : public UnBTypeNameHV<U>
{
	const V m_b;
	BTypeNameMUL2(const U& val, BTypeNameHV<U>* pOp1, const V& b):UnBTypeNameHV<U>(val,pOp1),m_b(b){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		this->op()->add(bin,m_b,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameMUL2<U,V>&){} // not allowed
};
template <typename U>
BTypeName<U> operator*(const BTypeName<U>& val1, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNameMUL<U>(val1.val()*val2.val(),val1.getBTypeNameHV(),val2.getBTypeNameHV())));
}
/*
template <typename U>
BTypeName<U> operator*(const typename Op<U>::Underlying& a, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameMUL1<U,typename Op<U>::Underlying>(a*val2.val(), a, val2.getBTypeNameHV())
	));
}
template <typename U>
BTypeName<U> operator*(const BTypeName<U>& val1, const typename Op<U>::Underlying& b)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameMUL2<U,typename Op<U>::Underlying>(val1.val()*b, val1.getBTypeNameHV(), b)
	));
}
template <typename U>
BTypeName<U> operator*(const typename Op<U>::Base& a, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameMUL1<U,typename Op<U>::Base>(a*val2.val(), a, val2.getBTypeNameHV())
	));
}
template <typename U>
BTypeName<U> operator*(const BTypeName<U>& val1, const typename Op<U>::Base& b)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameMUL2<U,typename Op<U>::Base>(val1.val()*b, val1.getBTypeNameHV(), b)
	));
}
*/
template <typename U, typename V>
BTypeName<U> operator*(const V& a, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameMUL1<U,V>(a*val2.val(), a, val2.getBTypeNameHV())
	));
}
template <typename U, typename V>
BTypeName<U> operator*(const BTypeName<U>& val1, const V& b)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameMUL2<U,V>(val1.val()*b, val1.getBTypeNameHV(), b)
	));
}

// DIVISION:

template <typename U>
struct BTypeNameDIV : public BinBTypeNameHV<U>
{
	BTypeNameDIV(const U& val, BTypeNameHV<U>* pOp1, BTypeNameHV<U>* pOp2):BinBTypeNameHV<U>(val,pOp1,pOp2){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		U tmp=Op<U>::myInv(this->op2()->val());
		this->op1()->add(bin,tmp,this->m_derivatives);
		this->op2()->sub(bin,tmp*this->val(),this->m_derivatives);
	}
private:
	void operator=(const BTypeNameDIV<U>&){} // not allowed
};
template <typename U, typename V>
struct BTypeNameDIV1 : public UnBTypeNameHV<U>
{
	const V m_a;
	BTypeNameDIV1(const U& val, const V& a, BTypeNameHV<U>* pOp2):UnBTypeNameHV<U>(val,pOp2),m_a(a){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		this->op()->sub(bin,Op<U>::myInv(this->op()->val())*this->val(),this->m_derivatives);
	}
private:
	void operator=(const BTypeNameDIV1<U,V>&){} // not allowed
};
template <typename U, typename V>
struct BTypeNameDIV2 : public UnBTypeNameHV<U>
{
	const V m_b;
	BTypeNameDIV2(const U& val, BTypeNameHV<U>* pOp1, const V& b):UnBTypeNameHV<U>(val,pOp1),m_b(b){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		this->op()->add(bin,Op<V>::myInv(m_b),this->m_derivatives);
	}
private:
	void operator=(const BTypeNameDIV2<U,V>&){} // not allowed
};
template <typename U>
BTypeName<U> operator/(const BTypeName<U>& val1, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameDIV<U>(val1.val()/val2.val(),val1.getBTypeNameHV(),val2.getBTypeNameHV())
	));
}
/*
template <typename U>
BTypeName<U> operator/(const typename Op<U>::Underlying& a, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameDIV1<U,typename Op<U>::Underlying>(a/val2.val(), a, val2.getBTypeNameHV())
	));
}
template <typename U>
BTypeName<U> operator/(const BTypeName<U>& val1, const typename Op<U>::Underlying& b)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameDIV2<U,typename Op<U>::Underlying>(val1.val()/b, val1.getBTypeNameHV(), b)
	));
}
template <typename U>
BTypeName<U> operator/(const typename Op<U>::Base& a, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameDIV1<U,typename Op<U>::Base>(a/val2.val(), a, val2.getBTypeNameHV())
	));
}
template <typename U>
BTypeName<U> operator/(const BTypeName<U>& val1, const typename Op<U>::Base& b)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameDIV2<U,typename Op<U>::Base>(val1.val()/b, val1.getBTypeNameHV(), b)
	));
}
*/
template <typename U, typename V>
BTypeName<U> operator/(const V& a, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameDIV1<U,V>(a/val2.val(), a, val2.getBTypeNameHV())
	));
}
template <typename U, typename V>
BTypeName<U> operator/(const BTypeName<U>& val1, const V& b)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNameDIV2<U,V>(val1.val()/b, val1.getBTypeNameHV(), b)
	));
}

// COMPOUND ASSIGNMENTS:

template <typename U> BTypeName<U>& BTypeName<U>::operator+=(const BTypeName<U>& val) { return (*this)=(*this)+val; }
template <typename U> BTypeName<U>& BTypeName<U>::operator-=(const BTypeName<U>& val) { return (*this)=(*this)-val; }
template <typename U> BTypeName<U>& BTypeName<U>::operator*=(const BTypeName<U>& val) { return (*this)=(*this)*val; }
template <typename U> BTypeName<U>& BTypeName<U>::operator/=(const BTypeName<U>& val) { return (*this)=(*this)/val; }
template <typename U> template <typename V> BTypeName<U>& BTypeName<U>::operator+=(const V& val) { return (*this)=(*this)+val; }
template <typename U> template <typename V> BTypeName<U>& BTypeName<U>::operator-=(const V& val) { return (*this)=(*this)-val; }
template <typename U> template <typename V> BTypeName<U>& BTypeName<U>::operator*=(const V& val) { return (*this)=(*this)*val; }
template <typename U> template <typename V> BTypeName<U>& BTypeName<U>::operator/=(const V& val) { return (*this)=(*this)/val; }

// UNARY MINUS

template <typename U>
struct BTypeNameUMINUS : public UnBTypeNameHV<U>
{
	BTypeNameUMINUS(const U& val, BTypeNameHV<U>* pOp):UnBTypeNameHV<U>(val,pOp){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		this->op()->sub(bin,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameUMINUS<U>&){} // not allowed
};

template <typename U>
BTypeName<U> operator-(const BTypeName<U>& val)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNameUMINUS<U>(Op<U>::myNeg(val.val()),val.getBTypeNameHV())));
}

// UNARY PLUS

template <typename U>
struct BTypeNameUPLUS : public UnBTypeNameHV<U>
{
	BTypeNameUPLUS(const U& val, BTypeNameHV<U>* pOp):UnBTypeNameHV<U>(val,pOp){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		this->op()->add(bin,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameUPLUS<U>&){} // not allowed
};

template <typename U>
BTypeName<U> operator+(const BTypeName<U>& val)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNameUPLUS<U>(Op<U>::myPos(val.val()),val.getBTypeNameHV())));
}

// POWER

template <typename U>
struct BTypeNamePOW : public BinBTypeNameHV<U>
{
	BTypeNamePOW(const U& val, BTypeNameHV<U>* pOp1, BTypeNameHV<U>* pOp2):BinBTypeNameHV<U>(val,pOp1,pOp2){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		U tmp1(this->op2()->val() *  Op<U>::myPow(this->op1()->val(),this->op2()->val()-Op<U>::myOne()));
		U tmp2(this->val() * Op<U>::myLog(this->op1()->val()));
		this->op1()->add(bin,tmp1,this->m_derivatives);
		this->op2()->add(bin,tmp2,this->m_derivatives);
	}
private:
	void operator=(const BTypeNamePOW<U>&){} // not allowed
};
template <typename U, typename V>
struct BTypeNamePOW1 : public UnBTypeNameHV<U>
{
	const V m_a;
	BTypeNamePOW1(const U& val, const V& a, BTypeNameHV<U>* pOp2):UnBTypeNameHV<U>(val,pOp2),m_a(a){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		U tmp2(this->val() * Op<V>::myLog(m_a));
		this->op()->add(bin,tmp2,this->m_derivatives);
	}
private:
	void operator=(const BTypeNamePOW1<U,V>&){} // not allowed
};
template <typename U, typename V>
struct BTypeNamePOW2 : public UnBTypeNameHV<U>
{
	const V m_b;
	BTypeNamePOW2(const U& val, BTypeNameHV<U>* pOp1, const V& b):UnBTypeNameHV<U>(val,pOp1),m_b(b){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		U tmp1(m_b *  Op<U>::myPow(this->op()->val(),m_b-Op<V>::myOne()));
		this->op()->add(bin,tmp1,this->m_derivatives);
	}
private:
	void operator=(const BTypeNamePOW2<U,V>&){} // not allowed
};
template <typename U>
BTypeName<U> pow(const BTypeName<U>& val1, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNamePOW<U>(Op<U>::myPow(val1.val(),val2.val()),val1.getBTypeNameHV(),val2.getBTypeNameHV())));
}
/*
template <typename U>
BTypeName<U> pow(const typename Op<U>::Underlying& a, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNamePOW1<U,typename Op<U>::Underlying>(Op<U>::myPow(a,val2.val()), a, val2.getBTypeNameHV())
	));
}
template <typename U>
BTypeName<U> pow(const BTypeName<U>& val1, const typename Op<U>::Underlying& b)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNamePOW2<U,typename Op<U>::Underlying>(Op<U>::myPow(val1.val(),b), val1.getBTypeNameHV(), b)
	));
}
template <typename U>
BTypeName<U> pow(const typename Op<U>::Base& a, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNamePOW1<U,typename Op<U>::Base>(Op<U>::myPow(a,val2.val()), a, val2.getBTypeNameHV())
	));
}
template <typename U>
BTypeName<U> pow(const BTypeName<U>& val1, const typename Op<U>::Base& b)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNamePOW2<U,typename Op<U>::Base>(Op<U>::myPow(val1.val(),b), val1.getBTypeNameHV(), b)
	));
}
*/
template <typename U, typename V>
BTypeName<U> pow(const V& a, const BTypeName<U>& val2)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNamePOW1<U,V>(Op<U>::myPow(a,val2.val()), a, val2.getBTypeNameHV())
	));
}
template <typename U, typename V>
BTypeName<U> pow(const BTypeName<U>& val1, const V& b)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(
		new BTypeNamePOW2<U,V>(Op<U>::myPow(val1.val(),b), val1.getBTypeNameHV(), b)
	));
}

// SQR

template <typename U>
struct BTypeNameSQR : public UnBTypeNameHV<U>
{
	BTypeNameSQR(const U& val, BTypeNameHV<U>* pOp):UnBTypeNameHV<U>(val,pOp){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		U tmp(Op<U>::myTwo() * this->op()->val());
		this->op()->add(bin,tmp,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameSQR<U>&){} // not allowed
};
template <typename U>
BTypeName<U> sqr(const BTypeName<U>& val)
{
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNameSQR<U>(Op<U>::mySqr(val.val()), val.getBTypeNameHV())));
}

// SQRT

template <typename U>
struct BTypeNameSQRT : public UnBTypeNameHV<U>
{
	BTypeNameSQRT(const U& val, BTypeNameHV<U>* pOp):UnBTypeNameHV<U>(val,pOp){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		U tmp(Op<U>::myInv(this->val()*Op<U>::myTwo()));
		this->op()->add(bin,tmp,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameSQRT<U>&){} // not allowed
};
template <typename U>
BTypeName<U> sqrt(const BTypeName<U>& val)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNameSQRT<U>(Op<U>::mySqrt(val.val()), val.getBTypeNameHV())));
}

// EXP

template <typename U>
struct BTypeNameEXP : public UnBTypeNameHV<U>
{
	BTypeNameEXP(const U& val, BTypeNameHV<U>* pOp):UnBTypeNameHV<U>(val,pOp){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		this->op()->add(bin,this->val(),this->m_derivatives);
	}
private:
	void operator=(const BTypeNameEXP<U>&){} // not allowed
};
template <typename U>
BTypeName<U> exp(const BTypeName<U>& val)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNameEXP<U>(Op<U>::myExp(val.val()), val.getBTypeNameHV())));
}

// LOG

template <typename U>
struct BTypeNameLOG : public UnBTypeNameHV<U>
{
	BTypeNameLOG(const U& val, BTypeNameHV<U>* pOp):UnBTypeNameHV<U>(val,pOp){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		this->op()->add(bin,Op<U>::myInv(this->op()->val()),this->m_derivatives);
	}
private:
	void operator=(const BTypeNameLOG<U>&){} // not allowed
};
template <typename U>
BTypeName<U> log(const BTypeName<U>& val)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNameLOG<U>(Op<U>::myLog(val.val()), val.getBTypeNameHV())));
}

// SIN

template <typename U>
struct BTypeNameSIN : public UnBTypeNameHV<U>
{
	BTypeNameSIN(const U& val, BTypeNameHV<U>* pOp):UnBTypeNameHV<U>(val,pOp){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		U tmp(Op<U>::myCos(this->op()->val()));
		this->op()->add(bin,tmp,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameSIN<U>&){} // not allowed
};
template <typename U>
BTypeName<U> sin(const BTypeName<U>& val)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNameSIN<U>(Op<U>::mySin(val.val()), val.getBTypeNameHV())));
}

// COS

template <typename U>
struct BTypeNameCOS : public UnBTypeNameHV<U>
{
	BTypeNameCOS(const U& val, BTypeNameHV<U>* pOp):UnBTypeNameHV<U>(val,pOp){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		U tmp(Op<U>::mySin(this->op()->val()));
		this->op()->sub(bin,tmp,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameCOS<U>&){} // not allowed
};
template <typename U>
BTypeName<U> cos(const BTypeName<U>& val)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNameCOS<U>(Op<U>::myCos(val.val()), val.getBTypeNameHV())));
}

// TAN

template <typename U>
struct BTypeNameTAN : public UnBTypeNameHV<U>
{
	BTypeNameTAN(const U& val, BTypeNameHV<U>* pOp):UnBTypeNameHV<U>(val,pOp){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		U tmp(Op<U>::mySqr(this->val())+Op<U>::myOne());
		this->op()->add(bin,tmp,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameTAN<U>&){} // not allowed
};
template <typename U>
BTypeName<U> tan(const BTypeName<U>& val)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNameTAN<U>(Op<U>::myTan(val.val()), val.getBTypeNameHV())));
}

// ASIN

template <typename U>
struct BTypeNameASIN : public UnBTypeNameHV<U>
{
	BTypeNameASIN(const U& val, BTypeNameHV<U>* pOp):UnBTypeNameHV<U>(val,pOp){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		U tmp(Op<U>::myInv(Op<U>::mySqrt(Op<U>::myOne()-Op<U>::mySqr(this->op()->val()))));
		this->op()->add(bin,tmp,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameASIN<U>&){} // not allowed
};
template <typename U>
BTypeName<U> asin(const BTypeName<U>& val)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNameASIN<U>(Op<U>::myAsin(val.val()), val.getBTypeNameHV())));
}

// ACOS

template <typename U>
struct BTypeNameACOS : public UnBTypeNameHV<U>
{
	BTypeNameACOS(const U& val, BTypeNameHV<U>* pOp):UnBTypeNameHV<U>(val,pOp){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		U tmp(Op<U>::myInv(Op<U>::mySqrt(Op<U>::myOne()-Op<U>::mySqr(this->op()->val()))));
		this->op()->sub(bin,tmp,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameACOS<U>&){} // not allowed
};
template <typename U>
BTypeName<U> acos(const BTypeName<U>& val)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNameACOS<U>(Op<U>::myAcos(val.val()), val.getBTypeNameHV())));
}

// ATAN

template <typename U>
struct BTypeNameATAN : public UnBTypeNameHV<U>
{
	BTypeNameATAN(const U& val, BTypeNameHV<U>* pOp):UnBTypeNameHV<U>(val,pOp){}
	virtual void propagate(typename Derivatives<U>::RecycleBin& bin)
	{
		U tmp(Op<U>::myInv(Op<U>::mySqr(this->op()->val())+Op<U>::myOne()));
		this->op()->add(bin,tmp,this->m_derivatives);
	}
private:
	void operator=(const BTypeNameATAN<U>&){} // not allowed
};
template <typename U>
BTypeName<U> atan(const BTypeName<U>& val)
{ 
	return BTypeName<U>(static_cast<BTypeNameHV<U>*>(new BTypeNameATAN<U>(Op<U>::myAtan(val.val()), val.getBTypeNameHV())));
}

template <typename U> struct Op< BTypeName<U> >
{
	typedef BTypeName<U> T;
	typedef BTypeName<U> Underlying;
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

} //namespace fadbad

#endif
