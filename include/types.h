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
 


#ifndef _TYPES_H
#define _TYPES_H


#include <inttypes.h>




typedef uint32_t Var;


typedef enum {
	POS = 1,
	NEG = 2,
	UNDEF = 0
} Polarity;


typedef enum {
 SAT = 10,
 UNSAT = 20,
 UNKNOWN = 0
} solution_t;



class Lit {
   uint32_t literal;
  public:
  Lit(): literal(0) {}
  
  
  explicit Lit( const int32_t number ){
    literal = (number < 0) ? (-2*number + 1) : 2*number;
  }
  
  Lit( const Var number , Polarity pol){
    literal = (pol == NEG) ? (2*number + 1) : 2*number;
  }
  
  uint32_t& operator=(const uint32_t& other){
    literal = other;
    return literal;
  }

  bool operator<(const Lit& other) const {
    return literal < other.literal;
  }

  bool operator>(const Lit& other) const {
    return literal > other.literal;
  }

  bool operator==(const Lit& other) const {
    return literal == other.literal;
  }
  
  bool operator!=(const Lit& other) const {
    return literal != other.literal;
  }

  int32_t& operator=(const Lit& other){
    literal = other.literal;
    return (int32_t&)literal;
  }
  
  Lit complement() const {
    Lit t;
    t.literal = literal ^ 1;
    return t;
  }
  
  Polarity getPolarity()  const {
    return (Polarity)(1 + (literal & 0x1));
  }

  Var toVar()  const {
    return literal / 2;
  }

  uint32_t toIndex()  const {
    return literal;
  }
  
  int32_t nr()  const  {
    return (literal%2==1) ? (((int32_t)literal)/2*-1) : (literal/2);
  }
  
  uint32_t data() const { return literal; }
  
  
  void fromData( uint32_t data ){ literal = data; }
}; 

const Lit NO_LIT = Lit(0);








#ifndef COMPRESS_Clause
	#define CL_PACK __attribute__((packed))
#else
	#define CL_PACK
#endif

#ifdef COMPRESS_WATCH_STRUCTS
	#define WATCH_STRUCT_PACK __attribute__((packed))
#else
	#define WATCH_STRUCT_PACK 
#endif

#endif
