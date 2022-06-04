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
 


#include "fileparser.h"

FileParser::FileParser()
{
	print_dimacs = false;
}

FileParser::FileParser( const StringMap& commandline)
{
	print_dimacs = false;
	parse = !( commandline.contains( (const char*)"--verb-help" ) || commandline.contains( (const char*)"--help" ) );

	if( commandline.contains( (const char*)"--verb-help" ) || commandline.contains( (const char*)"--help" ) )
	cerr << endl<< "=== file parser information ===" << endl;
	commandline.updateParameter("Fp_print" ,print_dimacs, "printes file with according p Line");
}

solution_t FileParser::parse_file(char* filename, uint32_t & var_cnt, std::vector< Clause* >* clauses )
{
	if(!parse) return UNKNOWN;
	
	#ifdef TRACK_Clause
		std::vector< Lit > track_lits;
		int32_t i = 0;
		cerr << "c type clause that has to be checked (ending symbol is a 0)" << endl;
		do {
			cin >> i;
			if( i != 0 )
			 track_lits.push_back( Lit(i) );
		} while ( i != 0 );

		if(   track_lits .size() > 0 ) selectionsort<Lit>( &(track_lits[0]) ,   track_lits .size() );
		
		cerr << "c clause to track:";
		for( uint32_t i = 0; i < track_lits.size(); ++i ) cerr << " " << track_lits[i].nr();
		cerr << endl;
	#endif

	
	std::vector< Clause* >& clause_set = *clauses;
	istream* fileptr;

	
	if( filename == 0 || strlen(filename) == 0 )
	{
		cerr << "c no problem file specified, use stdin" << endl;
		
		fileptr = &cin;
	} else {
		fileptr = new ifstream( filename, ios_base::in);
	}

	istream& file = *fileptr;

	if( !file )
	{
		cerr << "c can not open file " << filename << endl;
		exit( 0 );
	}

	std::vector< Lit > clause_lits;
	 clause_lits = std::vector< Lit >();  clause_lits.reserve( 120 );	
	string line;		
	uint32_t ind = 0;	
	uint32_t lines = 0;	
	
	while(getline (file, line)){
	 
		lines ++;
		ind = 0;

		if(line.size() == 0)
		 continue;
		
		while(line[ind] == ' ')
		 ind++;
		
		if(line[ind] == 10 || line[ind] == 13 || line[ind] == 0)
			continue;

		if(line[ind] == 'p')	
		 continue;

		if(line[ind] == 'c' || line[ind] == 'C')	
		 continue;
		
		if(line[ind] == '{' ){	
			while( line[ind] != '}' ) ind ++;		
		}
		
		
		if(line[ind] == '0'){
			
			
			cerr << "c found empty clause!" << endl;
			if( fileptr != &cin ) delete fileptr;
			return UNSAT;
		}

		
		if( line[ind] != '-' && ( line[ind] < '0' || line[ind] > '9' ) )
		{
			cerr << "invalid file ( symbol[" << (int)line[ind] << ", " << line[ind] << " ] in line " << lines << " at col " << ind << " : " << line << " )" << endl;
			exit(0);
		}

		
		  clause_lits .clear();
		while(line.size() > ind)	
		{
			
			int32_t number = 0;
			bool negative = false;
		
			if(line[ind] == '-')
			{
				negative = true;
				ind++;
			}
			
			while( line[ind] >= '0' && line[ind] <= '9' )
			{
				number *=10;
				number += line[ind++] - '0';
			}
			
			if( number == 0 ) break;	
			
			
			number = (negative) ? 0 - number : number;

			const Lit lit1 = Lit( number );
			  clause_lits.push_back( lit1 );
			
			if( var_cnt < lit1.toVar() ) var_cnt =  lit1 .toVar();
		
			
			while(line[ind] == ' ')
			 ind++;		
		}
		
		
		if(  clause_lits .size() == 0 ){
			if( fileptr != &cin ) delete fileptr;
			return UNSAT;
		}
		
		Clause* ref = addLiterals( clause_lits );
		if( ref != 0 ){
			 clause_set.push_back( ref );
		}
	 }
	 
	  if(print_dimacs){
		cout << "p cnf " << var_cnt << " " <<  (*clauses) .size() << endl;
		for(uint32_t ind_clause = 0; ind_clause <  (*clauses) .size(); ++ind_clause)
		{
			Clause& cl = *( (*clauses)[ ind_clause ] );
			for( uint32_t i = 0; i < cl.size(); ++i ) cout <<  cl.get_literal(i ).nr() << " ";
			cout << "0" << endl;
		}
		exit(12);
	  }
 
	{};
	if( fileptr != &cin ) delete fileptr;
	return UNKNOWN;

}

Clause* FileParser::addLiterals( Lit* clause_lits, uint32_t size ) const {
	
	selectionsort<Lit>( clause_lits , size );
	uint32_t i = 2;
	for( i = size - 1; i > 0; i-- ){
		
		if( clause_lits[i] ==  clause_lits[i-1] .complement() ) break;
		
		if( clause_lits[i] == clause_lits[i-1] ){
			for( uint32_t j = i+1; j < size - 1; ++j ) clause_lits[j] = clause_lits[j+1];
			--size;
		}
	}
	if( i == 0 ){
		return new Clause( clause_lits , size, false );
	}
	return 0;
}

Clause* FileParser::addLiterals( std::vector<Lit>& clause_lits ) const
{
	
	selectionsort<Lit>( &(clause_lits[0]) ,   clause_lits .size() );
	uint32_t i = 2;
	for( i =   clause_lits .size() - 1; i > 0; i-- ){
		
		if( clause_lits[i] ==  clause_lits[i-1] .complement() ) break;
		
		if( clause_lits[i] == clause_lits[i-1] ){
			  clause_lits.erase(  clause_lits.begin() +  i );
		}
	}
	if( i == 0 ){
		return new Clause( &(clause_lits[0]),   clause_lits .size(), false );
	}
	return 0;
}
