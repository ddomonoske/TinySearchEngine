# ENGS 50 Module 6 Testing Script F20
# Author Brandon Guzman


./querier ./ # number of arg test 

./querier ./ ../indexer/indexName1 # not valid crawler dir test

./querier ./ file   # unreadable file test 

valgrind ./querier ../pages ../indexer/indexName1 < error-queries   # prof's error tests

valgrind querier ../pages ../indexer/indexName1 -q good-queries.txt myoutput  # run quietly test with professor's queries 



