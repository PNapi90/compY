#!bin/bash

for i in {0..29}
do
	offset=$(($i*20))
	echo 
	echo Running: nice -n 19 ./compY -M -MC -t 20 -x 20 -o $offset
#	nice -n 19 ./compY -M -MC -t 20 -x 20 -o $offset | tee outputD${i}.txt
	echo Offset range $offset done
done

echo Double pass done

for i in {0..29}
do
        offset=$(($i*20))
        echo 
        echo Running: nice -n 19 ./compY -MC -t 20 -x 20 -o $offset
        nice -n 19 ./compY -MC -t 20 -x 20 -o $offset | tee outputS${i}.txt
        echo Offset range $offset done
done
