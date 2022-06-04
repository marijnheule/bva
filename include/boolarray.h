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
 



#ifndef _BOOLARRAY_H
#define _BOOLARRAY_H

#include <inttypes.h>	
#include <string.h>
#include <stdlib.h>
#include <assert.h>

class BoolArray{
  private:
    uint8_t* boolarray;
    
  public:
  
  BoolArray(const uint32_t elements)
  : boolarray(0)
  {
    
    boolarray = (uint8_t*)malloc( elements * sizeof(uint8_t) );
    memset( boolarray, 0, elements * sizeof(uint8_t) );
  }
  
  BoolArray() : boolarray(0) {}

  void init(const uint32_t elements){
    assert( boolarray == 0 );
    
    boolarray = (uint8_t*)malloc( elements * sizeof(uint8_t) );
    memset( boolarray, 0, elements * sizeof(uint8_t) );   
  }

  BoolArray copy(const uint32_t elements) const ;

  void destroy();

  void clear(const uint32_t elements);

  bool get( const uint32_t ind ) const ;

  void set( const uint32_t ind, const bool value );

  void extend(const uint32_t elements, const uint32_t newElements);

};




inline BoolArray BoolArray::copy(const uint32_t elements) const
{
  BoolArray ba;
  
  ba.boolarray = (uint8_t*)malloc( elements * sizeof(uint8_t));
  memcpy( ba.boolarray, boolarray, elements * sizeof(uint8_t) );
  return ba;
}

inline void BoolArray::extend(const uint32_t elements, const uint32_t newElements){
	assert( boolarray != 0 );
	
	boolarray = (uint8_t*)realloc( boolarray, (newElements ) * sizeof(uint8_t) );
	
	
	if( newElements > elements ){
		const uint32_t diff = newElements - elements;
		memset( ((uint8_t*)boolarray)+elements, 0, diff * sizeof(uint8_t) );
	}
}

inline void BoolArray::destroy()
{
	assert( boolarray != 0 );
	if( boolarray != 0 )
	{
		free(boolarray);
		boolarray = 0;
	}
}

inline void BoolArray::clear(const uint32_t elements){
	assert( boolarray != 0 );
	memset( boolarray, 0, elements * sizeof(uint8_t) );
}

inline bool BoolArray::get( const uint32_t ind ) const
{
    assert( boolarray != 0 );
    return ((uint8_t*)boolarray)[ ind ]==1;
}

inline void BoolArray::set( const uint32_t ind, const bool value )
{
	assert( boolarray != 0 );
	((uint8_t*)boolarray)[ ind ] = value == true ? 1 : 0;
}

#endif
