/*
    BVA
    Copyright (C) 2012 Norbert Manthey

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/ 
 



#ifndef _RISS_Clause_H
#define _RISS_Clause_H


#define _CL_ELEMENTS ((32 - sizeof(uint32_t) - sizeof(Lit*) - sizeof(uint32_t)) / sizeof(Lit))


#define TRACK_CHECK(x) 

#include <assert.h>
#include <cstdlib>
#include <cstring>
#include <vector>

#include <iostream>

#include <inttypes.h>

#include "types.h"
#include "literal_system.h"


#define CL_REF uint32_t

#define CL_OBJSIZE sizeof(Clause)


class Clause
{

private:
	uint32_t _sizedata;	
	Lit	own_lits[_CL_ELEMENTS] ;	
	Lit	*ext_lits;	
	union {	
		float activity;
		uint32_t hash;
	} data;	
	void init();

	
	void reset(std::vector<Lit>& lits);	
	void reset(const Lit *lits, uint32_t count);

	
	void set_size(uint32_t s );	
	void	setStorage(uint32_t s);
	
public:
	
	Clause( bool learnt=true);	
	Clause(std::vector<Lit>& lits, bool learnt=true);	
	Clause(const Lit *lits, uint32_t count, bool learnt=true);	
	Clause& operator=(const Clause& other);	
	~Clause();

	
	bool operator >=(Clause& other) const ;
	
	bool operator <=(Clause& other) const;
	
	bool operator < (const Clause& other) const;

	public:
	
	bool equals(const Clause& other) const;
	
	
	uint32_t	size() const ;
	
	
	Lit	get_literal(uint32_t pos) const;
	
	
	void set_literal(uint32_t pos, Lit lit1);
	
	
	Clause	copy() const ;
	
	
	Clause	copy_except(const Var v);
	
	
	void	add(Lit lit1);	
	void	remove(Lit lit1);	
	void  remove_index(uint32_t ind);
	
	
	void	swap_literals(uint32_t pos1, uint32_t pos2);	
	bool contains_literal( const Lit literal);

	
	float get_activity() const;	
	void set_activity( const float act);	
	void inc_activity( const float act);

	
	
	Clause* resolve(const Clause& other) const;	
	Clause* resolve(const Clause& other, Var variable) const;

	
	bool check_resolve( const Clause& other, Var variable) const;	
	bool subsumes_or_simplifies_clause(const Clause& other, Lit *lit1) const;

	
	void sort();

	
	bool getFlag1() const ;
	void setFlag1(bool flag);

	bool getFlag2() const ;
	void setFlag2(bool flag);

	bool isXor() const ;
	void setXor(bool flag);

	bool is1ooN() const;
	void set1ooN(bool flag);

	bool isLearnt() const ;
	void setLearnt( bool l );

	void ignore(){
		setFlag1(true);
	}
	
	void unignore(){
		setFlag1(false);
	}

	void markToDelete(){
		setFlag2( true );
	}

	bool isIgnored() const {
		return getFlag1() || getFlag2();
	}
	
	bool isToDelete() const {
		return getFlag2();
	}
	
	
	void printInfo();
} ; 

	
inline void Clause::printInfo(){
	std::cerr << "cl[" << std::hex << this << std::dec << "]: clauseSize()(" << size() << "), act(" << data.activity
			<< "), ext(" << size() - _CL_ELEMENTS << ") at " << std::hex << ext_lits << std::dec << ", locals:";
	for( uint32_t i = 0 ; i < _CL_ELEMENTS; ++i ){
		std::cerr << " " <<  own_lits[i] .nr();
	}
	std::cerr << " and externals: ";
	for( uint32_t i = _CL_ELEMENTS; i < size() ; ++i ){
		std::cerr << " " <<  ext_lits[i-_CL_ELEMENTS] .nr();
	}
	std::cerr << std::endl;
}

inline Clause::Clause(bool learnt)
{
	init();
	setStorage(0);
	setLearnt(learnt);
}

inline Clause::Clause(std::vector<Lit>& lits, bool learnt)
{
	init();
	reset(lits);
	setLearnt(learnt);
}

inline Clause::Clause(const Lit *lits, uint32_t count, bool learnt)
{
	init();
	reset(lits, count);
	setLearnt(learnt);
}

inline Clause& Clause::operator=(const Clause& other)
{
	
	init();
	
	_sizedata = other._sizedata;
	
	set_size(0);
	
	setStorage( other.size() );
	assert( size() == other.size());
	if( size()>_CL_ELEMENTS){
		memcpy (own_lits, other.own_lits, _CL_ELEMENTS*sizeof(Lit));
		memcpy (ext_lits, other.ext_lits, ( size()- _CL_ELEMENTS)*sizeof(Lit));
	} else {
		memcpy (own_lits, other.own_lits, _CL_ELEMENTS*sizeof(Lit));
	}
	data.activity = other.data.activity;
	TRACK_CHECK("copied to other clause");
	return *this;
}

inline bool Clause::operator < (const Clause& other) const {
	const uint32_t clauseSize = size();
	if( clauseSize > other.size() ) return false;
	if( clauseSize < other.size() ) return true;
	for( uint32_t i = 0 ; i < clauseSize; i++ ){
		if( get_literal(i) > other.get_literal(i) ) return false;
		if( get_literal(i) < other.get_literal(i) ) return true;
	}
	return false;
}

inline bool Clause::operator >=(Clause& other) const {
	const uint32_t clauseSize = size();
	if( clauseSize > other.size() ) return true;
	if( clauseSize < other.size() ) return false;
	for( uint32_t i = 0 ; i < clauseSize; i++ ){
		if( get_literal(i) > other.get_literal(i) ) return true;
		if( get_literal(i) < other.get_literal(i) ) return false;
	}
	return true;
}
	
inline bool Clause::operator <=(Clause& other) const {
	const uint32_t clauseSize = size();
	if( clauseSize > other.size() ) return false;
	if( clauseSize < other.size() ) return true;
	for( uint32_t i = 0 ; i < clauseSize; i++ ){
		if( get_literal(i) > other.get_literal(i) ) return false;
		if( get_literal(i) < other.get_literal(i) ) return true;
	}
	return true;
}

inline Clause::~Clause()
{
	
	
	TRACK_CHECK("destroyed");
	if(ext_lits != 0) free( ext_lits);
	ext_lits = 0;
	set_size( 0 );
	_sizedata = 0;
	set_activity( 0 );
}

inline void Clause::reset(std::vector<Lit>& lits)
{
	setStorage( lits.size() );
	

	for (uint32_t i=0; i<lits.size(); i++)
		set_literal(i, lits[i]);
	TRACK_CHECK("reset");
}

inline void Clause::reset(const Lit *lits, uint32_t count)
{
	setStorage(count);

	for (uint32_t i=0; i<count; i++)
		set_literal(i, lits[i]);
	TRACK_CHECK("reset");
}

inline bool Clause::equals(const Clause& other) const {
	const uint32_t myS = size();
	const uint32_t oS = other.size();
	for( uint32_t i = 0 ; i < myS ; ++i ){
		uint32_t j = 0;
		for( ; j < oS; ++j ){
			if( get_literal(i) == other.get_literal(j) ) break;
		}
		if( j != oS ) return false;
	}
	return true;
}

inline uint32_t Clause::size() const
{
	return _sizedata & 0x7FFFFFF;	
}

inline void Clause::set_size(uint32_t s ){
	_sizedata = (_sizedata & ~0x7FFFFFF) | (s & 0x7FFFFFF);	

	TRACK_CHECK("set clauseSize to " << s);
}

inline Lit Clause::get_literal(uint32_t pos) const
{
	
	Lit l = (pos < _CL_ELEMENTS) ? own_lits[pos] : ext_lits[pos-_CL_ELEMENTS];
	return l;
}

inline void Clause::set_literal(uint32_t pos, Lit lit1)
{
	TRACK_CHECK("set literal at " << pos << " to " << lit1.nr() );
	(pos <_CL_ELEMENTS) ? own_lits[pos] = lit1 : ext_lits[pos-_CL_ELEMENTS] = lit1;

}

inline void Clause::swap_literals(uint32_t pos1, uint32_t pos2)
{
	assert (pos1 < size());
	assert (pos2 < size());
	
	Lit tmp = get_literal(pos1);
	set_literal(pos1, get_literal(pos2));
	set_literal(pos2, tmp);
	TRACK_CHECK("swap literals " << pos1 << " and " << pos2);
}

inline void Clause::add(Lit lit1)
{
	setStorage(size()+1);
	set_literal(size()-1, lit1);

}

inline void	Clause::remove(Lit lit1)
{
	TRACK_CHECK("remove literal " << lit1.nr() );
	uint32_t i, j;

	for (i = 0, j = 0; i < size(); i ++) {
		if (get_literal(i) != lit1)
			set_literal(j++, get_literal(i));
	}
	setStorage(j);

}

inline void Clause::remove_index(uint32_t ind)
{
	TRACK_CHECK("remove literal at index " << ind);
	swap_literals( ind, size() - 1 );
	setStorage( size() - 1 );

}

inline Clause Clause::copy_except(const Var v){
	TRACK_CHECK("copy except variable " << v);
	const uint32_t sz = size()-1;
	Lit tmp[sz];
	uint32_t j = 0;
	
	if( sz + 1 > _CL_ELEMENTS )
	{
		for(int i=0; i< (int)_CL_ELEMENTS; ++i){
			if( v != own_lits[i].toVar()) tmp[j++] = own_lits[i];
		}

		
		for (int i=0; i<(int)(size()-_CL_ELEMENTS); ++i)
			if( v != ext_lits[i].toVar()) tmp[j++] = ext_lits[i];
	} else {
		for(int i=0; i< (int)size(); ++i)
			if( v != own_lits[i].toVar()) tmp[j++] = own_lits[i];
	}

	
	Clause cl(tmp, sz, get_activity());
	cl.setLearnt( isLearnt() );

	return cl;
}
inline Clause Clause::copy() const
{
	const uint32_t sz = size();
	Lit tmp[sz];
	uint32_t j = 0;
	
	if( sz > _CL_ELEMENTS )
	{
		for(int i=0; i< (int)_CL_ELEMENTS; ++i){
			tmp[j++] = own_lits[i];
		}
		
		for (int i=0; i<(int)(size()-_CL_ELEMENTS); ++i)
			tmp[j++] = ext_lits[i];
	} else {
		for(int i=0; i< (int)size(); ++i)
			tmp[j++] = own_lits[i];
	}

	
	Clause cl(tmp, sz, get_activity());
	cl.setLearnt( isLearnt() );
	return cl;
}

inline void Clause::setStorage(uint32_t new_lits)
{
	assert( sizeof(Lit)==4 );

	if (size() != new_lits) {
		

		
		if (new_lits > _CL_ELEMENTS) {
			if(size() > _CL_ELEMENTS){
				
				#ifndef JAVALIB
				ext_lits = (Lit *)realloc(ext_lits,
																	sizeof(Lit) * (new_lits - _CL_ELEMENTS));
				#else
					Lit* tmp = (Lit*)malloc( sizeof(Lit) * (new_lits - _CL_ELEMENTS ) );
					if( tmp == 0 ) std::cerr << "c was not able to Mallocate memory for a new clause" << std::endl;
					memcpy( tmp, ext_lits, sizeof(Lit) * (size() - _CL_ELEMENTS) );
					FREE_SIZE( ext_lits, sizeof(Lit) * (size() - _CL_ELEMENTS) );
					ext_lits = tmp;
				#endif
			} else { 
				ext_lits = (Lit *)malloc(sizeof(Lit) * (new_lits - _CL_ELEMENTS) );
				#ifdef JAVALIB
					if( ext_lits == 0 ) std::cerr << "c was not able to Mallocate memory for a new clause" << std::endl;
				#endif
				}
		} else {
			
			if (size() > _CL_ELEMENTS){
				
				free(ext_lits);
				ext_lits = 0;
			}
		}
		set_size( new_lits );

		
	}
	TRACK_CHECK("set storage to " << new_lits);

}

inline void Clause::init()
{
	_sizedata = 0;
	ext_lits = 0;
	data.activity = 0;
	for (unsigned int i=0; i<_CL_ELEMENTS; ++i) own_lits[i]=NO_LIT;
}

inline float Clause::get_activity() const
{
	return data.activity;
}

inline void Clause::set_activity( const float activity)
{
	data.activity = activity;
}

inline void Clause::inc_activity( const float act)
{
	data.activity += act;
}

inline Clause* Clause::resolve(const Clause& other) const
{
	TRACK_CHECK("resolve");
	const uint32_t s = size();
	for( int32_t i = s - 1; i >=0; i--)
	{
		const uint32_t os = other.size();
		for( int32_t j = os - 1; j >=0; j--)
		{
			if( get_literal( i ) ==  other.get_literal( j ) .complement() )
			return resolve( other,  get_literal( i ) .toVar() );
		}
	}
 	return 0;
}

inline Clause* Clause::resolve(const Clause& other, Var variable) const
{
	Var var11, var12;
	Lit lit11, lit12;
	uint32_t j, sz;

	uint32_t bytes = (size() + other.size())  * sizeof(Lit);

	Lit new_lits[ bytes ];
	sz = 0;

	const Clause& cls1 = (size() < other.size()) ? (*this) : other;
	const Clause& cls2 = (size() < other.size()) ? other : (*this);

	for (uint32_t i = 0; i < cls1.  size(); i++)
	{
		lit11 = cls1.get_literal(i);
		var11 = lit11.toVar();

		if (variable == var11)
			continue;

		for (j = 0; j < cls2.  size(); j++)
		{
			lit12 = cls2.get_literal(j);
			var12 = lit12.toVar();

			if (var12 == var11) {
				if (lit11 == lit12.complement()) {
					return 0;
				}
				else
					break;
			}
		}

		if (j == cls2.size())
			new_lits[sz++] = lit11;
	}

	for (uint32_t j = 0; j < cls2.  size(); j++)
	{
		lit12 = cls2.get_literal(j);
		var12 = lit12.toVar();

		if (var12 != variable)
			new_lits[sz++] = lit12;
	}

	
	return new Clause(new_lits, sz, cls1.isLearnt() || cls2.isLearnt() );
}

inline bool Clause::check_resolve( const Clause& other, Var variable) const
{
	TRACK_CHECK("check resolve");
	Var var11, var12;
	Lit lit11, lit12;

	const Clause& cls1 = (size() < other.size()) ? (*this) : other;
	const Clause& cls2 = (size() < other.size()) ? other : (*this);

	for (uint32_t i = 0; i < cls1.size(); i++)
	{
		lit11 = cls1.get_literal(i);
		var11 = lit11.toVar();

    	if (variable == var11)
    		continue;

		for (uint32_t j = 0; j < cls2.size(); j++)
		{
			lit12 = cls2.get_literal(j);
			var12 = lit12.toVar();

			if (var12 == var11) {
				if (lit11 == lit12.complement())
					return false;
				else
					break;
			}
		}
	}

	
	return true;
}

inline bool Clause::contains_literal( const Lit literal)
{
	TRACK_CHECK("contain literal " << literal.nr());
	for (uint32_t i = 0; i < size(); i++)
	{
		if( get_literal(i) == literal ) return true;
	}
	return false;
}


inline bool Clause::subsumes_or_simplifies_clause(const Clause& other, Lit *lit1) const
{
	TRACK_CHECK("check subsume simplify");
	
	const uint32_t other_clauseSize = other.size();
	const uint32_t clauseSize = size();
	if ( clauseSize > other_clauseSize) return false;

	*lit1 = NO_LIT;

	
	for (uint32_t i=0; i<clauseSize; i++) {
		uint32_t j=0;
		for (; j<other_clauseSize; j++) {
			const Lit otherJ = other.get_literal(j);
			
			if (get_literal(i) == otherJ) break;
			
			if ((*lit1 == NO_LIT) && (get_literal(i) == otherJ.complement()) ) {
				*lit1 = otherJ;
				break;
			}
			
			if ((*lit1 != NO_LIT )&& (get_literal(i) == otherJ.complement()))
			  return false;
		}
		
		if (j==other_clauseSize) return false;
	}

	
	return true;
}


inline void Clause::sort(){
	const uint32_t clauseSize = size()-1;
	for( uint32_t i = 0 ; i < clauseSize; ++i ){
		uint32_t p = i;
		Lit l = get_literal(p);
		for( uint32_t j = i + 1; j<=clauseSize; ++j ){
			const Lit l2 = get_literal(j);
			if( l > l2 ){ p = j; l = l2; }
		}
		swap_literals(i, p);
	}
}

inline bool Clause::getFlag1() const { 
	TRACK_CHECK("get flag 1");
	return _sizedata & 0x80000000;
}

inline void Clause::setFlag1(bool flag){ 
	TRACK_CHECK("set flag 1");
	 _sizedata = (_sizedata & ~0x80000000) | ((uint32_t)flag << 31); 
}


inline bool Clause::getFlag2() const { 
	TRACK_CHECK("get flag 2");
	 return _sizedata & 0x40000000; 
}

inline void Clause::setFlag2(bool flag){ 
	TRACK_CHECK("set flag 2");
	 _sizedata = (_sizedata & ~0x40000000) | ((uint32_t)flag << 30); 
}

inline bool Clause::isXor() const { 
	 return _sizedata & 0x20000000; 
}

inline void Clause::setXor(bool flag){ 
	 _sizedata = (_sizedata & ~0x20000000) | ((uint32_t)flag << 29); 
}

inline bool Clause::is1ooN() const { 
	return _sizedata & 0x10000000; 
}

inline void Clause::set1ooN(bool flag){ 
	 _sizedata = (_sizedata & ~0x10000000) | ((uint32_t)flag << 28); 
}

inline bool Clause::isLearnt() const { 
	TRACK_CHECK("get flag learnt");
	 return _sizedata & 0x8000000; 
}

inline void Clause::setLearnt( bool l ){ 
	TRACK_CHECK("set flag learnt to " << l);
	 _sizedata = (_sizedata & ~0x8000000) | ((uint32_t)l << 27); 
}


#endif 
