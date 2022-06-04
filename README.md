 ============================
| cnfTools / formula Toolkit | 
 ============================

===== Table of Contents =====

 1. Introduction
 2. Getting started
 3. Available methods
 4. Adding new methods
(5). Output syntax [[Tentative]]

===== 1. Introduction =======

cnfTools is a collection of algorithms for SAT formulas.
The goal is to create a "swiss-knife" tool that allows 
simple analysis and modification of SAT formulas.

This README presents an overview of the available methods
and explains how to implement new ones.
More detailed descriptions of each method can be found in 
the ./doc directory.

===== 2. Getting started ====

The toolkit can be compiled in the usual way:

$ make 

this will generate an executable called 'formula'.

To run a particular method (e.g. np-encoder) type:

$ ./formula input_file.cnf -method=np-encoder 

to get help on the usage of a method, type 

$ ./formula -method=np-encoder --verb-help

to get generic help type:

$ ./formula -h

===== 3. Available Methods ===

The following methods are available in cnfTools:

* 

===== 4. Adding New Methods ==

The following describe the procedure required for adding 
a new method:


1) Choose a name for your method: e.g. newCoolMethod.
   Note: the name needs to follow the standard for C variables.

2) Run ./newMethod newCoolMethod

