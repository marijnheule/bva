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
 


#ifndef _ITER_FILE_PARSER_H
#define _ITER_FILE_PARSER_H

#include <fstream>
#include <iostream>
#include <assert.h>
#include <string>
#include <vector>

#include "stringmap.h"
#include "clause.h"
#include "sort.h"

using namespace std;


class FileParser{
protected:
	
	bool parse;	
	bool print_dimacs;	
public:

	
	 FileParser();
	
	
	 FileParser( const StringMap& commandline);
	
	
	 solution_t parse_file(char* filename, uint32_t & var_cnt, std::vector< Clause* >* clauses );

protected:
	
	Clause* addLiterals( std::vector< Lit >& clause_lits ) const;
	
	
	Clause* addLiterals( Lit* clause_lits, uint32_t size ) const;
};

#endif
