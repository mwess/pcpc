#!/bin/bash

#Aufruf: ./renderImage.sh /usr/local/povray/share/povray-3.6/scenes/objects/fractal1.pov Width=20 Height=20 
distributeJobs(){
	#jobstring="qsub -v "
	for((j = 0; j < counter; j++))
	do
		for((i = 0; i < nodesPerPic; i++))
		do
			jobstring="qsub -v path=${inputpath[j]},Width=${width[j]},Height=${height[j]},nodes=$nodesPerPic,count=$i,pic=$j runjob.sh"
			#echo "$jobstring"
			$jobstring
			echo "Job $i submitted."
		done
	done
}

merge(){
	#Warten, bis alles fertig
	# erste 18 byte header, rest ist bild
	echo "All jobs are done."

	timer=0
	for((j = 0; j < counter; j++))
	do
		for((i = 0; i < nodesPerPic; i++))
		do
			timer=0
			running="`qstat -u pbsuser1`"
			while [ "$running" != "" ]
			do
				echo "Waiting for Jobs."
				sleep 1
				running=`(qstat -u pbsuser1)`
			done
            		while [ ! -e "$j"_"$i"_control ]
            		do
			    echo "while,$timer,$j,$i"
			    jobstring="qsub -v path=${inputpath[j]},Width=${width[j]},Height=${height[j]},nodes=$nodesPerPic,count=$i,pic=$j runjob.sh"
		    	    if ((timer > 30))  
			    then
				echo $jobstring
				$jobstring
				timer=0
				continue
			    fi
			    sleep 2
			    ((timer+=2))
            		done
			echo "$timer,$j,$i"
		done
	done

	`chmod +r *.tga`
	rm *_output 
	for((i = 0; i < counter; i++))
	do
		touch "$i"_output
	done	

	#Schleifen zum umspeichern
	for((i=0; i<counter;i++))
	do
		`cat "$i"_0.tga >> "$i"_output`
		for((j=1; j<nodesPerPic; j++))
		do
			`tail -c +19 "$i"_"$j".tga >> "$i"_output`
		done
	done
#	`cat 0.tga >> output`
#	for((i=1; i<nodes;i++))
#	do
#		`tail -c +19 $i.tga >> output`
#	done	
}

cleanup(){
	rm runjob.sh.*
	rm *.tga
	rm *control
}

#Aufruf: ./renderImage.sh /usr/local/povray/share/povray-3.6/scenes/objects/fractal1.pov Width=20 Height=20 
#./renderImage.sh /usr/local/povray/share/povray-3.6/scenes/objects/fractal1.pov Width=20 Height=20 /usr/local/povray/share/povray-3.6/scenes/advanced/skyvase.pov Width=2- Height=20

counter=0
declare -a inputpath
declare -a width
declare -a width1
declare -a height
declare -a height

for i in $@
do
	a=$((counter/3))
	if [ $(( $counter % 3 )) -eq 0 ]
	then
		inputpath[$a]=$i
	#	inputpath_$counter=${inputpath##I}
	fi
	if [ $(( $counter % 3 )) -eq 1 ]
	then
		width[$a]=$i
		width1[$a]=${width##Width=}
	#	width_$counter=${width##Width=}
	fi
	if [ $(( $counter % 3 )) -eq 2 ]
	then
		height[$a]=$i
		height1[$a]=${height##Height=}
	#	height_$counter=${height##Height=}
	fi
	(( counter++ ))
done

#Speicherueberpruefung
neededSpace=18
space=`free -b | grep Mem | awk '{ print $4 }'`
for((j = 0; j < counter; j++))
do
	a=${width1[j]}
	b=${height1[j]}
	#neededSpace=$(($neededSpace+(${width1[j]}*${height1[j]}*3)))
	neededSpace=$(($neededSpace+(a*b*3)))
done

echo $neededSpace
echo $space

if((neededSpace > space ))
then
	exit
fi

#echo ${inputpath[1]}

counter=$((counter/3))
nodes=`pbsnodes -l free | wc -l`
#nodes=1
nodesPerPic=$(( nodes/counter ))

echo "Nodes: $nodes"
echo "Nodes Per Pic: $nodesPerPic"

distributeJobs
merge
cleanup
