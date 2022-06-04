#!/bin/bash

# Create a new method in the cnfTools toolkit
# 
# Usage: newMethod.sh method_name

if [ $# -ne 1 ]
then
  echo "Usage: $0 method_name"
  exit -1
fi

name=$1
base=${0%/*}

# Steps required:
#
# - Copy include/template.h to include/$name.h
# - Copy src/template.cc to src/$name.cc
# - Replace "template" with $name in src/$name.cc and include/$name.h 
# - Modify solver.cc 
# - Compile
# - Run the solver

className="$name"Solver
tmp1="if( methodToUse == \"$name\" ){ \n\t\tcerr << \"c start $name\" << endl; \n\t\t$className solver(cmd); \n\t\treturn solver.solve(formula, varCnt); \n\t} else"

sed s/template/$name/g $base/include/template.h >  $base/include/$name.h
sed s/template/$name/g $base/src/template.cc > $base/src/$name.cc

cat $base/src/solver.cc |  \
sed "/#include \"template.h\"/a \#include \"$name.h\"" | \
sed  "/PASTE here/i \ \t$tmp1"  > solver.tmp
mv solver.tmp $base/src/solver.cc

cd $base

make clean && make
./formula -method=$name --verb-help

