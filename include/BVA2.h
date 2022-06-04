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
 


#ifndef _BVA2_H
#define _BVA2_H

#include <string>

#include "assignment.h"
#include "stringmap.h"
#include "clause.h"
#include "sort.h"

#include "heap.h"

#include "interupt.h"


#include <cmath>
#include <ctime>
#include <vector>
#include <deque>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;


class BVA2Solver{
	
	const StringMap& cmd;
	
	void set_parameter( const StringMap& commandline );
	
	
	uint32_t debug;	
	uint32_t minSize; 
	uint32_t varCnt;
	uint32_t addedVariables; 
  int64_t limit; 
	bool print; 
	uint32_t push; 
	vector < vector< Clause* > > occ; 
	
	bool simplify; 
	bool transform; 
	bool treatComplement; 
	
	uint32_t capacity; 
	uint32_t* occCount; 
	
	
	vector< vector< Lit > > binary;	

	vector< Clause* > mark;	
	vector< Lit > countMark;	
	vector< uint32_t > countCount; 
	vector< uint64_t > countSize; 
	
	vector< uint32_t > replaced; 
	
	string outfileName;	
	fstream outfile;
	
	Heap < uint32_t, greater < uint32_t > > work;	
	
	
	vector< Clause* > toChangeClauses; 
	vector< vector< Clause* > > matchingClauses; 
	vector< Lit > matchingLiterals; 
	
	public:
	
	BVA2Solver( const StringMap& commandline );
	
	Assignment* solve( vector<Clause*>& formula, uint32_t varCnt );
	
	protected:
	
	
	bool init( vector<Clause*>& formula );
	
	Var nextVariable();
	
	void removeClause( Clause* clause );
	
	Clause* addClause( Clause* clause );
	
	void printFormula(vector<Clause*>& formula);
	
	void printClause( const Clause* clause );
	
	
	bool propagate(vector<Clause*>& formula);
	
	
	void removeDuplicates(vector< Clause* >& clauses);
	
	
	int32_t reduction( int32_t left, int32_t right );
	
	
	void handleComplement( const Lit right );
	
	
	uint32_t addMatches( const vector<Clause*>& rightList, vector<Clause*>& leftList, const Lit right, const Lit left, uint32_t index = 1 );
};

#endif
