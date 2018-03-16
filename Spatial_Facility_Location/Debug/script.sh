#! /bin/bash
stop[0]=10000
stop[1]=50000
stop[2]=100000
stop[3]=200000
stop[4]=500000
stop[5]=1000000
stop[6]=2000000
stop[7]=5000000

h[0]="e"
h[1]="m"
h[2]="d"
h[3]="b"
h[4]="a"
h[5]="x"

for i in 0 1 2 3 4 5 6 7 
do
	for j in 0 1 2 3 4 5
	do 
		#echo ${stop[$i]}
		#stop=$(($base * $i)) 
		./distance RD_biomass.tif RD_fricc.tif T ${stop[$i]} ${h[$j]} 
		#echo "$stop"
		#echo "$base"
	done
done
