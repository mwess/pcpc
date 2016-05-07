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

distributeJobs2(){
    #forward[$i]="${pictures[$i]}"
    #backward["${pictures[$i]}"]=$i
    #echo "${forward[$i]}"
    #echo "${backward[${pictures[$i]}]}"
    #echo "$nPictures"
    #echo "$nodesPerPicture"
        for((i = 0; i < nPictures; i++))
        do
            for((j = 0; j < nodesPerPicture; j++))
            do
                jobstring="qsub -v path=${forward[$i]},$width,$height,nodes=$nodesPerPicture,count=$j,output=$i runjob.sh"
                echo "$jobstring"
                #echo "Job $j of picture $i submitted."
            done
        done
	##jobstring="qsub -v "
	#for((i = 0; i < nodes; i++))
	#do
	#	jobstring="qsub -v path=$inputpath,$width,$height,nodes=$nodes,count=$i runjob.sh"
	#	#echo "$jobstring"
	#	$jobstring
	#	echo "Job $i submitted."
	#done
}

distributeJobs3(){
    #forward[$i]="${pictures[$i]}"
    #backward["${pictures[$i]}"]=$i
    #echo "${forward[$i]}"
    #echo "${backward[${pictures[$i]}]}"
    #echo "$nPictures"
    #echo "$nodesPerPicture"
        for((i = 0; i < nPictures; i++))
        do
            for((j = 0; j < nodesPerPicture; j++))
            do
                jobstring="qsub -v path=${forward[$i]},$width,$height,nodes=$nodesPerPicture,count=$j,output=$i runjob.sh"
                echo "$jobstring"
                #echo "Job $j of picture $i submitted."
            done
        done
	##jobstring="qsub -v "
	#for((i = 0; i < nodes; i++))
	#do
	#	jobstring="qsub -v path=$inputpath,$width,$height,nodes=$nodes,count=$i runjob.sh"
	#	#echo "$jobstring"
	#	$jobstring
	#	echo "Job $i submitted."
	#done
}

merge2(){
	#Warten, bis alles fertig
	running="`qstat -u pbsuser1`"
	while [ "$running" != "" ]
	do
		echo "Waiting for Jobs."
		sleep 1
		running=`(qstat -u pbsuser1)`
	done
	# erste 18 byte header, rest ist bild
	echo "All jobs are done."

	`chmod +r *.tga`

        for((i = 0; i < nPictures; i++))
        do
            rm $i
            touch $i
            `cat $i_0.tga >> $i.tga`
            rm $i_0.tga
        done
        for((i = 0; i < nPictures; i++))
        do
            for((j = 1; j < nodesPerPicture; j++))
            do
                `tail -c +19 $i_$j.tga >> $i.tga`
                rm $i_$j.tga
            done
        done
        for((i=0; i < nPictures; i++))
        do
            mv $i.tga ${forward[$i]}.tga
        done
	#rm output
	#touch output
	#
	#`cat 0.tga >> output`
	#for((i=1; i<nodes;i++))
	#do
	#	`tail -c +19 $i.tga >> output`
	#done	
}

merge(){
	#Warten, bis alles fertig
	running="`qstat -u pbsuser1`"
	while [ "$running" != "" ]
	do
		echo "Waiting for Jobs."
		sleep 1
		running=`(qstat -u pbsuser1)`
	done
	# erste 18 byte header, rest ist bild
	echo "All jobs are done."

	`chmod +r *.tga`
	rm output
	touch output
	
	`cat 0.tga >> output`
	for((i=1; i<nodes;i++))
	do
		`tail -c +19 $i.tga >> output`
	done	
}

cleanup(){
	rm runjob.sh.*
	rm *[0-9].tga
}

#Aufruf: ./renderImage.sh /usr/local/povray/share/povray-3.6/scenes/objects/fractal1.pov Width=20 Height=20 

#1.read in arguments and save all pictures in an array
#function readparams(){
declare -a pictures
while [[ $# > 1 ]]
do
key="$1"
case $key in
    -w|--width=*)
    width="$2"
    shift
    ;;
    -h|--height=*)
    height="$2"
    shift
    ;;
    #--default)
    #pictures=("${pictures[@]}" $1)
    #shift
    #;;
    *)
    pictures=("${pictures[@]}" $1)
    #echo $key
    ;;
esac
shift
done
#}
#2.create dictionaries for input file names
declare -A forward
declare -A backward
nPictures=${#pictures[@]}
for((i = 0; i < nPictures; i++))
do
    forward[$i]="${pictures[$i]}"
    backward["${pictures[$i]}"]=$i
    #echo "${forward[$i]}"
    #echo "${backward[${pictures[$i]}]}"
done

#nodes=`pbsnodes -l free | wc -l`
nodes=40

#compute how many node each picture gets
nodesPerPicture=$((nodes/nPictures))

#distributeJobs2

#3.save job commands in an array
declare -a jobarray
for((i = 0; i < nPictures; i++))
do
    for((j = 0; j < nodesPerPicture; j++))
    do
        jobstring="qsub -v path=${forward[$i]},$width,$height,nodes=$nodesPerPicture,count=$j,output=$i_$j.tga runjob.sh"
        jobarray=("${jobarray[@]}" $jobstring)
    pictures=("${pictures[@]}" $1)
        echo "$jobstring"
        #echo "Job $j of picture $i submitted."
    done
done

#4.Error handling. Check if output file has been created, else execute command again
while [ ${#jobarray[@]} -eq 0 ]
do
    for((i = 0; i < nPictures; i++))
    do
        for((j = 0; j < nodesPerPicture; j++))
        do
            if [ ! -e $i_$j.tga ]
            then
                ind=$((i*nodesPerPicture+j))
                $jobstring[$ind]
            fi
        done
    done
    sleep 5
done

#5.Merge step
`chmod +r *.tga`
rm output
touch output

echo "All jobs are done."

`chmod +r *.tga`

for((i = 0; i < nPictures; i++))
do
    rm $i
    touch $i
    `cat $i_0.tga >> $i.tga`
    rm $i_0.tga
done
for((i = 0; i < nPictures; i++))
do
    for((j = 1; j < nodesPerPicture; j++))
    do
        `tail -c +19 $i_$j.tga >> $i.tga`
        rm $i_$j.tga
    done
done
for((i=0; i < nPictures; i++))
do
    mv $i.tga ${forward[$i]}.tga
done

cleanup







#inputpath=$1
#inputpath1=${inputpath##I}
#width=$2
#width1=${width##Width=}
#echo "Width $width1"
#height=$3
#height1=${height##Height=}
#outputpath=$4
#outputpath1=${outputpath##O}

#echo "Nodes: $nodes"

#distributeJobs
#merge

#cleanup
#cleanup

#readparams
#echo "$width"
#echo "$height"
#echo ${pictures[@]}
