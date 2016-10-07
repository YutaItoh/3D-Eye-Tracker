// Petter Strandmark 2013.
#ifndef SPII_TERM_FACTORY_H
#define SPII_TERM_FACTORY_H

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>

//#include <spii/auto_diff_term.h>
#include <spii/spii.h>
#include <spii/term.h>

namespace spii
{

template<typename T>
struct TermTeacher;

class SPII_API TermFactory
{
public:
	typedef std::function<Term*(std::istream&)> TermCreator;

	TermFactory();
	~TermFactory();
	TermFactory(const TermFactory&)      = delete;
	void operator = (const TermFactory&) = delete;

	template<typename T>
	void teach_term()
	{
		TermTeacher<T>::teach(*this);
	}

	static std::string fix_name(std::string org_name);

	std::shared_ptr<const Term> create(const std::string& term_name, std::istream& in) const;
	void teach_term(const std::string& term_name, const TermCreator& creator);

private:
	class Implementation;
	// unique_pointer would have been nice, but there are issues
	// with sharing these objects across DLL boundaries in VC++.
	Implementation* impl;
};

//
// The TermTeacher class is used because partially specializing
// functions is not possible.
//
template<typename T>
struct TermTeacher
{
	static void teach(TermFactory& factory)
	{
		auto creator = [](std::istream& in) -> Term*
		{
			auto term = new T;
			in >> *term;
			return term;
		};
		factory.teach_term(factory.fix_name(typeid(T).name()), creator);
	}
};

// Specialize for AutoDiffTerm.
template<typename Functor, int... D>
class AutoDiffTerm;

template<typename T, int... D>
struct TermTeacher< AutoDiffTerm<T, D...> >
{
	typedef AutoDiffTerm<T, D...> AutoTerm;
	static void teach(TermFactory& factory)
	{
		auto creator = [](std::istream& in) -> Term*
		{
			auto term = new AutoTerm;
			in >> *term;
			return term;
		};
		factory.teach_term(typeid(AutoTerm).name(), creator);
	}
};

}

#endif
