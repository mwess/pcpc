#!/bin/bash

#Aufruf: ./renderImage.sh /usr/local/povray/share/povray-3.6/scenes/objects/fractal1.pov Width=20 Height=20 
distributeJobs(){
	#jobstring="qsub -v "
	for((i = 0; i < nodes; i++))
	do
		jobstring="qsub -v path=$inputpath,$width,$height,nodes=$nodes,count=$i runjob.sh"
		#echo "$jobstring"
		$jobstring
		echo "Job $i submitted."
	done
}

merge(){
	#Warten, bis alles fertig
	running="`qstat -u pbsuser1`"
#	echo $running
	while [ "$running" != "" ]
	do
		echo "Waiting for Jobs."
		sleep 1
		running=`(qstat -u pbsuser1)`
	done
	# erste 18 byte header, rest ist bild
	echo "All jobs are done."

	`chmod +r *.tga`
	touch output.tga
	
	`cat 1.tga >> output.tga`
	for((i=1; i<nodes;i++))
	do
		`tail -c +19 $i.tga >> output.tga`
	done	
}

cleanup(){
	rm runjob.sh.*
	rm *[0-9].tga
}

#Aufruf: ./renderImage.sh /usr/local/povray/share/povray-3.6/scenes/objects/fractal1.pov Width=20 Height=20 

inputpath=$1
inputpath1=${inputpath##I}
width=$2
width1=${width##Width=}
echo "Width $width1"
height=$3
height1=${height##Height=}
#outputpath=$4
#outputpath1=${outputpath##O}

nodes=`pbsnodes -l free | wc -l`
echo "Nodes: $nodes"

distributeJobs
merge
#cleanup
