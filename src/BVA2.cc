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


#include "BVA2.h"



BVA2Solver::BVA2Solver( const StringMap& commandline )
:
cmd( commandline )
{
	varCnt = 0;
	addedVariables = 0;
	minSize = 1;
	debug=0;
	limit = 10000000;
	print = false;
	push = 2;

	simplify = true;
	transform = true;
	treatComplement = true;
	
	set_parameter( commandline );
}
	
Assignment* BVA2Solver::solve( vector<Clause*>& formula, uint32_t vc )
{
	varCnt = vc;
	
	if( cmd.contains("-h") || cmd.contains("--help")  || cmd.contains("--verb-help")  ) return 0;
	
	
	if( !init( formula ) ) {
	  transform = false;
	  formula.clear();
	  vector< Lit > empty;
	  formula.push_back( new Clause(empty) );
	}
	
	vector<Lit> stack;
	
	uint32_t break_counter = 0;
	uint32_t duplicates = 0;
	uint32_t complementCount = 0;
	int32_t currentReduction = 0;
	
	if( outfileName.size() != 0 ) {
		outfile.open( outfileName.c_str(), std::ios::out );
	}
	
	if( transform ) {
	
	while (!work.is_empty ()) {
	  
          const uint32_t data = work.item_at (0);
          work.remove_item (data);
	  
	  
	  if( isInterupted() ) break;
	  
	  Lit tmpL;
	  tmpL.fromData( data );
	  const Lit right = tmpL;
	  
	  
	  
	  
	  matchingLiterals.clear();
	  
	  if( occ[ right.toIndex() ].size() < 3 ) continue;
	  if( debug > 0 ) cerr << "c work on " << right.nr() << endl;
	  
	  uint32_t index = 0;
	  while ( matchingClauses.size() < index+1 ) matchingClauses.push_back( vector<Clause*>() );
	  
	  matchingClauses[0].clear();
	  for( uint32_t i = 0 ; i < occ[ right.toIndex() ].size(); ++i )
	  {
	    matchingClauses[0].push_back( occ[ right.toIndex() ][i] );
	  }
	  
	  matchingLiterals.push_back(right); 
	  currentReduction = 0;	
	  
	  
	  while( true )
	  {
	  
	  stack.clear();
	    
	  index ++; 
	  
	  
	  
	  for( uint32_t i = 0 ; i <  matchingClauses[0].size(); ++i )
	  {
	    
	    while ( matchingClauses.size() < index+1 ) matchingClauses.push_back( vector<Clause*>() );
	    
	    
	    Clause* C =  matchingClauses[0][i];
	    if( C == 0 ) {
	      continue;
	    }
	    
	    
	    
	    if( C->size() < minSize + 1 ) continue;
	    
	    Lit l1 = NO_LIT;
	    uint32_t min = 0;
	    bool rightInFlag=false;
	    for( uint32_t j = 0 ; j < C->size(); ++ j ) {
	      const Lit& lt = C->get_literal(j);
	      mark[lt.toIndex()] = C;
	      if( lt == right ) { mark[lt.toIndex()] = 0; rightInFlag=true; continue; }
	      
	      if( debug > 1 ) cerr << "c mark " << lt.nr() << endl;
	      
	      if ( l1 == NO_LIT ) {
		l1 = lt; min = occ[ lt.toIndex() ].size();
	      } else {
		if( min > occ[ lt.toIndex() ].size() ) {
		  min = occ[ lt.toIndex() ].size();
		  l1 = lt;
		}
	      }
	    }
	    if( !rightInFlag ) {
	      cerr << "c literal " << right.nr() << " is not in selected clause ";
	      printClause(C);
	      assert ( rightInFlag );
	    }
	    
	    
	    assert( l1 != right );
	    assert( l1 != NO_LIT) ;
	    
	    
	    if( debug > 1 ) {
	      cerr << "c candidate clause: ";
	      for( uint32_t tmp = 0; tmp < C->size(); ++ tmp ) {
		cerr << C->get_literal(tmp).nr() << " ";
	      } cerr << "0" << endl;
	    }
	    if( debug > 1 ) cerr << "c smallest occuring literal " << l1.nr() << endl;
	    
	    if( l1 == NO_LIT ) continue;
	    
	    if( occ[ l1.toIndex() ].size() < 2 ) continue; 
	    
	    
	    for( uint32_t j = 0 ; j < occ[ l1.toIndex() ].size(); ++j ) {
	      const Clause* D = occ[ l1.toIndex() ][j];
	

	      if( ++break_counter >= limit && limit != -1) 
		goto break_BVA;
	      
	      if( D == C ) continue;
	      
	      if( C->size() != D->size() ){
		if( debug > 2 ) {
		  cerr << "c reject because of size: ";
		  for( uint32_t tmp = 0; tmp < D->size(); ++ tmp ) {
		    cerr << D->get_literal(tmp).nr() << " ";
		  } cerr << "0" << endl;
		}
		continue;	
	      }

	      
	      Lit match = NO_LIT;
	      assert ( mark[ l1.toIndex() ] == C );
	      
	      for( uint32_t k =0; k < D->size(); ++k ) {
		const Lit l2 = D->get_literal(k);

		
		if( l2 == right ) goto nextD;
		
		if( mark[ l2.toIndex() ] != C ) {
		  if( match == NO_LIT ) {
		    match = l2;
		  }
		  else {
		    if( debug > 2 ) {
		      cerr << "c reject because of not enough matching literals: (last=" << match.nr() << ", this=" << l2.nr() << ")";
		      for( uint32_t tmp = 0; tmp < D->size(); ++ tmp ) {
			cerr << D->get_literal(tmp).nr() << " ";
		      } cerr << "0" << endl;
		    } 
		    goto nextD;
		  }
		}
	      }
	      
	      
	      if( match == NO_LIT ) { duplicates++; continue; }
	      
	      assert ( match != right );
	      
	      if( debug > 1 ) cerr << "c found match: " << match.nr() << endl;
	
	      
	      if (countMark[ match.toIndex() ] == right ) {
		countCount[ match.toIndex() ] ++;
		countSize[ match.toIndex() ] += D->size() - 1;
		if( debug > 1 ) cerr << "c set count[" << match.nr() << "] to " << countCount[ match.toIndex() ] << endl;
	      } else {
		
		countMark[ match.toIndex() ] = right;
		countCount[ match.toIndex() ] = 1;
		countSize[ match.toIndex() ] = D->size() - 1;
		
		stack.push_back( match );
		if( debug > 1 ) cerr << "c set count[" << match.nr() << "] to " << countCount[ match.toIndex() ] << endl;
	      }
	      
	      nextD:;
	    }
	    
	  }
	  
	  
	  if( stack.size() == 0 ) break;
	  
	  uint32_t max = 0;
	  Lit left = NO_LIT; 
	  bool foundRightComplement = false;
	  
	  for( uint32_t i = 0 ; i < stack.size(); ++i ){
	    assert( countMark[ stack[i].toIndex() ] == right );
	    
	    const Lit stackLit = stack[i];
	    
	    
	    
	    for(  uint32_t j = 0; j < matchingLiterals.size(); ++ j ) {
	      if( matchingLiterals[j] == stackLit ) {
		
		goto nextStackLiteral;
	      }
	    }
	    
	    if( stack[i] == right.complement() ) foundRightComplement = true;
	    
	    countMark[ stack[i].toIndex() ] = 0;
	    
	    if( countCount[ stack[i].toIndex() ] > max ){
	      left = stack[i];
	      assert(left != right );
	      max = countCount[ left.toIndex() ];
	      
	    }
	    nextStackLiteral:;
	  }
	  
	  
	  if( treatComplement && foundRightComplement ) left = right.complement();
	  
	  
	  if( left == right.complement() && treatComplement ) {
	    handleComplement( right );
	    complementCount ++;
	    
	    break;
	  }
	  
	  
	  
	  if( index == 1 && max < 3 ) {
	    break;
	  }
	  
	  
	  if ( debug > 1 ) cerr << "c found maximum left= " << left.nr() << " (" << countCount[ left.toIndex() ] << ")" << endl;
	  
	  
	  int32_t newReduction = ( (int)(matchingLiterals.size() + 1) * (int)max ) - ( (int)max + (int)(matchingLiterals.size() + 1) ); 
	  
	  
	  if( newReduction > currentReduction )
	  {
	    if( debug > 0 ) cerr << "c set current reduction of literal " << right.nr() << " to " << newReduction  << " lits=" << matchingLiterals.size() << " match=" << max << endl;
	    currentReduction = newReduction;
	    matchingLiterals.push_back( left );
	  } else {
	    
	    break;
	  }
	  
	  if( replaced.size()  <= countCount[ left.toIndex() ] ) replaced.resize( countCount[ left.toIndex() ] + 10, 0 );
	  replaced[ countCount[left.toIndex()] ] ++;
	  
	  const uint32_t matches = countCount[ left.toIndex() ];

	  
	  while ( matchingClauses.size() < matchingLiterals.size() ) matchingClauses.push_back( vector<Clause*>() );
	  
	  
	  matchingClauses[index].clear();
	  matchingClauses[index].assign( matchingClauses[0].size(),0);
	  
	  

	  assert (max <= occ[ left.toIndex() ].size() );
	  const vector<Clause*>& rightList = matchingClauses[ 0 ];
	  vector<Clause*>& leftList = occ[ left.toIndex() ]; 
	  uint32_t foundCurrentMatches = 0;
	  
	  for( uint32_t i = 0 ; i < rightList.size(); ++i ) {
	    if( rightList[i] == 0 ) {
	      continue; 
	    }
	    Clause* C = rightList[i];

	    for( uint32_t j = 0; j < C->size(); j ++ ) {
	      if( C->get_literal(j) == right ) continue;
	      mark[ C->get_literal(j).toIndex() ] = C;
	    }
	    
	    if ( mark[ left.toIndex() ] == C ) { 
	      matchingClauses[ 0 ][i] = 0;	
	      continue;
	    }
	    mark[ right.toIndex() ] = 0; mark[ left.toIndex() ] = C;
	    unsigned oldFoundCurrentMatches = foundCurrentMatches;
	    for( uint32_t j = foundCurrentMatches ; j < leftList.size(); ++j ) {
	      Clause* D = leftList[j];

	      if( D->size() != C->size() ) continue; 
	      for( uint32_t k = 0 ; k < D->size() ; ++ k ){
		if( mark[ D->get_literal(k).toIndex() ] != C ){

		  goto nextD2;
		}
	      }
	      matchingClauses[index][i] = D;
	      
	      
	      {
	      Clause* tmp2 = leftList[ foundCurrentMatches ];
	      leftList[ foundCurrentMatches ] = D;
	      leftList[ j ] = tmp2;
	      }
	      foundCurrentMatches++;
	      break;
	      nextD2:;
	    }
	    mark[ left.toIndex() ] = 0;
	    
	    if( foundCurrentMatches == oldFoundCurrentMatches ) matchingClauses[0][i] = 0;
	  }
	  



	  
	  
	  } 
	  
	  
	  if( matchingLiterals.size() < 2 ) continue;
	  
	  
	  const Var newX = nextVariable();
	  if( outfileName.size() != 0 ) {
		  outfile << newX << endl;
	  }
	  
	  if (debug > 0 ) cerr << "c new variable " << newX << " reduces formula by: " << currentReduction  << " with " << matchingLiterals.size() << " lits" << endl;
	  
	  Lit replaceLit = Lit( newX, NEG );
	  
	  assert( matchingLiterals.size() <= matchingClauses.size() ); 
	  
	  for( uint32_t i = 0 ; i < matchingLiterals.size(); ++ i ) {
	    vector<Clause*>& list = matchingClauses[i];
	    if( i == 0 ) {	
	      
	      
	      for( uint32_t j = 0 ; j < list.size(); ++ j )
	      {
		if( list[j] == 0 ) continue; 
		bool replaceFlag = false;
		for( uint32_t k = 0 ; k< list [j]->size(); ++k ) {
		  if( list [j]->get_literal( k ) == right ) { 
		    list [j]->set_literal( k, replaceLit );
		    occCount[ right.toIndex() ] --;
		    replaceFlag = true;
		  }
		}
		if( !replaceFlag ) {
		  cerr << "c could not replace literal " << right.nr() << " to " << replaceLit.nr() << " in clause "; printClause( list[j] );
		  assert( replaceFlag );
		}
	      
		
		for( uint32_t k = 0 ; k < occ[ right.toIndex() ].size(); ++k ) {
		  if( list[j] == occ[ right.toIndex() ][k] ) {
		    occ[ right.toIndex() ][k] = occ[ right.toIndex() ][ occ[ right.toIndex() ].size() - 1 ];
		    occ[ right.toIndex() ].pop_back();
		    break;
		  }
		}
		
		
		occ[ replaceLit.toIndex() ]. push_back( list[j] );
		
		occCount[ replaceLit.toIndex() ] ++;
	      }
	    } else {
	      assert( list.size() == matchingClauses[0].size() );
	      
	      for( uint32_t j = 0 ; j < list.size(); ++ j ) {
		if( matchingClauses[0][j] != 0 && list[j] != 0 ) removeClause( list[j] ); 
	      }
	    }
	  }
	  
	  if( matchingLiterals.size() < 2 ) {
	    cerr << "c to few matching literals: ";
	    for( unsigned i = 0 ; i < matchingLiterals.size(); ++ i ) {
	     cerr << " " << matchingLiterals[i].nr(); 
	    }cerr << endl;
	    cerr << "c index =" << index << endl;
	    cerr << "c stack:";
	    for( unsigned i = 0 ; i < stack.size(); ++i ) {
	     cerr << " " << stack[i].nr() << "["<< countCount[stack[i].toIndex()] <<"]"; 
	    } cerr << endl;
	  }
	  assert( matchingLiterals.size() > 1 );
	  
	  vector< Lit > clauseLits;
	  clauseLits.clear();
	  clauseLits.push_back( Lit( newX, POS ) );
	  clauseLits.push_back( NO_LIT );
	  
	  for( uint32_t i = 0; i < matchingLiterals.size(); ++i ) {
	    clauseLits[1] = matchingLiterals[i];
	    formula.push_back( addClause( new Clause( clauseLits ) ) );
	  }
	  
	  if( push > 0 ) {
	    work.insert_item( right.data() );
	  }
	  if( push > 1 ) {
	    if( matchingLiterals.size() > 2 ) work.insert_item( Lit( newX, POS ).data() ); 
	    work.insert_item( Lit( newX, NEG ).data() );
	  }

	
	  
	} 
	
	} 

	break_BVA:;

	
	cerr << "c duplicate clauses:   " << duplicates << endl;
	cerr << "c complement literals: " << complementCount << endl;
	
	cerr << "c replaced:";
	for( uint32_t i = 0 ; i < replaced.size(); ++i ) {
	  if( replaced[i] > 0 ) {
	    cerr << " " << i << ":" << replaced[i] << " ";
	  }
	}
	cerr << endl;
	
	uint32_t cls = 0;
	for( uint32_t i = 0 ; i < formula.size(); ++i ){
	  cls = formula[i]->isIgnored() ? cls : cls + 1;
	}
	  
	cerr << "vars: " << varCnt + addedVariables << " csl: " << cls << endl;
	
	if( print )
	{
	  stringstream s;
	  uint32_t cls = 0;
	  for( uint32_t i = 0 ; i < formula.size(); ++i ){
	    cls = formula[i]->isIgnored() ? cls : cls + 1;
	  }
	  
	  s << "p cnf " << varCnt + addedVariables << " " << cls << endl;
	  cout << s.str();
	  for( uint32_t i = 0 ; i < formula.size(); ++i )
	  {
	    stringstream s;
	    const Clause& clause = *( formula[i] );
	    if( clause.isIgnored() ) continue;
	    
	    for( uint32_t j = 0 ; j < clause.size(); ++ j ) {
    		s << clause.get_literal(j).nr() << " ";
	    }
	    s << "0" << endl;
	    cout << s.str();
	  } 
	}
	
	return 0;
}


uint32_t BVA2Solver::addMatches( const vector<Clause*>& rightList, vector<Clause*>& leftList, const Lit right, const Lit left, uint32_t index )
{
	  uint32_t foundCurrentMatches = 0;
	  for( uint32_t i = 0 ; i < rightList.size(); ++i ) {
	    if( rightList[i] == 0 ) continue; 
	    Clause* C = rightList[i];

	    for( uint32_t j = 0; j < C->size(); j ++ ) {
	      if( C->get_literal(j) == right ) continue;
	      mark[ C->get_literal(j).toIndex() ] = C;
	    }
	    
	    if ( mark[ left.toIndex() ] == C ) continue;
	    mark[ right.toIndex() ] = 0; mark[ left.toIndex() ] = C;
	    for( uint32_t j = foundCurrentMatches ; j < leftList.size(); ++j ) {
	      Clause* D = leftList[j];

	      if( D->size() != C->size() ) continue; 
	      for( uint32_t k = 0 ; k < D->size() ; ++ k ){
		if( mark[ D->get_literal(k).toIndex() ] != C ){

		  goto nextD2;
		}
	      }
	      if( index == 1 ) {
		matchingClauses[0].push_back(C);
		matchingClauses[1].push_back(D);

	      } else {
		
		matchingClauses[index][i] = D;

	      }
	      
	      {
	      Clause* tmp2 = leftList[ foundCurrentMatches ];
	      leftList[ foundCurrentMatches ] = D;
	      leftList[ j ] = tmp2;
	      }
	      foundCurrentMatches++;
	      break;
	      nextD2:;
	    }
	    mark[ left.toIndex() ] = 0;
	  }
	  
	  return foundCurrentMatches;
}

void BVA2Solver::handleComplement( const Lit right )
{
  toChangeClauses.clear();
  const Lit left = right.complement();
  for( uint32_t i = 0 ; i < occ[ right.toIndex() ].size(); ++i )
  {
    Clause* C = occ[ right.toIndex() ][i];
    for( uint32_t j = 0; j < C->size(); j ++ ) {
      if( C->get_literal(j) == right ) continue;
      mark[ C->get_literal(j).toIndex() ] = C;
    }
    
    if ( mark[ left.toIndex() ] == C ) continue;
    mark[ right.toIndex() ] = 0;	
    mark[ left.toIndex() ] = C;
    
    for( uint32_t j = 0 ; j < occ[ left.toIndex() ].size(); ++j ) {
      Clause* D = occ[ left.toIndex() ][j];
      if( D->size() != C->size() ) continue; 
      
     if( debug > 1 ) cerr << "c check clause " << j << " of literal " << left.nr() << endl;
      for( uint32_t k = 0 ; k < D->size() ; ++ k ){
	if( mark[ D->get_literal(k).toIndex() ] != C ){
	  if( debug > 1 )  cerr << "c reject clause because of marks (" << D->get_literal(k).nr() << endl;
	  goto nextD2;
	}
      }
      
      toChangeClauses.push_back(C);
      
      occ[ right.toIndex() ][i] = occ[right.toIndex()][ occ[right.toIndex()].size() -1 ];
      occ[right.toIndex() ].pop_back();
      --i;
      
      removeClause(D);
      
      break;
      
      nextD2:;
    }
    mark[ left.toIndex() ] = 0;
  }

  
  for( uint32_t i = 0; i < toChangeClauses.size(); ++i ) toChangeClauses[i]->remove( right );
  
  if( push > 0 ) work.insert_item( right.data() );
}

int32_t BVA2Solver::reduction( int32_t left, int32_t right )
{
  return (left*right) - left - right;
}

bool BVA2Solver::propagate(vector<Clause*>& formula) {
  Assignment assignment( varCnt );
  deque < Lit > unitQueue;
  
  
  for( uint32_t i = 0 ; i < formula.size(); ++i )
  {
     Clause& clause = *( formula[i] );
     if( clause.isIgnored() ) continue;
     
     if( clause.size() == 1 ) {
       clause.ignore();
       const Lit& l = clause.get_literal(0);
       if( assignment.is_unsat( l ) ) {
	  assignment.destroy(varCnt);
	  return false;
       }
       if( assignment.is_sat(l) ) continue;
       unitQueue.push_back(l);
       assignment.set_polarity(l.toVar(),l.getPolarity());
     } else {
	if( clause.size() == 0 ) {
	  assignment.destroy(varCnt);
	  return false;
	}
     }
  } 
  
  cerr << "c propagte " << unitQueue.size() << " initial units" << endl;
  bool conflict = false;
  while( ! unitQueue.empty() ) {
    Lit l = unitQueue.front();
    unitQueue.pop_front();
    
    for( uint32_t i = 0 ; i < occ[ l.complement().toIndex() ].size(); ++i ) {
      Clause& clause = *(occ[ l.complement().toIndex() ][i]);
      if( clause.isIgnored() ) continue;
      
      uint32_t j = 0;
      for(  ; j < clause.size(); ++ j ) {
	if( assignment.is_sat( clause.get_literal(j) ) ) { 
	  clause.ignore();
	  break;
	}
      }
      
      if( j != clause.size() ) {
	--i; 
	continue;
      }
      
      Lit fl = NO_LIT;
      for( j = 0; j < clause.size(); ++ j ) {
	if( assignment.is_undef( clause.get_literal(j).toVar() ) ) {
	  if( fl == NO_LIT ) fl = clause.get_literal(j);
	  else break;
	}
      }
      
      if( j == clause.size() ) {
	if( fl != NO_LIT ) { 
	  unitQueue.push_back(fl);
	  assignment.set_polarity( fl.toVar(), fl.getPolarity() );
	  clause.ignore();
	}
	else {
	  assignment.destroy(varCnt);
	  return false;
	}
      }
    }
  }
  
  
  for( uint32_t i = 0 ; i < formula.size(); ++i )
  {
    Clause* clp = formula[i];
    Clause& clause = *clp;
    
    for( uint32_t j = 0 ; j < clause.size(); ++ j ) {
      if( assignment.is_unsat( clause.get_literal(j) ) )
      {
	clause.remove_index(j);
      }
    }
  }

  
  for( Var v = 1; v <= varCnt; ++v ) {
    if( ! assignment.is_undef(v) ) {
      Lit l = Lit( v, assignment.get_polarity(v) );
      formula.push_back( new Clause( &l,1) );
    }
  }
  
  assignment.destroy(varCnt);
  return true;
}

void BVA2Solver::printClause( const Clause* clause ) {
  for ( uint32_t i = 0 ; i < clause->size(); ++i ) cerr << " " << clause->get_literal(i).nr();
  cerr << endl;
}

void BVA2Solver::printFormula(vector<Clause*>& formula)
{
 for( uint32_t i = 0 ; i < formula.size(); ++i )
  {
     const Clause& clause = *( formula[i] );
     if( clause.isIgnored() ) continue;
     
     for( uint32_t j = 0 ; j < clause.size(); ++ j ) {
	cout << clause.get_literal(j).nr() << " ";
     }
     cout << "0" << endl;
  } 
}

void BVA2Solver::removeClause( Clause* clause ) {
  for( uint32_t i = 0 ; i < clause->size(); ++i ) {
    vector< Clause* >& list = occ[ clause->get_literal(i).toIndex() ];
    
    bool flag = false;
    for( uint32_t j = 0 ; j < list.size(); ++j ) {
      if( list[j] == clause ) {
	list[j] = list[ list.size() - 1 ];
	list.pop_back();
	flag = true; 
      }
    }
    if(!flag) {
      cerr << "c not properly stored in list of literal " << clause->get_literal(i).nr() << ": ";
      printClause( clause );
      assert(false);
    }
    
    
  }
  
  for( uint32_t i = 0 ; i < clause->size(); ++i ) {
    const Lit& l = clause->get_literal(i);
    occCount[ l.toIndex() ] --;
    if( work.contains( l.data() ) ) work.update_item( l.data() );
  }
  
  clause->ignore();
}

Clause* BVA2Solver::addClause( Clause* clause ) {
  for( uint32_t j = 0; j < clause->size(); ++j )
  {
    const Lit& l = clause->get_literal(j);
    occ[ l.toIndex() ].push_back( clause );
    occCount[ l.toIndex() ] ++;
    
    if( work.contains( l.data() ) ) work.update_item( l.data() );
  }
  return clause;
}

Var BVA2Solver::nextVariable() {
    addedVariables++;
    
    
    if( capacity <= max_index( varCnt+addedVariables ) ) {
      
      
      occCount = (uint32_t*) realloc ( occCount, sizeof(uint32_t) * 2 * capacity ) ;
      
      uint32_t* lastElement = & ( occCount[ capacity ] );
      memset( lastElement, 0, sizeof(uint32_t) * capacity );
      
      capacity = capacity * 2;
    }
    
    
    work.resize( max_index(varCnt+addedVariables) , &occCount );
    
    vector<Clause*> tmp;
    
    occ.push_back( tmp );
    occ.push_back( tmp );
    
    mark.push_back( 0 ); mark.push_back( 0 );
    countMark.push_back( NO_LIT );countMark.push_back( NO_LIT );
    countCount.push_back( 0 );countCount.push_back( 0 );
    countSize.push_back( 0 );countSize.push_back( 0 );
    
    return varCnt+addedVariables;
}

bool BVA2Solver::init( vector<Clause*>& formula ) {
  occ.clear();
  occ.resize( max_index(varCnt) );
  
  capacity = max_index( varCnt ) * 2;
  occCount = (uint32_t*) malloc ( sizeof(uint32_t) * capacity ) ;
  memset( occCount, 0, sizeof(uint32_t) * capacity );
  
  work = Heap < uint32_t, greater < uint32_t > >(&occCount, max_index(varCnt) );
  
  for( uint32_t i = 0 ; i < formula.size(); ++i )
  {
    Clause& c = * (formula[i]);
    if( c.isIgnored() ) cerr << "c ignored clause in formula" << endl;
  }
  
  
  if( simplify ) { 
    
    for( uint32_t i = 0 ; i < formula.size(); ++i )
    {
      const Clause* clp = formula[i];
      const Clause& clause = *clp;
  
      addClause( formula[i] );
      
      if( clause.size() == 0 ) cerr << "c found empty clause in formula!" << endl;
    }
    
    if( !propagate( formula ) ) {
      return false;
    }
    
  
    removeDuplicates(formula);

    
    for( Var v = 1 ; v <= varCnt; ++v ) {
      occ[ Lit(v,POS).toIndex() ].clear();
      occ[ Lit(v,NEG).toIndex() ].clear();
    }
  
  }
  
  for( uint32_t i = 0 ; i < formula.size(); ++i )
  {
    const Clause* clp = formula[i];
    const Clause& clause = *clp;
    if( clause.isIgnored() ) continue; 
    addClause( formula[i] );
    
    if( clause.size() == 0 ) cerr << "c found empty clause in formula!" << endl;
  }
  
  for( Var v = 1 ; v <= varCnt; ++v ) {
    Lit l = Lit( v, POS );
    if( occ[l.toIndex() ].size() > 2 ) {
      work.insert_item( l.data() );
    }
    l = l.complement();
    if( occ[l.toIndex() ].size() > 2 ) {
      work.insert_item( l.data() );
    }
  }
  
  mark.resize( max_index(varCnt), 0 );
  countMark.resize( max_index(varCnt), NO_LIT );
  countCount.resize( max_index(varCnt), 0 );
  countSize.resize( max_index(varCnt), 0 );

  return true;
}

void BVA2Solver::removeDuplicates(vector< Clause* >& clauses){

	
	for( int32_t i = 0 ; i < clauses.size(); i++ ){
		if( clauses[i]->isIgnored() ) {
		  clauses[i] = clauses[ clauses.size() - 1 ];
		  clauses.pop_back();
		  --i;
		} else {
		  clauses[i]->sort();
		}
	}
	
	int32_t n = clauses.size();
	int32_t m, s;
	
	Clause** a=new Clause*[ n ];
	
	for( int32_t i = 0 ; i < n; i++ ){
		if( clauses[i]->isIgnored() ) assert(false);
		a[i] = clauses[i];
	}
	
	Clause** original = a;	
	Clause** b=new Clause*[ n ];

	
	for (s=1; s<n; s+=s)
	{
		m = n;
		do {
			m = m - 2*s;	
			int32_t hi = (m+s > 0) ? m + s : 0;	
			
			int32_t i = (m > 0) ? m : 0;	
			int32_t j = hi;
			
			int32_t stopb = m + 2*s;	
			int32_t currentb = i;			
			
			
			while( i < hi && j < stopb)
			{
				if( *(a[i]) < *(a[j])  )
					b[currentb++] = a[i++];
				else
					b[currentb++] = a[j++];
			}
			
			for( ; i < hi; )
				b[currentb++] = a[i++];
				
			for( ; j< stopb; 	)
				b[currentb++] = a[j++];
				
		} while( m > 0 );
		
		
		Clause** tmp = a;
		a = b;
		b = tmp;
	}
	
	delete [] b;
	
	for( int32_t i = 0 ; i < n; i++ ){
		clauses[i] = a[i];
	}
	
	
	  for( uint32_t i = clauses.size() - 1; i >=  1; --i ) {
		
		Clause& c1 = *( clauses[i] );
		if( c1.isIgnored() ) continue;

		Clause& c2 = *( clauses[i-1]);
	  	if( c1.size() != c2.size() ) continue;
	  	
	  	uint32_t j = 0;
	  	for( ; j < c1.size(); ++j ) {
	  		if( c1.get_literal(j) != c2.get_literal(j) ) break;
	  	}
	  	
	  	if( j == c1.size() ) {
	  		
	  	
		
	  		clauses[i]->ignore();
			
			
	  	}
	  }
	
	return;
}

void BVA2Solver::set_parameter( const StringMap& commandline )
{
	
	if( commandline.contains( (const char*)"--verb-help" ) || commandline.contains( (const char*)"--help" ) )
		cerr << endl<< "=== BVA solver information ===" << endl;
	
	
	commandline.updateParameter( "debug", debug, "debug level" ,0,3);
	commandline.updateParameter( "minMatch", minSize, "minimum number of matching literals", 1,1000000000);
	commandline.updateParameter( "print", print, "print formula");
	commandline.updateParameter( "push",push,"push variables back into the queue (0=none, 1=original, 2=all)",0,2);
	
	commandline.updateParameter( "newVarFile", outfileName, "file to output all the variables that have ben added");

	commandline.updateParameter( "simplify", simplify, "run UP and duplicate removal");
	commandline.updateParameter( "transform", transform, "run BVA on simplified formula");
	commandline.updateParameter( "complement", treatComplement, "treat complement pairs special");
	commandline.updateParameter( "limit", limit, "when to abort BVA (-1 = unlimited)", -1,3000000000);

	
	
	if( minSize < 1 ) minSize = 1;
}
