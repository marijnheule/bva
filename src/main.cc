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
 

#include "types.h"
#include "clause.h"
#include "solver.h"
#include "microtime.h"
#include "fileparser.h"
#include "stringmap.h"
#include "commandlineparser.h"

#include <stdlib.h>
#include <signal.h>

#include "interupt.h"

using namespace std;

int32_t main(int32_t argc, char* argv[])
{
	
	signal( SIGTERM, signalHandler);
	signal( SIGINT,  signalHandler);
	
	StringMap commandline;
	CommandLineParser cmp;
	commandline = cmp.parse(argc, argv);
	
	uint32_t varCnt = 0;
	vector< Clause* > clauses;

	bool parsedUNSAT = false;	
	FileParser fp( commandline );
	Assignment* assignment = 0;
	
	
	if( commandline.contains("-h") || commandline.contains("--help")  || commandline.contains("--verb-help")  )
	{
		
		Solver solver(commandline);
		solver.solve(clauses, varCnt);
		return 0;
	} else {
		
		uint64_t start = get_microseconds();
		if( UNSAT != fp.parse_file( (char*)commandline.get( (const char*)"file" ).c_str(), varCnt, &clauses ) ){
			Solver solver(commandline);
			assignment = solver.solve(clauses, varCnt);	
		} else {
			parsedUNSAT = true;
		}
		uint64_t end = get_microseconds();
		if( !commandline.contains( (const char*)"-s" ) ) cout << "c solver needed " << end - start << "us (" << (end - start)/1000000 << "s)" << endl;
	}
	
	
	
	if( !commandline.contains( (const char*)"-s" ) )
	{
	  
	  if( assignment == 0 ){
	  	if( parsedUNSAT ) {
				cout << "s UNSATISFIABLE" << endl;
				exit( 20 );  	
	  	} else {
				cout << "s UNKNOWN" << endl;
				if( commandline.contains( (const char*)"outfile" ) ){
					ofstream solutionFile( commandline.get( (const char*)"outfile" ).c_str() );
					solutionFile << "s UNKNOWN" << endl;
				}
		  }
		  
		  for( unsigned i = 0 ; i < clauses.size(); ++i ) delete clauses[i];
		  return 0;
	  } else {
		  cerr << "c checking solution ... ";
			  bool satisfied = false;
		  uint32_t i = 0;
		  for( ; i < clauses.size(); ++i )
		  {
			  uint32_t j = 0;
			  const Clause& clause = *( clauses[i] );
			  for( ; j < clause.size(); ++j )
			  {
				  if( assignment->is_sat( clause.get_literal(j) ) ) break;
			  }
			  
			  if( j == clause.size() ) break;
		  }
			  if( i == clauses.size() )
		  {
			  
			  satisfied = true;
		  }
			  if (satisfied)
			  cerr << " OK" << endl << "Solution is a model" << endl;
		  else
			  cerr << " FAIL" << endl << "Solution is NOT a model" << endl;
		  
		  cout << "s SATISFIABLE" << endl;
		  
		  if( !commandline.contains( (const char*)"-q" ) && !commandline.contains( (const char*)"outfile" ))
		  {
				  cout << "v";
				  for( uint32_t i = 1 ; i<=varCnt; ++i ) cout << " " << Lit(i, assignment->get_polarity( i) ).nr();
				  cout << " 0" << endl; 
		  } else {
			  if( commandline.contains( (const char*)"outfile" ) ){
				  ofstream solutionFile( commandline.get( (const char*)"outfile" ).c_str() );
				  solutionFile << "s SATISFIABLE" << endl;
				  if( !commandline.contains( (const char*)"-q" ) ){
						  solutionFile << "v";
						  for( uint32_t i = 1 ; i<=varCnt; ++i ) solutionFile << " " << Lit(i, assignment->get_polarity( i) ).nr();
						  solutionFile << " 0" << endl; 
				  }
			  }
		  }
		  
		  
		  assignment->destroy(varCnt);
		  delete assignment;
		  for( unsigned i = 0 ; i < clauses.size(); ++i ) delete clauses[i];
		  return 10;
	  }
	}
	
	return 0;
}
