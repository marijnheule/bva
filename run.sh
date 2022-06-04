INPUT=$1

./bva -method=BVA2 -file= $INPUT -print=1 -limit=-1 | grep -v "UNKNOWN" > tmp-$$.txt

cat tmp-$$.txt | grep -e "r " -e "d " | sed 's|r ||' > proof-$$.drat
cat tmp-$$.txt | grep -v -e "r " -e "d " > formula-$$.cnf
rm tmp-$$.txt

# cadical formula-$$.cnf formula-$$.drat --no-binary

# cat formula-$$.drat >> proof-$$.drat

# ~/drat-trim/drat-trim $INPUT proof-$$.drat



