#!bin/bash

for i in {0..59}
do
	offset=$(($i*10))
	echo 
	echo Running: nice -n 19 ./compY -MC -D -t 10 -x 10 -o $offset
	nice -n 19 ./compY -MC -D -t 10 -x 10 -o $offset
	echo Offset range $offset done
done
