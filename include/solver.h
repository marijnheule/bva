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
 


#ifndef _SOLVER_H
#define _SOLVER_H

#include <string>

#include "assignment.h"
#include "stringmap.h"
#include "clause.h"
#include "sort.h"

using namespace std;


class Solver{
	
	string methodToUse;
	const StringMap& cmd;
	
	public:
	
	Solver( const StringMap& commandline );
	
	Assignment* solve( vector<Clause*>& formula, uint32_t varCnt );
	
	protected:
	
	void set_parameter( const StringMap& commandline );
};

#endif
