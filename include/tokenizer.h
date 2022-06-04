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
 


#ifndef _TOKENIZER_H
#define _TOKENIZER_H


#include <inttypes.h>
#include <vector>
#include <set>
#include <string>
#include <cstring>

#include <iostream>

#define SEPSIZE 4

using namespace std;


class Tokenizer{

	
	struct sstr{
		char c[SEPSIZE];
		sstr() { }
	};

	
	bool init; 
	const string& data;	
	uint32_t b;	
	uint32_t e; 
	
	vector<sstr> separators;	
	vector<sstr> words;	

public:
	Tokenizer(const string& str) : init(false), data(str), b(0), e(0){}
	
	
	bool addWord( const string& c ){ return addWord( c.c_str() ); }
	bool addWord( const char* c ){
		sstr s;
		if( strlen(c) >= SEPSIZE ) return false;	
		strcpy(s.c,c);
		words.push_back(s);
		return true;
	}

	
	bool addSeparator( const string& c ){ return addSeparator( c.c_str() ); }
	bool addSeparator( const char* c ){
		sstr s;
		if( strlen(c) >= SEPSIZE ) return false;	
		strcpy(s.c,c);
		separators.push_back(s);
		return true;
	}

	bool hasNext()
	{
		if(!init){ findNext(); init=true; }
		return ( b < data.size() );
	}
	
	string nextToken(){
		uint32_t tb = b;
		uint32_t te = e;
		b = e + 1;
		findNext();

		return data.substr( tb, te-tb+1 );
	}

private:
	void findNext(){
		
		for( ; b < data.size(); ++b ){
			uint32_t l = inSeparators(b);
			if( l == 0 ){	
				l = inWords(b);	
				if( l > 0 ){
					e = b + l - 1;
					return;
				}
				e = b;
				break;
			}
		}
		
		e = b + 1;
		for( ; e < data.size(); ++e ){
			if( inWords(e) > 0 || inSeparators(e) > 0 ) break;
		}
		e--;
		
	}

	
	uint32_t inWords(uint32_t pos){
		for( uint32_t i =0; i < words.size(); ++i ){
			const uint32_t l = strlen( words[i].c );
			if( pos + l > data.size() ) continue;
			if( memcmp( words[i].c, data.c_str() + pos, l ) == 0 ) return l;
		}
		return 0;
	}
	
	
	uint32_t inSeparators(uint32_t pos){
		for( uint32_t i =0; i < separators.size(); ++i ){
			const uint32_t l = strlen( separators[i].c );
			if( pos + l > data.size() ) continue;
			if( memcmp( separators[i].c, data.c_str() + pos, l ) == 0 ) return l;
		}
		return 0;
	}
};


#endif
