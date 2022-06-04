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
 


#include "solver.h"
#include "BVA2.h"

Solver::Solver( const StringMap& commandline )
:
methodToUse("BVA2"),
cmd(commandline)
{
  set_parameter( commandline );
}
	
Assignment* Solver::solve( vector<Clause*>& formula, uint32_t varCnt ) {
	cerr << "c parsed instance with " << varCnt << " variables and " << formula.size() << " clauses" << endl;
	
 	if( methodToUse == "BVA2" ){ 
		cerr << "c start BVA2" << endl; 
		BVA2Solver solver(cmd); 
		return solver.solve(formula, varCnt); 
	} else
	
	{
		cerr << "c specified method " << methodToUse << " cannot be found" << endl;
		assert( false );
		return 0;
	}
}

void Solver::set_parameter( const StringMap& commandline ){
	if( commandline.contains( (const char*)"--verb-help" ) || commandline.contains( (const char*)"--help" ) )
		cerr << endl<< "=== Solver Wrapper Information ===" << endl;
	commandline.updateParameter( "method", methodToUse, "name of the method that should be applied");
}
