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
#TODO: check other jobs while waiting for one that didnt work correctly
time = 0
while [ $index -eq 0 ]
do
    $index=1
    for((i = 0; i < nPictures; i++))
    do
        for((j = 0; j < nodesPerPicture; j++))
        do
            $time = 0
            while [ ! -e $i_$j.control ]
            do
                sleep 2
                $time += 2
                #if waiting lasted for over 20 seconds -> submit job again
                if [ time > 20 ]
                then
                    $index=0
                    ind=$((i*nodesPerPicture+j))
                    $jobstring[$ind]
                done
                fi
            done
        done
    done
    sleep 5
done

#5.Merge step
`chmod +r *.tga`

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

#6.Clean up
rm runjob.sh.*
rm *[0-9].tga
