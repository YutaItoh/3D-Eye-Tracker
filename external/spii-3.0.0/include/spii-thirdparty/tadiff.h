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
// COPYRIGHT NOTICE
// ***************************************************************

#ifndef _TADIFF_H
#define _TADIFF_H

#include <algorithm>

#ifndef MaxLength
#define MaxLength 40
#endif

#include "fadbad.h"

namespace fadbad
{

template <typename U, int N>
class TValues
{
	unsigned int m_n;
	U m_val[N];
public:	
	TValues():m_n(0){std::fill(m_val,m_val+N,Op<U>::myZero());}
	template <typename V> explicit TValues(const V& val):m_n(1){m_val[0]=val;std::fill(m_val+1,m_val+N,Op<U>::myZero());}
	U& operator[](const unsigned int i)
	{
		USER_ASSERT(i<N,"Index "<<i<<" out of bounds [0,"<<N<<"]")
		return m_val[i];
	}
	const U& operator[](const unsigned int i) const 
	{
		USER_ASSERT(i<N,"Index "<<i<<" out of bounds [0,"<<N<<"]")
		return m_val[i];
	}
	unsigned int length() const { return m_n; }
	unsigned int& length() { return m_n; }
	void reset(){ m_n=0; }
};

template <typename U, int N>
class TTypeNameHV // Heap Value
{
	TValues<U,N> m_val;
	mutable unsigned int m_rc;
protected:
	virtual ~TTypeNameHV(){}
public:
	TTypeNameHV():m_rc(0){}
	template <typename V> explicit TTypeNameHV(const V& val):m_val(val),m_rc(0){}
	const U& val(const unsigned int i) const { return m_val[i]; }
	U& val(const unsigned int i) { return m_val[i]; }
	unsigned int length() const { return m_val.length(); }
	unsigned int& length() { return m_val.length(); }
	void decRef(TTypeNameHV<U,N>*& pTTypeNameHV) const { if (--m_rc==0) { delete this; pTTypeNameHV=0;} }
	void incRef() const {++m_rc;}

	virtual void reset(){m_val.reset();}
	virtual unsigned int eval(const unsigned int k){return k+1;}
};

template <typename U, int N=MaxLength>
class TTypeName
{
private:
	struct SV // Stack Value refers to reference-counted Heap Value:
	{
		mutable TTypeNameHV<U,N>* m_pTTypeNameHV;
		SV(TTypeNameHV<U,N>* pTTypeNameHV):m_pTTypeNameHV(pTTypeNameHV){ m_pTTypeNameHV->incRef(); }
		SV(const typename TTypeName<U,N>::SV& sv):m_pTTypeNameHV(sv.m_pTTypeNameHV){ m_pTTypeNameHV->incRef(); }
		~SV(){ m_pTTypeNameHV->decRef(m_pTTypeNameHV); }
		TTypeNameHV<U,N>* getTTypeNameHV() const { return m_pTTypeNameHV; }
		void setTTypeNameHV(TTypeNameHV<U,N>* pTTypeNameHV) 
		{ 
			if (m_pTTypeNameHV!=pTTypeNameHV) 
			{ 
				m_pTTypeNameHV->decRef(m_pTTypeNameHV);
				m_pTTypeNameHV=pTTypeNameHV;
				m_pTTypeNameHV->incRef();
			}
		}
		const U& val() const { return m_pTTypeNameHV->val(0); }
		const U& val(const unsigned int i) const { return m_pTTypeNameHV->val(i); }
		unsigned int length() const { return m_pTTypeNameHV->length(); }
		unsigned int& length() { return m_pTTypeNameHV->length(); }	
		U& val(const unsigned int i) { return m_pTTypeNameHV->val(i); }

		void reset(){m_pTTypeNameHV->reset();}
		unsigned int eval(const unsigned int i){return m_pTTypeNameHV->eval(i);}
	} m_sv;
public:
	typedef U UnderlyingType;
	TTypeName():m_sv(new TTypeNameHV<U,N>()){}
	TTypeName(TTypeNameHV<U,N>* pTTypeNameHV):m_sv(pTTypeNameHV){}
	explicit TTypeName(const typename TTypeName<U,N>::SV& sv):m_sv(sv){}
	template <typename V> /*explicit*/ TTypeName(const V& val):m_sv(new TTypeNameHV<U,N>(val)){m_sv.length()=N;}
	TTypeName<U,N>& operator=(const TTypeName<U,N>& val) 
	{
		if (this==&val) return *this;
		m_sv.setTTypeNameHV(val.m_sv.getTTypeNameHV());
		return *this;
	}
	template <typename V> TTypeName<U,N>& operator=(const V& val) 
	{ 
		m_sv.setTTypeNameHV(new TTypeNameHV<U,N>(val));
		m_sv.length()=N;
		return *this; 
	}
	TTypeNameHV<U,N>* getTTypeNameHV() const { return m_sv.getTTypeNameHV(); }
	void setTTypeNameHV(const TTypeNameHV<U,N>* pTTypeNameHV) { m_sv.setTTypeNameHV(pTTypeNameHV); }
	const U& val() const { return m_sv.val(); }
	unsigned int length() const { return m_sv.length(); }	
	const U& operator[](const unsigned int i) const { return m_sv.val(i); }
	U& operator[](const unsigned int i) { if (i>=m_sv.length()) m_sv.length()=i+1; return m_sv.val(i);}
	
	TTypeName<U,N>& operator+=(const TTypeName<U,N>& val);
	TTypeName<U,N>& operator-=(const TTypeName<U,N>& val);
	TTypeName<U,N>& operator*=(const TTypeName<U,N>& val);
	TTypeName<U,N>& operator/=(const TTypeName<U,N>& val);
	template <typename V> TTypeName<U,N>& operator+=(const V& val);
	template <typename V> TTypeName<U,N>& operator-=(const V& val);
	template <typename V> TTypeName<U,N>& operator*=(const V& val);
	template <typename V> TTypeName<U,N>& operator/=(const V& val);	

	void reset(){m_sv.reset();}
	unsigned int eval(const unsigned int i){return m_sv.eval(i);}
};

template <typename U, int N> bool operator==(const TTypeName<U,N>& val1, const TTypeName<U,N>& val2) { return Op<U>::myEq(val1.val(),val2.val()); }
template <typename U, int N> bool operator!=(const TTypeName<U,N>& val1, const TTypeName<U,N>& val2) { return Op<U>::myNe(val1.val(),val2.val()); }
template <typename U, int N> bool operator<(const TTypeName<U,N>& val1, const TTypeName<U,N>& val2) { return Op<U>::myLt(val1.val(),val2.val()); }
template <typename U, int N> bool operator<=(const TTypeName<U,N>& val1, const TTypeName<U,N>& val2) { return Op<U>::myLe(val1.val(),val2.val()); }
template <typename U, int N> bool operator>(const TTypeName<U,N>& val1, const TTypeName<U,N>& val2) { return Op<U>::myGt(val1.val(),val2.val()); }
template <typename U, int N> bool operator>=(const TTypeName<U,N>& val1, const TTypeName<U,N>& val2) { return Op<U>::myGe(val1.val(),val2.val()); }
template <typename U, int N, typename V> bool operator==(const TTypeName<U,N>& val1, const V& val2) { return Op<U>::myEq(val1.val(),val2); }
template <typename U, int N, typename V> bool operator==(const V& val1, const TTypeName<U,N>& val2) { return Op<U>::myEq(val1,val2.val()); }
template <typename U, int N, typename V> bool operator!=(const TTypeName<U,N>& val1, const V& val2) { return Op<U>::myNe(val1.val(),val2); }
template <typename U, int N, typename V> bool operator!=(const V& val1, const TTypeName<U,N>& val2) { return Op<U>::myNe(val1,val2.val()); }
template <typename U, int N, typename V> bool operator<(const TTypeName<U,N>& val1, const V& val2) { return Op<U>::myLt(val1.val(),val2); }
template <typename U, int N, typename V> bool operator<(const V& val1, const TTypeName<U,N>& val2) { return Op<U>::myLt(val1,val2.val()); }
template <typename U, int N, typename V> bool operator<=(const TTypeName<U,N>& val1, const V& val2) { return Op<U>::myLe(val1.val(),val2); }
template <typename U, int N, typename V> bool operator<=(const V& val1, const TTypeName<U,N>& val2) { return Op<U>::myLe(val1,val2.val()); }
template <typename U, int N, typename V> bool operator>(const TTypeName<U,N>& val1, const V& val2) { return Op<U>::myGt(val1.val(),val2); }
template <typename U, int N, typename V> bool operator>(const V& val1, const TTypeName<U,N>& val2) { return Op<U>::myGt(val1,val2.val()); }
template <typename U, int N, typename V> bool operator>=(const TTypeName<U,N>& val1, const V& val2) { return Op<U>::myGe(val1.val(),val2); }
template <typename U, int N, typename V> bool operator>=(const V& val1, const TTypeName<U,N>& val2) { return Op<U>::myGe(val1,val2.val()); }

// Binary operator base class:

template <typename U, int N>
class BinTTypeNameHV : public TTypeNameHV<U,N>
{
	TTypeNameHV<U,N>* m_pOp1;
	TTypeNameHV<U,N>* m_pOp2;
public:
	BinTTypeNameHV(const U& val, TTypeNameHV<U,N>* pOp1, TTypeNameHV<U,N>* pOp2):TTypeNameHV<U,N>(val),m_pOp1(pOp1),m_pOp2(pOp2)
	{
		m_pOp1->incRef();m_pOp2->incRef();
	}
	BinTTypeNameHV(TTypeNameHV<U,N>* pOp1, TTypeNameHV<U,N>* pOp2):TTypeNameHV<U,N>(),m_pOp1(pOp1),m_pOp2(pOp2)
	{
		m_pOp1->incRef();m_pOp2->incRef();
	}
	virtual ~BinTTypeNameHV()
	{
		m_pOp1->decRef(m_pOp1);m_pOp2->decRef(m_pOp2);
	}
	TTypeNameHV<U,N>* op1() { return m_pOp1; }
	TTypeNameHV<U,N>* op2() { return m_pOp2; }

	unsigned int op1Eval(const unsigned int k){return this->op1()->eval(k);}
	unsigned int op2Eval(const unsigned int k){return this->op2()->eval(k);}
	const U& op1Val(const unsigned int k) {return this->op1()->val(k);}
	const U& op2Val(const unsigned int k) {return this->op2()->val(k);}
	void reset(){op1()->reset();op2()->reset();TTypeNameHV<U,N>::reset();}
};

// Unary operator base class:

template <typename U, int N>
class UnTTypeNameHV : public TTypeNameHV<U,N>
{
	TTypeNameHV<U,N>* m_pOp;
public:
	UnTTypeNameHV(const U& val, TTypeNameHV<U,N>* pOp):TTypeNameHV<U,N>(val),m_pOp(pOp)
	{
		m_pOp->incRef();
	}
	UnTTypeNameHV(TTypeNameHV<U,N>* pOp):TTypeNameHV<U,N>(),m_pOp(pOp)
	{
		m_pOp->incRef();
	}
	virtual ~UnTTypeNameHV()
	{
		m_pOp->decRef(m_pOp);
	}
	TTypeNameHV<U,N>* op() { return m_pOp; }

	unsigned int opEval(const unsigned int k){return this->op()->eval(k);}
	const U& opVal(const unsigned int k) {return this->op()->val(k);}
	void reset(){op()->reset();TTypeNameHV<U,N>::reset();}
};

// ADDITION:

template <typename U, int N>
struct TTypeNameADD : public BinTTypeNameHV<U,N>
{
	TTypeNameADD(const U& val, TTypeNameHV<U,N>* pOp1, TTypeNameHV<U,N>* pOp2):BinTTypeNameHV<U,N>(val,pOp1,pOp2){}
	TTypeNameADD(TTypeNameHV<U,N>* pOp1, TTypeNameHV<U,N>* pOp2):BinTTypeNameHV<U,N>(pOp1,pOp2){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=std::min(this->op1Eval(k),this->op2Eval(k));
		for(unsigned int i=this->length();i<l;++i) this->val(i)=this->op1Val(i)+this->op2Val(i);
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameADD<U,N>&){} // not allowed
};
template <typename U, int N, typename V>
struct TTypeNameADD1 : public UnTTypeNameHV<U,N>
{
	const V m_a;
	TTypeNameADD1(const U& val, const V& a, TTypeNameHV<U,N>* pOp2):UnTTypeNameHV<U,N>(val,pOp2),m_a(a){}
	TTypeNameADD1(const V& a, TTypeNameHV<U,N>* pOp2):UnTTypeNameHV<U,N>(pOp2),m_a(a){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		if (0==this->length()) { this->val(0)=m_a+this->opVal(0); this->length()=1; }
		for(unsigned int i=this->length();i<l;++i) this->val(i)=this->opVal(i);
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameADD1<U,N,V>&){} // not allowed
};
template <typename U, int N, typename V>
struct TTypeNameADD2 : public UnTTypeNameHV<U,N>
{
	const V m_b;
	TTypeNameADD2(const U& val, TTypeNameHV<U,N>* pOp1, const V& b):UnTTypeNameHV<U,N>(val,pOp1),m_b(b){}
	TTypeNameADD2(TTypeNameHV<U,N>* pOp1, const V& b):UnTTypeNameHV<U,N>(pOp1),m_b(b){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		if (0==this->length()) { this->val(0)=this->opVal(0)+m_b; this->length()=1; }
		for(unsigned int i=this->length();i<l;++i) this->val(i)=this->opVal(i);
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameADD2<U,N,V>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> operator+(const TTypeName<U,N>& val1, const TTypeName<U,N>& val2)
{
	TTypeNameHV<U,N>* pHV=val1.length()>0 && val2.length()>0 ?
		new TTypeNameADD<U,N>(val1.val()+val2.val(),val1.getTTypeNameHV(),val2.getTTypeNameHV()):
		new TTypeNameADD<U,N>(val1.getTTypeNameHV(),val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
/*
template <typename U, int N>
TTypeName<U,N> operator+(const typename Op<U>::Underlying& a, const TTypeName<U,N>& val2)
{
	TTypeNameHV<U,N>* pHV=val2.length()>0 ?
		new TTypeNameADD1<U,N,typename Op<U>::Underlying>(a+val2.val(), a, val2.getTTypeNameHV()):
		new TTypeNameADD1<U,N,typename Op<U>::Underlying>(a, val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N>
TTypeName<U,N> operator+(const TTypeName<U,N>& val1, const typename Op<U>::Underlying& b)
{
	TTypeNameHV<U,N>* pHV=val1.length()>0?
		new TTypeNameADD2<U,N,typename Op<U>::Underlying>(val1.val()+b, val1.getTTypeNameHV(), b):
		new TTypeNameADD2<U,N,typename Op<U>::Underlying>(val1.getTTypeNameHV(), b);
	return TTypeName<U,N>(pHV);
}
template <typename U, int N>
TTypeName<U,N> operator+(const typename Op<U>::Base& a, const TTypeName<U,N>& val2)
{
	TTypeNameHV<U,N>* pHV=val2.length()>0 ?
		new TTypeNameADD1<U,N,typename Op<U>::Base>(a+val2.val(), a, val2.getTTypeNameHV()):
		new TTypeNameADD1<U,N,typename Op<U>::Base>(a, val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N>
TTypeName<U,N> operator+(const TTypeName<U,N>& val1, const typename Op<U>::Base& b)
{
	TTypeNameHV<U,N>* pHV=val1.length()>0?
		new TTypeNameADD2<U,N,typename Op<U>::Base>(val1.val()+b, val1.getTTypeNameHV(), b):
		new TTypeNameADD2<U,N,typename Op<U>::Base>(val1.getTTypeNameHV(), b);
	return TTypeName<U,N>(pHV);
}
*/
template <typename U, int N, typename V>
TTypeName<U,N> operator+(const V& a, const TTypeName<U,N>& val2)
{
	TTypeNameHV<U,N>* pHV=val2.length()>0 ?
		new TTypeNameADD1<U,N,V>(a+val2.val(), a, val2.getTTypeNameHV()):
		new TTypeNameADD1<U,N,V>(a, val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N, typename V>
TTypeName<U,N> operator+(const TTypeName<U,N>& val1, const V& b)
{
	TTypeNameHV<U,N>* pHV=val1.length()>0?
		new TTypeNameADD2<U,N,V>(val1.val()+b, val1.getTTypeNameHV(), b):
		new TTypeNameADD2<U,N,V>(val1.getTTypeNameHV(), b);
	return TTypeName<U,N>(pHV);
}

// SUBTRACTION:

template <typename U, int N>
struct TTypeNameSUB : public BinTTypeNameHV<U,N>
{
	TTypeNameSUB(const U& val, TTypeNameHV<U,N>* pOp1, TTypeNameHV<U,N>* pOp2):BinTTypeNameHV<U,N>(val,pOp1,pOp2){}
	TTypeNameSUB(TTypeNameHV<U,N>* pOp1, TTypeNameHV<U,N>* pOp2):BinTTypeNameHV<U,N>(pOp1,pOp2){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=std::min(this->op1Eval(k),this->op2Eval(k));
		for(unsigned int i=this->length();i<l;++i) this->val(i)=this->op1Val(i)-this->op2Val(i);
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameSUB<U,N>&){} // not allowed
};
template <typename U, int N, typename V>
struct TTypeNameSUB1 : public UnTTypeNameHV<U,N>
{
	const V m_a;
	TTypeNameSUB1(const U& val, const V& a, TTypeNameHV<U,N>* pOp2):UnTTypeNameHV<U,N>(val,pOp2),m_a(a){}
	TTypeNameSUB1(const V& a, TTypeNameHV<U,N>* pOp2):UnTTypeNameHV<U,N>(pOp2),m_a(a){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		if (0==this->length()) { this->val(0)=m_a-this->opVal(0); this->length()=1; }
		for(unsigned int i=this->length();i<l;++i) this->val(i)=Op<U>::myNeg(this->opVal(i));
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameSUB1<U,N,V>&){} // not allowed
};
template <typename U, int N, typename V>
struct TTypeNameSUB2 : public UnTTypeNameHV<U,N>
{
	const V m_b;
	TTypeNameSUB2(const U& val, TTypeNameHV<U,N>* pOp1, const V& b):UnTTypeNameHV<U,N>(val,pOp1),m_b(b){}
	TTypeNameSUB2(TTypeNameHV<U,N>* pOp1, const V& b):UnTTypeNameHV<U,N>(pOp1),m_b(b){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		if (0==this->length()) { this->val(0)=this->opVal(0)-m_b; this->length()=1; }
		for(unsigned int i=this->length();i<l;++i) this->val(i)=this->opVal(i);
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameSUB2<U,N,V>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> operator-(const TTypeName<U,N>& val1, const TTypeName<U,N>& val2)
{ 
	TTypeNameHV<U,N>* pHV=val1.length()>0 && val2.length()>0 ?
		new TTypeNameSUB<U,N>(val1.val()-val2.val(),val1.getTTypeNameHV(),val2.getTTypeNameHV()):
		new TTypeNameSUB<U,N>(val1.getTTypeNameHV(),val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
/*
template <typename U, int N>
TTypeName<U,N> operator-(const typename Op<U>::Underlying& a, const TTypeName<U,N>& val2)
{
	TTypeNameHV<U,N>* pHV=val2.length()>0 ?
		new TTypeNameSUB1<U,N,typename Op<U>::Underlying>(a-val2.val(), a, val2.getTTypeNameHV()):
		new TTypeNameSUB1<U,N,typename Op<U>::Underlying>(a, val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N>
TTypeName<U,N> operator-(const TTypeName<U,N>& val1, const typename Op<U>::Underlying& b)
{
	TTypeNameHV<U,N>* pHV=val1.length()>0 ?
		new TTypeNameSUB2<U,N,typename Op<U>::Underlying>(val1.val()-b, val1.getTTypeNameHV(), b):
		new TTypeNameSUB2<U,N,typename Op<U>::Underlying>(val1.getTTypeNameHV(), b);
	return TTypeName<U,N>(pHV);
}
template <typename U, int N>
TTypeName<U,N> operator-(const typename Op<U>::Base& a, const TTypeName<U,N>& val2)
{
	TTypeNameHV<U,N>* pHV=val2.length()>0 ?
		new TTypeNameSUB1<U,N,typename Op<U>::Base>(a-val2.val(), a, val2.getTTypeNameHV()):
		new TTypeNameSUB1<U,N,typename Op<U>::Base>(a, val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N>
TTypeName<U,N> operator-(const TTypeName<U,N>& val1, const typename Op<U>::Base& b)
{
	TTypeNameHV<U,N>* pHV=val1.length()>0 ?
		new TTypeNameSUB2<U,N,typename Op<U>::Base>(val1.val()-b, val1.getTTypeNameHV(), b):
		new TTypeNameSUB2<U,N,typename Op<U>::Base>(val1.getTTypeNameHV(), b);
	return TTypeName<U,N>(pHV);
}
*/
template <typename U, int N, typename V>
TTypeName<U,N> operator-(const V& a, const TTypeName<U,N>& val2)
{
	TTypeNameHV<U,N>* pHV=val2.length()>0 ?
		new TTypeNameSUB1<U,N,V>(a-val2.val(), a, val2.getTTypeNameHV()):
		new TTypeNameSUB1<U,N,V>(a, val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N, typename V>
TTypeName<U,N> operator-(const TTypeName<U,N>& val1, const V& b)
{
	TTypeNameHV<U,N>* pHV=val1.length()>0 ?
		new TTypeNameSUB2<U,N,V>(val1.val()-b, val1.getTTypeNameHV(), b):
		new TTypeNameSUB2<U,N,V>(val1.getTTypeNameHV(), b);
	return TTypeName<U,N>(pHV);
}

// MULTIPLICATION:

template <typename U, int N>
struct TTypeNameMUL : public BinTTypeNameHV<U,N>
{
	TTypeNameMUL(const U& val, TTypeNameHV<U,N>* pOp1, TTypeNameHV<U,N>* pOp2):BinTTypeNameHV<U,N>(val,pOp1,pOp2){}
	TTypeNameMUL(TTypeNameHV<U,N>* pOp1, TTypeNameHV<U,N>* pOp2):BinTTypeNameHV<U,N>(pOp1,pOp2){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=std::min(this->op1Eval(k),this->op2Eval(k));
		for(unsigned int i=this->length();i<l;++i)
		{
			this->val(i)=Op<U>::myZero();
			for(unsigned int j=0;j<=i;++j)
				Op<U>::myCadd(this->val(i),this->op1Val(j)*this->op2Val(i-j));
		}
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameMUL<U,N>&){} // not allowed
};
template <typename U, int N, typename V>
struct TTypeNameMUL1 : public UnTTypeNameHV<U,N>
{
	const V m_a;
	TTypeNameMUL1(const U& val, const V& a, TTypeNameHV<U,N>* pOp2):UnTTypeNameHV<U,N>(val,pOp2),m_a(a){}
	TTypeNameMUL1(const V& a, TTypeNameHV<U,N>* pOp2):UnTTypeNameHV<U,N>(pOp2),m_a(a){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		for(unsigned int i=this->length();i<l;++i) this->val(i)=m_a*this->opVal(i);
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameMUL1<U,N,V>&){} // not allowed
};
template <typename U, int N, typename V>
struct TTypeNameMUL2 : public UnTTypeNameHV<U,N>
{
	const V m_b;
	TTypeNameMUL2(const U& val, TTypeNameHV<U,N>* pOp1, const V& b):UnTTypeNameHV<U,N>(val,pOp1),m_b(b){}
	TTypeNameMUL2(TTypeNameHV<U,N>* pOp1, const V& b):UnTTypeNameHV<U,N>(pOp1),m_b(b){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		for(unsigned int i=this->length();i<l;++i) this->val(i)=this->opVal(i)*m_b;
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameMUL2<U,N,V>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> operator*(const TTypeName<U,N>& val1, const TTypeName<U,N>& val2)
{
	TTypeNameHV<U,N>* pHV=val1.length()>0 && val2.length()>0 ?
		new TTypeNameMUL<U,N>(val1.val()*val2.val(),val1.getTTypeNameHV(),val2.getTTypeNameHV()):
		new TTypeNameMUL<U,N>(val1.getTTypeNameHV(),val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
/*
template <typename U, int N>
TTypeName<U,N> operator*(const typename Op<U>::Underlying& a, const TTypeName<U,N>& val2)
{
	TTypeNameHV<U,N>* pHV=val2.length()>0 ?
		new TTypeNameMUL1<U,N,typename Op<U>::Underlying>(a*val2.val(), a, val2.getTTypeNameHV()):
		new TTypeNameMUL1<U,N,typename Op<U>::Underlying>(a, val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N>
TTypeName<U,N> operator*(const TTypeName<U,N>& val1, const typename Op<U>::Underlying& b)
{
	TTypeNameHV<U,N>* pHV=val1.length()>0 ?
		new TTypeNameMUL2<U,N,typename Op<U>::Underlying>(val1.val()*b, val1.getTTypeNameHV(), b):
		new TTypeNameMUL2<U,N,typename Op<U>::Underlying>(val1.getTTypeNameHV(), b);
	return TTypeName<U,N>(pHV);
}
template <typename U, int N>
TTypeName<U,N> operator*(const typename Op<U>::Base& a, const TTypeName<U,N>& val2)
{
	TTypeNameHV<U,N>* pHV=val2.length()>0 ?
		new TTypeNameMUL1<U,N,typename Op<U>::Base>(a*val2.val(), a, val2.getTTypeNameHV()):
		new TTypeNameMUL1<U,N,typename Op<U>::Base>(a, val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N>
TTypeName<U,N> operator*(const TTypeName<U,N>& val1, const typename Op<U>::Base& b)
{
	TTypeNameHV<U,N>* pHV=val1.length()>0 ?
		new TTypeNameMUL2<U,N,typename Op<U>::Base>(val1.val()*b, val1.getTTypeNameHV(), b):
		new TTypeNameMUL2<U,N,typename Op<U>::Base>(val1.getTTypeNameHV(), b);
	return TTypeName<U,N>(pHV);
}
*/
template <typename U, int N, typename V>
TTypeName<U,N> operator*(const V& a, const TTypeName<U,N>& val2)
{
	TTypeNameHV<U,N>* pHV=val2.length()>0 ?
		new TTypeNameMUL1<U,N,V>(a*val2.val(), a, val2.getTTypeNameHV()):
		new TTypeNameMUL1<U,N,V>(a, val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N, typename V>
TTypeName<U,N> operator*(const TTypeName<U,N>& val1, const V& b)
{
	TTypeNameHV<U,N>* pHV=val1.length()>0 ?
		new TTypeNameMUL2<U,N,V>(val1.val()*b, val1.getTTypeNameHV(), b):
		new TTypeNameMUL2<U,N,V>(val1.getTTypeNameHV(), b);
	return TTypeName<U,N>(pHV);
}

// DIVISION:

template <typename U, int N>
struct TTypeNameDIV : public BinTTypeNameHV<U,N>
{
	TTypeNameDIV(const U& val, TTypeNameHV<U,N>* pOp1, TTypeNameHV<U,N>* pOp2):BinTTypeNameHV<U,N>(val,pOp1,pOp2){}
	TTypeNameDIV(TTypeNameHV<U,N>* pOp1, TTypeNameHV<U,N>* pOp2):BinTTypeNameHV<U,N>(pOp1,pOp2){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=std::min(this->op1Eval(k),this->op2Eval(k));
		for(unsigned int i=this->length();i<l;++i)
		{
			this->val(i)=this->op1Val(i);
			for(unsigned int j=1;j<=i;++j) Op<U>::myCsub(this->val(i),this->op2Val(j)*this->val(i-j));
			Op<U>::myCdiv(this->val(i), this->op2Val(0));
		}
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameDIV<U,N>&){} // not allowed
};
template <typename U, int N, typename V>
struct TTypeNameDIV1 : public UnTTypeNameHV<U,N>
{
	const V m_a;
	TTypeNameDIV1(const U& val, const V& a, TTypeNameHV<U,N>* pOp2):UnTTypeNameHV<U,N>(val,pOp2),m_a(a){}
	TTypeNameDIV1(const V& a, TTypeNameHV<U,N>* pOp2):UnTTypeNameHV<U,N>(pOp2),m_a(a){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		if (0==this->length()) { this->val(0)=m_a/this->opVal(0); this->length()=1; }
		for(unsigned int i=this->length();i<l;++i)
		{
			this->val(i)=Op<U>::myZero();
			for(unsigned int j=1;j<=i;++j) Op<U>::myCsub(this->val(i),this->opVal(j)*this->val(i-j));
			Op<U>::myCdiv(this->val(i), this->opVal(0));
		}
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameDIV1<U,N,V>&){} // not allowed
};
template <typename U, int N, typename V>
struct TTypeNameDIV2 : public UnTTypeNameHV<U,N>
{
	const V m_b;
	TTypeNameDIV2(const U& val, TTypeNameHV<U,N>* pOp1, const V& b):UnTTypeNameHV<U,N>(val,pOp1),m_b(b){}
	TTypeNameDIV2(TTypeNameHV<U,N>* pOp1, const V& b):UnTTypeNameHV<U,N>(pOp1),m_b(b){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		for(unsigned int i=this->length();i<l;++i) this->val(i)=this->opVal(i)/m_b;
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameDIV2<U,N,V>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> operator/(const TTypeName<U,N>& val1, const TTypeName<U,N>& val2)
{
	TTypeNameHV<U,N>* pHV=val1.length()>0 && val2.length()>0 ?
		new TTypeNameDIV<U,N>(val1.val()/val2.val(),val1.getTTypeNameHV(),val2.getTTypeNameHV()):
		new TTypeNameDIV<U,N>(val1.getTTypeNameHV(),val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
/*
template <typename U, int N>
TTypeName<U,N> operator/(const typename Op<U>::Underlying& a, const TTypeName<U,N>& val2)
{
	TTypeNameHV<U,N>* pHV=val2.length()>0 ?
		new TTypeNameDIV1<U,N,typename Op<U>::Underlying>(a/val2.val(), a, val2.getTTypeNameHV()):
		new TTypeNameDIV1<U,N,typename Op<U>::Underlying>(a, val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N>
TTypeName<U,N> operator/(const TTypeName<U,N>& val1, const typename Op<U>::Underlying& b)
{
	TTypeNameHV<U,N>* pHV=val1.length()>0 ?
		new TTypeNameDIV2<U,N,typename Op<U>::Underlying>(val1.val()/b, val1.getTTypeNameHV(), b):
		new TTypeNameDIV2<U,N,typename Op<U>::Underlying>(val1.getTTypeNameHV(), b);
	return TTypeName<U,N>(pHV);
}
template <typename U, int N>
TTypeName<U,N> operator/(const typename Op<U>::Base& a, const TTypeName<U,N>& val2)
{
	TTypeNameHV<U,N>* pHV=val2.length()>0 ?
		new TTypeNameDIV1<U,N,typename Op<U>::Base>(a/val2.val(), a, val2.getTTypeNameHV()):
		new TTypeNameDIV1<U,N,typename Op<U>::Base>(a, val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N>
TTypeName<U,N> operator/(const TTypeName<U,N>& val1, const typename Op<U>::Base& b)
{
	TTypeNameHV<U,N>* pHV=val1.length()>0 ?
		new TTypeNameDIV2<U,N,typename Op<U>::Base>(val1.val()/b, val1.getTTypeNameHV(), b):
		new TTypeNameDIV2<U,N,typename Op<U>::Base>(val1.getTTypeNameHV(), b);
	return TTypeName<U,N>(pHV);
}
*/
template <typename U, int N, typename V>
TTypeName<U,N> operator/(const V& a, const TTypeName<U,N>& val2)
{
	TTypeNameHV<U,N>* pHV=val2.length()>0 ?
		new TTypeNameDIV1<U,N,V>(a/val2.val(), a, val2.getTTypeNameHV()):
		new TTypeNameDIV1<U,N,V>(a, val2.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N, typename V>
TTypeName<U,N> operator/(const TTypeName<U,N>& val1, const V& b)
{
	TTypeNameHV<U,N>* pHV=val1.length()>0 ?
		new TTypeNameDIV2<U,N,V>(val1.val()/b, val1.getTTypeNameHV(), b):
		new TTypeNameDIV2<U,N,V>(val1.getTTypeNameHV(), b);
	return TTypeName<U,N>(pHV);
}

// COMPOUND ASSIGNMENTS:

template <typename U, int N> TTypeName<U,N>& TTypeName<U,N>::operator+=(const TTypeName<U,N>& val) { return (*this)=(*this)+val; }
template <typename U, int N> TTypeName<U,N>& TTypeName<U,N>::operator-=(const TTypeName<U,N>& val) { return (*this)=(*this)-val; } 
template <typename U, int N> TTypeName<U,N>& TTypeName<U,N>::operator*=(const TTypeName<U,N>& val) { return (*this)=(*this)*val; }
template <typename U, int N> TTypeName<U,N>& TTypeName<U,N>::operator/=(const TTypeName<U,N>& val) { return (*this)=(*this)/val; }
template <typename U, int N> template <typename V> TTypeName<U,N>& TTypeName<U,N>::operator+=(const V& val) { return (*this)=(*this)+val; }
template <typename U, int N> template <typename V> TTypeName<U,N>& TTypeName<U,N>::operator-=(const V& val) { return (*this)=(*this)-val; }
template <typename U, int N> template <typename V> TTypeName<U,N>& TTypeName<U,N>::operator*=(const V& val) { return (*this)=(*this)*val; }
template <typename U, int N> template <typename V> TTypeName<U,N>& TTypeName<U,N>::operator/=(const V& val) { return (*this)=(*this)/val; }

// UNARY MINUS

template <typename U, int N>
struct TTypeNameUMINUS : public UnTTypeNameHV<U,N>
{
	TTypeNameUMINUS(const U& val, TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(val,pOp){}
	TTypeNameUMINUS(TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(pOp){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		for(unsigned int i=this->length();i<l;++i) this->val(i)=Op<U>::myNeg(this->opVal(i));
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameUMINUS<U,N>&){} // not allowed
};

template <typename U, int N>
TTypeName<U,N> operator-(const TTypeName<U,N>& val)
{
	TTypeNameHV<U,N>* pHV=val.length()>0 ? 
		new TTypeNameUMINUS<U,N>(Op<U>::myNeg(val.val()),val.getTTypeNameHV()) : 
		new TTypeNameUMINUS<U,N>(val.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}

// UNARY PLUS

template <typename U, int N>
struct TTypeNameUPLUS : public UnTTypeNameHV<U,N>
{
	TTypeNameUPLUS(const U& val, TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(val,pOp){}
	TTypeNameUPLUS(TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(pOp){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		for(unsigned int i=this->length();i<l;++i) this->val(i)=+this->opVal(i);
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameUPLUS<U,N>&){} // not allowed
};

template <typename U, int N>
TTypeName<U,N> operator+(const TTypeName<U,N>& val)
{
	TTypeNameHV<U,N>* pHV=val.length()>0 ? 
		new TTypeNameUPLUS<U,N>(+val.val(),val.getTTypeNameHV()) : 
		new TTypeNameUPLUS<U,N>(val.getTTypeNameHV());	
	return TTypeName<U,N>(pHV);
}

// POWER

template <typename U, int N>
struct TTypeNamePOW : public UnTTypeNameHV<U,N>
{
	TTypeNamePOW(const U& val, TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(val,pOp){}
	TTypeNamePOW(TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(pOp){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		for(unsigned int i=this->length();i<l;++i) this->val(i)=this->opVal(i);
		return this->length()=l;
	}
private:
	void operator=(const TTypeNamePOW<U,N>&){} // not allowed
};
template <typename U, int N, typename V>
struct TTypeNamePOW1 : public UnTTypeNameHV<U,N>
{
	TTypeNamePOW1(const U& val, TTypeNameHV<U,N>* pOp2):UnTTypeNameHV<U,N>(val,pOp2){}
	TTypeNamePOW1(TTypeNameHV<U,N>* pOp2):UnTTypeNameHV<U,N>(pOp2){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		for(unsigned int i=this->length();i<l;++i) this->val(i)=this->opVal(i);
		return this->length()=l;
	}
private:
	void operator=(const TTypeNamePOW1<U,N,V>&){} // not allowed
};
template <typename U, int N, typename V>
struct TTypeNamePOW2 : public UnTTypeNameHV<U,N>
{
	TTypeNamePOW2(const U& val, TTypeNameHV<U,N>* pOp1):UnTTypeNameHV<U,N>(val,pOp1){}
	TTypeNamePOW2(TTypeNameHV<U,N>* pOp1):UnTTypeNameHV<U,N>(pOp1){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		for(unsigned int i=this->length();i<l;++i) this->val(i)=this->opVal(i);
		return this->length()=l;
	}
private:
	void operator=(const TTypeNamePOW2<U,N,V>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> pow(const TTypeName<U,N>& val1, const TTypeName<U,N>& val2)
{
	TTypeName<U,N> tmp(exp(val2*log(val1)));
	TTypeNameHV<U,N>* pHV=val1.length()>0 && val2.length()>0 ?
		new TTypeNamePOW<U,N>(Op<U>::myPow(val1.val(),val2.val()),tmp.getTTypeNameHV()) :
		new TTypeNamePOW<U,N>(tmp.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
/*
template <typename U, int N>
TTypeName<U,N> pow(const typename Op<U>::Underlying& a, const TTypeName<U,N>& val2)
{
	TTypeName<U,N> tmp(exp(val2*Op<U>::myLog(a)));
	TTypeNameHV<U,N>* pHV=val2.length()>0 ?
		new TTypeNamePOW1<U,N,typename Op<U>::Underlying>(Op<U>::myPow(a,val2.val()), tmp.getTTypeNameHV()) :
		new TTypeNamePOW1<U,N,typename Op<U>::Underlying>(tmp.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N>
TTypeName<U,N> pow(const TTypeName<U,N>& val1, const typename Op<U>::Underlying& b)
{
	TTypeName<U,N> tmp(exp(b*log(val1)));
	TTypeNameHV<U,N>* pHV=val1.length()>0 ?
		new TTypeNamePOW2<U,N,typename Op<U>::Underlying>(Op<U>::myPow(val1.val(),b), tmp.getTTypeNameHV()) :
		new TTypeNamePOW2<U,N,typename Op<U>::Underlying>(tmp.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N>
TTypeName<U,N> pow(const typename Op<U>::Base& a, const TTypeName<U,N>& val2)
{
	TTypeName<U,N> tmp(exp(val2*Op<typename Op<U>::Base>::myLog(a)));
	TTypeNameHV<U,N>* pHV=val2.length()>0 ?
		new TTypeNamePOW1<U,N,typename Op<U>::Base>(Op<U>::myPow(a,val2.val()), tmp.getTTypeNameHV()) :
		new TTypeNamePOW1<U,N,typename Op<U>::Base>(tmp.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N>
TTypeName<U,N> pow(const TTypeName<U,N>& val1, const typename Op<U>::Base& b)
{
	TTypeName<U,N> tmp(exp(b*log(val1)));
	TTypeNameHV<U,N>* pHV=val1.length()>0 ?
		new TTypeNamePOW2<U,N,typename Op<U>::Base>(Op<U>::myPow(val1.val(),b), tmp.getTTypeNameHV()) :
		new TTypeNamePOW2<U,N,typename Op<U>::Base>(tmp.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
*/
template <typename U, int N, typename V>
TTypeName<U,N> pow(const V& a, const TTypeName<U,N>& val2)
{
	TTypeName<U,N> tmp(exp(val2*Op<V>::myLog(a)));
	TTypeNameHV<U,N>* pHV=val2.length()>0 ?
		new TTypeNamePOW1<U,N,V>(Op<U>::myPow(a,val2.val()), tmp.getTTypeNameHV()) :
		new TTypeNamePOW1<U,N,V>(tmp.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}
template <typename U, int N, typename V>
TTypeName<U,N> pow(const TTypeName<U,N>& val1, const V& b)
{
	TTypeName<U,N> tmp(exp(b*log(val1)));
	TTypeNameHV<U,N>* pHV=val1.length()>0 ?
		new TTypeNamePOW2<U,N,V>(Op<U>::myPow(val1.val(),b), tmp.getTTypeNameHV()) :
		new TTypeNamePOW2<U,N,V>(tmp.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}

// SQR

template <typename U, int N>
struct TTypeNameSQR : public UnTTypeNameHV<U,N>
{
	TTypeNameSQR(const U& val, TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(val,pOp){}
	TTypeNameSQR(TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(pOp){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		if (0==this->length()) { this->val(0)=Op<U>::mySqr(this->opVal(0)); this->length()=1; }
		for(unsigned int i=this->length();i<l;++i)
		{
			this->val(i)=Op<U>::myZero();
			unsigned int m=(i+1)/2;
			for(unsigned int j=0;j<m;++j) Op<U>::myCadd(this->val(i), this->opVal(i-j)*this->opVal(j));
			Op<U>::myCmul(this->val(i), Op<U>::myTwo());
			if (0==i%2) Op<U>::myCadd(this->val(i), Op<U>::mySqr(this->opVal(m)));
		}
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameSQR<U,N>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> sqr(const TTypeName<U,N>& val)
{ 
	TTypeNameHV<U,N>* pHV=val.length()>0 ?
		new TTypeNameSQR<U,N>(Op<U>::mySqr(val.val()), val.getTTypeNameHV()) :
		new TTypeNameSQR<U,N>(val.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}

// SQRT

template <typename U, int N>
struct TTypeNameSQRT : public UnTTypeNameHV<U,N>
{
	TTypeNameSQRT(const U& val, TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(val,pOp){}
	TTypeNameSQRT(TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(pOp){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		if (0==this->length()) { this->val(0)=Op<U>::mySqrt(this->opVal(0)); this->length()=1; }
		for(unsigned int i=this->length();i<l;++i)
		{
			this->val(i)=Op<U>::myZero();
			unsigned int m=(i+1)/2;
			for(unsigned int j=1;j<m;++j) Op<U>::myCadd(this->val(i), this->val(i-j)*this->val(j));
			Op<U>::myCmul(this->val(i), Op<U>::myTwo());
			if (0==i%2) Op<U>::myCadd(this->val(i), Op<U>::mySqr(this->val(m)));
			this->val(i)=(this->opVal(i)-this->val(i))/(Op<U>::myTwo()*this->val(0));
		}
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameSQRT<U,N>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> sqrt(const TTypeName<U,N>& val)
{
	TTypeNameHV<U,N>* pHV=val.length()>0 ?
		new TTypeNameSQRT<U,N>(Op<U>::mySqrt(val.val()), val.getTTypeNameHV()):
		new TTypeNameSQRT<U,N>(val.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}

// EXP

template <typename U, int N>
struct TTypeNameEXP : public UnTTypeNameHV<U,N>
{
	TTypeNameEXP(const U& val, TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(val,pOp){}
	TTypeNameEXP(TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(pOp){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		if (0==this->length()) { this->val(0)=Op<U>::myExp(this->opVal(0)); this->length()=1; }
		for(unsigned int i=this->length();i<l;++i)
		{
			this->val(i)=Op<U>::myZero();
			for(unsigned int j=0;j<i;++j)
				Op<U>::myCadd(this->val(i), (Op<U>::myOne()-Op<U>::myInteger(j) / 
					Op<U>::myInteger(i))*this->opVal(i-j)*this->val(j));
		}
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameEXP<U,N>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> exp(const TTypeName<U,N>& val)
{
	TTypeNameHV<U,N>* pHV=val.length()>0 ?
		new TTypeNameEXP<U,N>(Op<U>::myExp(val.val()), val.getTTypeNameHV()):
		new TTypeNameEXP<U,N>(val.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}

// LOG

template <typename U, int N>
struct TTypeNameLOG : public UnTTypeNameHV<U,N>
{
	TTypeNameLOG(const U& val, TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(val,pOp){}
	TTypeNameLOG(TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(pOp){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		if (0==this->length()) { this->val(0)=Op<U>::myLog(this->opVal(0)); this->length()=1; }
		for(unsigned int i=this->length();i<l;++i)
		{
			this->val(i)=this->opVal(i);
			for(unsigned int j=1;j<i;++j)
				Op<U>::myCsub(this->val(i), (Op<U>::myOne()-Op<U>::myInteger(j) /
					Op<U>::myInteger(i))*this->opVal(j)*this->val(i-j));
			Op<U>::myCdiv(this->val(i), this->opVal(0));
		}
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameLOG<U,N>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> log(const TTypeName<U,N>& val)
{
	TTypeNameHV<U,N>* pHV=val.length()>0 ?
		new TTypeNameLOG<U,N>(Op<U>::myLog(val.val()), val.getTTypeNameHV()):
		new TTypeNameLOG<U,N>(val.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}

// SIN

template <typename U, int N>
struct TTypeNameSIN : public UnTTypeNameHV<U,N>
{
	U m_COS[N];
	TTypeNameSIN(const U& val, TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(val,pOp){m_COS[0]=Op<U>::myCos(this->opVal(0));}
	TTypeNameSIN(TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(pOp){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		if (0==this->length()) 
		{ 
			this->val(0)=Op<U>::mySin(this->opVal(0));
			m_COS[0]=Op<U>::myCos(this->opVal(0));
			this->length()=1; 
		}
		for(unsigned int i=this->length();i<l;++i)
		{
			this->val(i)=Op<U>::myZero();
			for(unsigned int j=0;j<i;++j)
				Op<U>::myCadd(this->val(i), Op<U>::myInteger(j+1)*m_COS[i-1-j]*this->opVal(j+1));
			Op<U>::myCdiv(this->val(i), Op<U>::myInteger(i));
			m_COS[i]=Op<U>::myZero();
			for(unsigned int j=0;j<i;++j)
				Op<U>::myCsub(m_COS[i], Op<U>::myInteger(j+1)*this->val(i-1-j)*this->opVal(j+1));
			Op<U>::myCdiv(m_COS[i], Op<U>::myInteger(i));
		}
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameSIN<U,N>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> sin(const TTypeName<U,N>& val)
{
	TTypeNameHV<U,N>* pHV=val.length()>0 ?
		new TTypeNameSIN<U,N>(Op<U>::mySin(val.val()), val.getTTypeNameHV()):
		new TTypeNameSIN<U,N>(val.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}

// COS

template <typename U, int N>
struct TTypeNameCOS : public UnTTypeNameHV<U,N>
{
	U m_SIN[N];
	TTypeNameCOS(const U& val, TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(val,pOp){m_SIN[0]=Op<U>::mySin(this->opVal(0));}
	TTypeNameCOS(TTypeNameHV<U,N>* pOp):UnTTypeNameHV<U,N>(pOp){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=this->opEval(k);
		if (0==this->length()) 
		{ 
			this->val(0)=Op<U>::myCos(this->opVal(0));
			m_SIN[0]=Op<U>::mySin(this->opVal(0));
			this->length()=1; 
		}
		for(unsigned int i=this->length();i<l;++i)
		{
			this->val(i)=Op<U>::myZero();
			for(unsigned int j=0;j<i;++j)
				Op<U>::myCsub(this->val(i), Op<U>::myInteger(j+1)*m_SIN[i-1-j]*this->opVal(j+1));
			Op<U>::myCdiv(this->val(i), Op<U>::myInteger(i));
			m_SIN[i]=Op<U>::myZero();
			for(unsigned int j=0;j<i;++j)
				Op<U>::myCadd(m_SIN[i], Op<U>::myInteger(j+1)*this->val(i-1-j)*this->opVal(j+1));
			Op<U>::myCdiv(m_SIN[i], Op<U>::myInteger(i));
		}
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameCOS<U,N>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> cos(const TTypeName<U,N>& val)
{
	TTypeNameHV<U,N>* pHV=val.length()>0 ?
		new TTypeNameCOS<U,N>(Op<U>::myCos(val.val()), val.getTTypeNameHV()):
		new TTypeNameCOS<U,N>(val.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}

// TAN

template <typename U, int N>
struct TTypeNameTAN : public BinTTypeNameHV<U,N>
{
	TTypeNameTAN(const U& val, TTypeNameHV<U,N>* pOp, TTypeNameHV<U,N>* pSqrCos):BinTTypeNameHV<U,N>(val,pOp,pSqrCos){}
	TTypeNameTAN(TTypeNameHV<U,N>* pOp, TTypeNameHV<U,N>* pSqrCos):BinTTypeNameHV<U,N>(pOp,pSqrCos){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=std::min(this->op1Eval(k),this->op2Eval(k));
		if (0==this->length()) { this->val(0)=Op<U>::myTan(this->op1Val(0)); this->length()=1; }
		for(unsigned int i=this->length();i<l;++i)
		{
			this->val(i)=Op<U>::myZero();
			for(unsigned int j=1;j<i;++j)
				Op<U>::myCadd(this->val(i), Op<U>::myInteger(j)*this->val(j)*this->op2Val(i-j));
			this->val(i)=(this->op1Val(i)-this->val(i)/Op<U>::myInteger(i))/this->op2Val(0);
		}
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameTAN<U,N>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> tan(const TTypeName<U,N>& val)
{ 
	TTypeName<U,N> tmp(sqr(cos(val)));
	TTypeNameHV<U,N>* pHV=val.length()>0 ?
		new TTypeNameTAN<U,N>(Op<U>::myTan(val.val()), val.getTTypeNameHV(), tmp.getTTypeNameHV()):
		new TTypeNameTAN<U,N>(val.getTTypeNameHV(), tmp.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}

// ASIN

template <typename U, int N>
struct TTypeNameASIN : public BinTTypeNameHV<U,N>
{
	TTypeNameASIN(const U& val, TTypeNameHV<U,N>* pOp, TTypeNameHV<U,N>* pSqrt):BinTTypeNameHV<U,N>(val,pOp,pSqrt){}
	TTypeNameASIN(TTypeNameHV<U,N>* pOp, TTypeNameHV<U,N>* pSqrt):BinTTypeNameHV<U,N>(pOp,pSqrt){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=std::min(this->op1Eval(k),this->op2Eval(k));
		if (0==this->length()) { this->val(0)=Op<U>::myAsin(this->op1Val(0)); this->length()=1; }
		for(unsigned int i=this->length();i<l;++i)
		{
			this->val(i)=Op<U>::myZero();
			for(unsigned int j=1;j<i;++j)
				Op<U>::myCadd(this->val(i), Op<U>::myInteger(j)*this->val(j)*this->op2Val(i-j));
			this->val(i)=(this->op1Val(i)-this->val(i)/Op<U>::myInteger(i))/this->op2Val(0);
		}
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameASIN<U,N>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> asin(const TTypeName<U,N>& val)
{
	TTypeName<U,N> tmp(sqrt(Op<U>::myOne()-sqr(val)));
	TTypeNameHV<U,N>* pHV=val.length()>0 ?
		new TTypeNameASIN<U,N>(Op<U>::myAsin(val.val()), val.getTTypeNameHV(), tmp.getTTypeNameHV()):
		new TTypeNameASIN<U,N>(val.getTTypeNameHV(), tmp.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}

// ACOS

template <typename U, int N>
struct TTypeNameACOS : public BinTTypeNameHV<U,N>
{
	TTypeNameACOS(const U& val, TTypeNameHV<U,N>* pOp, TTypeNameHV<U,N>* pSqrt):BinTTypeNameHV<U,N>(val,pOp,pSqrt){}
	TTypeNameACOS(TTypeNameHV<U,N>* pOp, TTypeNameHV<U,N>* pSqrt):BinTTypeNameHV<U,N>(pOp,pSqrt){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=std::min(this->op1Eval(k),this->op2Eval(k));
		if (0==this->length()) { this->val(0)=Op<U>::myAcos(this->op1Val(0)); this->length()=1; }
		for(unsigned int i=this->length();i<l;++i)
		{
			this->val(i)=Op<U>::myZero();
			for(unsigned int j=1;j<i;++j)
				Op<U>::myCadd(this->val(i), Op<U>::myInteger(j)*this->val(j)*this->op2Val(i-j));
			this->val(i)=Op<U>::myNeg((this->op1Val(i)+this->val(i)/Op<U>::myInteger(i))/this->op2Val(0));
		}
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameACOS<U,N>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> acos(const TTypeName<U,N>& val)
{
	TTypeName<U,N> tmp(sqrt(Op<U>::myOne()-sqr(val)));
	TTypeNameHV<U,N>* pHV=val.length()>0 ?
		new TTypeNameACOS<U,N>(Op<U>::myAcos(val.val()), val.getTTypeNameHV(), tmp.getTTypeNameHV()):
		new TTypeNameACOS<U,N>(val.getTTypeNameHV(), tmp.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}

// ATAN

template <typename U, int N>
struct TTypeNameATAN : public BinTTypeNameHV<U,N>
{
	TTypeNameATAN(const U& val, TTypeNameHV<U,N>* pOp, TTypeNameHV<U,N>* p1pSqr):BinTTypeNameHV<U,N>(val,pOp,p1pSqr){}
	TTypeNameATAN(TTypeNameHV<U,N>* pOp, TTypeNameHV<U,N>* p1pSqr):BinTTypeNameHV<U,N>(pOp,p1pSqr){}
	unsigned int eval(const unsigned int k)
	{
		unsigned int l=std::min(this->op1Eval(k),this->op2Eval(k));
		if (0==this->length()) { this->val(0)=Op<U>::myAtan(this->op1Val(0)); this->length()=1; }
		for(unsigned int i=this->length();i<l;++i)
		{
			this->val(i)=Op<U>::myZero();
			for(unsigned int j=1;j<i;++j)
				Op<U>::myCadd(this->val(i), Op<U>::myInteger(j)*this->val(j)*this->op2Val(i-j));
			this->val(i)=(this->op1Val(i)-this->val(i)/Op<U>::myInteger(i))/this->op2Val(0);
		}
		return this->length()=l;
	}
private:
	void operator=(const TTypeNameATAN<U,N>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> atan(const TTypeName<U,N>& val)
{ 
	TTypeName<U,N> tmp(Op<U>::myOne()+sqr(val));
	TTypeNameHV<U,N>* pHV=val.length()>0 ?
		new TTypeNameATAN<U,N>(Op<U>::myAtan(val.val()), val.getTTypeNameHV(), tmp.getTTypeNameHV()):
		new TTypeNameATAN<U,N>(val.getTTypeNameHV(), tmp.getTTypeNameHV());
	return TTypeName<U,N>(pHV);
}

// Ned's diff operator

template <typename U, int N>
struct DIFF : public UnTTypeNameHV<U,N>
{
	int m_b;
	DIFF(const U& val, TTypeNameHV<U,N>* pOp, const int b):UnTTypeNameHV<U,N>(val,pOp),m_b(b){}
	DIFF(TTypeNameHV<U,N>* pOp, const int b):UnTTypeNameHV<U,N>(pOp),m_b(b){}
	unsigned int eval(const unsigned int k)
	{
// IN ORDER TO COMPUTE i'th ORDER COEFFICIENTS OF diff(m_o1,b)
// WE NEED (i+b)'th ORDER COEFFICIENTS OF TaylorOp.
		unsigned int l=this->opEval(k+m_b);
// NOW WE SHOULD HAVE op1 EXPANDED TO DEGREE (i+b)'th ORDER,
// WE CAN PROCEED TO COMPUTE UP TO i'TH ORDER COEFFICIENTS
// OF diff(op1,b).
		if (this->length()+m_b<l)
		{
			for(unsigned int i=this->length();i<l-m_b;++i)
			{
				unsigned int fact=1;
				for(unsigned int j=i+m_b;j>i;--j){ fact*=j; }
				this->val(i)=this->opVal(i+m_b)*fact;
			}
			this->length()=l-m_b;
		}
		return this->length();
	}
private:
	void operator=(const DIFF<U,N>&){} // not allowed
};
template <typename U, int N>
TTypeName<U,N> diff(const TTypeName<U,N>& val, const int b)
{
// IF THE ARGUMENT TAYLOR EXPANSION TaylorOp HAS BEEN EVALUATED TO
// DEGREE i THEN WE CAN EVALUATE THE ZERO ORDER VALUE OF
// diff(TaylorOp,i).
// THIS FUNCTION EVALUATES THE 0.ORDER COEFFICIENT
	TTypeNameHV<U,N>* pHV=0;
	if (val.length()>b)
	{
		unsigned int fact=1;
		for(unsigned int j=b;j>1;--j){ fact*=j; }
		pHV=new DIFF<U,N>(val[b]*fact, val.getTTypeNameHV(), b);
	}
	else
	{
		pHV=new DIFF<U,N>(val.getTTypeNameHV(), b);
	}
	return TTypeName<U,N>(pHV);
}


template <typename U, int N> struct Op< TTypeName<U,N> >
{
	typedef TTypeName<U,N> V;
	typedef TTypeName<U,N> Underlying;
	typedef typename Op<U>::Base Base;
	static Base myInteger(const int i) { return Base(i); }
	static Base myZero() { return myInteger(0); }
	static Base myOne() { return myInteger(1);}
	static Base myTwo() { return myInteger(2); }
	static Base myPI() { return Op<Base>::myPI(); }
	static V myPos(const V& x) { return +x; }
	static V myNeg(const V& x) { return -x; }
	template <typename Y> static V& myCadd(V& x, const Y& y) { return x+=y; }
	template <typename Y> static V& myCsub(V& x, const Y& y) { return x-=y; }
	template <typename Y> static V& myCmul(V& x, const Y& y) { return x*=y; }
	template <typename Y> static V& myCdiv(V& x, const Y& y) { return x/=y; }
	static V myInv(const V& x) { return myOne()/x; }
	static V mySqr(const V& x) { return fadbad::sqr(x); }
	template <typename X, typename Y>
	static V myPow(const X& x, const Y& y) { return fadbad::pow(x,y); }
	static V mySqrt(const V& x) { return fadbad::sqrt(x); }
	static V myLog(const V& x) { return fadbad::log(x); }
	static V myExp(const V& x) { return fadbad::exp(x); }
	static V mySin(const V& x) { return fadbad::sin(x); }
	static V myCos(const V& x) { return fadbad::cos(x); }
	static V myTan(const V& x) { return fadbad::tan(x); }
	static V myAsin(const V& x) { return fadbad::asin(x); }
	static V myAcos(const V& x) { return fadbad::acos(x); }
	static V myAtan(const V& x) { return fadbad::atan(x); }
	static bool myEq(const V& x, const V& y) { return x==y; }
	static bool myNe(const V& x, const V& y) { return x!=y; }
	static bool myLt(const V& x, const V& y) { return x<y; }
	static bool myLe(const V& x, const V& y) { return x<=y; }
	static bool myGt(const V& x, const V& y) { return x>y; }
	static bool myGe(const V& x, const V& y) { return x>=y; }
};

} // namespace fadbad

#endif
