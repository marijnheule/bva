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
 



#include "commandlineparser.h"

StringMap CommandLineParser::parse(int32_t argc, char* argv[], bool binary)
{
	map.clear();

	
	
	map.insert(  (char*)"restart_event", (char*)"geometric");
	map.insert(  (char*)"restart_event_geometric_max", (char*)"150");
	map.insert(  (char*)"restart_event_geometric_nested", (char*)"1");
	map.insert(  (char*)"restart_event_geometric_inc", (char*)"1.3");
	map.insert(  (char*)"rem_keep_last", (char*)"1");
	map.insert(  (char*)"CP_er", (char*)"1" );

	int32_t scan_begin = 0;
	if( binary){
		
		map.insert( (char*)"binary", argv[0] );
		scan_begin++;
	}
	
	
	if( scan_begin == 1 && argc > 1 )
	{
		if( argv[1][0] != '-' )
		{
			map.insert(  (char*)"file", argv[1] );
			scan_begin = 2;
		}
	}
	
	
	if( scan_begin == 2 && argc > 2 ){
		if( argv[2][0] != '-' )
		{
			map.insert(  (char*)"outfile", argv[2] );
			scan_begin = 3;
		}
	}

	
	for( int32_t i = scan_begin; i < argc; ++i )
	{
		if( strlen( argv[i] ) == 1 )
		{
			map.insert( argv[i], 0 );	
			continue;
		}
		
		if( argv[i][0] == '-')
		{
			string s( argv[i] );
			int32_t pos = s.find("=");
			
			if( s.find("=") != string::npos ){
			  string key = s.substr(1,pos-1);
			  string value = s.substr(pos+1);
			  
			  if( value.size() > 0 ) map.insert(  key.c_str(), value.c_str() );  
			  else {
			    
			    if( key == "file" ){
			      
			      if( i + 1< argc ){
				if( argv[i+1][0] != '-' ){
				  map.insert(  key.c_str(),argv[i+1] );
				  ++i;
				}
			      }
			    }
			  }
			  continue;
			}
		}
		
		map.insert(  argv[i], 0 );
	}
	
	
	if( map.contains( (const char*)"configfile" ) ){
		parseFile( map.get( (const char*)"configfile" ), map );
	}
	if( map.contains( (const char*)"cf" ) ){
		parseFile( map.get( (const char*)"cf" ), map );
	}	
	
	
	if( map.contains( (const char*)"-h" ) || map.contains((const char*)"--help" ) || map.contains((const char*)"--verb-help" ) ){
#ifdef CSPSOLVER
		print_csp_help();
#else
		print_sat_help();
#endif
		if( !map.contains((const char*)"--help" ) && !map.contains((const char*)"--verb-help" )) exit( -1 );
	}




#ifdef COMPETITION
	map.remove( "-h" );
	map.remove( "--help" );
	map.remove( "--verb-help" );
	map.remove( "-v" );
	map.remove( "-i" );
#endif


#ifdef TESTBINARY
	map.remove( "-v" );
	map.remove( "-i" );
#endif
	
	
	if(  map.contains( (const char*)"-v" ) )
	{
		cerr << "c commandline parameter:" << endl;
		for( uint32_t i = 0 ; i < map.size(); ++i )
		{
			cerr << "c " << map[i].key << " -- ";
			if( map[i].value.size() > 0 ) cerr << map[i].value;
			cerr << endl;
		}
	}
	
	if( map.contains("-h") ||  map.contains("--help") ||  map.contains("--verb-help") ){
		map.remove("file");
	}
	
	return map;
}


bool CommandLineParser::parseFile( string filename, StringMap& thisMap ){
	
	vector<string> fileargv;
	ifstream file;

	
	file.open(  filename.c_str() , ios_base::in);
	if( !file ){
		cerr << "c can not open configfile " << filename << endl;
		return false;
	}
	string line;
	while(getline (file, line)){
		fileargv.push_back(line);
	}
	
	for( uint32_t i = 0; i < fileargv.size(); ++i ){
		if( fileargv[i].size() == 1 ){
			thisMap.insert( (char*)fileargv[i].c_str(), 0 );	
			continue;
		}
		
		if( fileargv[i][0] == '-')
		{
			string s( fileargv[i] );
			int32_t pos = s.find("=");
			cerr << "found = at " << pos << endl;
			if( s.find("=") != string::npos ){
			  string key = s.substr(1,pos-1);
			  string value = s.substr(pos+1);
			  cerr << "found key: " << key << " and value " << value << endl;
			  if( value.size() > 0 ) map.insert(  key.c_str(), value.c_str() );  
			  continue;
			}
		}
		
		thisMap.insert(  fileargv[i].c_str(), 0 );
	}
	return true;
}


bool CommandLineParser::parseString( const string& commands, StringMap& thisMap ){
  Tokenizer tokenizer(commands);
  tokenizer.addSeparator(" ");
  tokenizer.addSeparator("\t");
  
  vector< string > parameterLines;
  
  while( tokenizer.hasNext() ){
    
	string token = tokenizer.nextToken();
	
      
	if( token.size() == 1 ){
		thisMap.insert( (char*)token.c_str(), 0 );	
		continue;
	}
	
	if( token[0] == '-')
	{
		int32_t pos = token.find("=");
	
		if( token.find("=") != string::npos ){
		  string key = token.substr(1,pos-1);
		  string value = token.substr(pos+1);
	
		  if( value.size() > 0 ) thisMap.insert(  key.c_str(), value.c_str() );  
		  continue;
		}
	}
	
	
	thisMap.insert(  token.c_str(), 0 );
  }
  return true;
}


void CommandLineParser::print_sat_help()
{
		string binary = map.get((const char*)"binary" );
		cerr << "usage:" << endl;
		cerr << binary << " [File] [Outfile] -Name=Value" << endl;
		cerr << binary << " -file=Filename -outfile=Filename -Name=Value" << endl;
		cerr << binary << " -file=Filename -configfile=ConfigFile" << endl << endl;
		cerr << "examples:" << endl;
		cerr << binary << " sat.cnf solution.cnf" << endl;
		cerr << binary << " -solver=template -file=sat.cnf -outfile=solution.cnf" << endl << endl;
		cerr << "explanation:" << endl;
		cerr << "\t\tName ... name of the parameter." << endl;
		cerr << "\t\tValue ... value of the according parameter." << endl << endl;
		cerr << "\t\tcf ,configfile ... txt file where further arguments are listed(one per line)" << endl << endl;
		cerr << "\t\t-h          ... shows this info." << endl ;
		cerr << "\t\t--help      ... shows long help" << endl ;
		cerr << "\t\t--verb-help ... shows verbose long help" << endl ;
		cerr << "\t\t-q     ... stops outputing the found satisfying assignment." << endl;
		cerr << "\t\t-s     ... output only stdout of selected tool" << endl;
}


void CommandLineParser::print_csp_help()
{
		string binary = map.get((const char*)"binary" );
		cerr << "csp solving is not supportet at the moment" << endl;
}
