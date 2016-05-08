#!/bin/bash
#jobstring="qsub -v path=${forward[$i]},$width,$height,nodes=$nodesPerPicture,count=$j,output=$i runjob.sh
#jobstring="qsub -v path=$inputpath,$width,$height,nodes=$nodes,count=$i runjob.sh"
input=${path}
width="Width=${Width}"
width1=${width##Width=}
height="Height=${Height}"
height1=${height##Height=}
nodes=${nodes}
nodes1=${nodes##nodes=}
countNode=${count}
countNode1=${countNode##count=}
#outputFile=${output}
#output=${outputFile##output=}
output=${output}
# Datei anlegen
touch $output.tga
range=$((height1/nodes))
nodes1=$(($nodes1-1))
if [ "$countNode1" -ne "$nodes1" ]
   then
   end=$((($countNode1+1)*$range))
   end=$(($end-1))
else
   end=$height1
fi 
start=$(($countNode1*$range))
echo "Start: $start end: $end"
# $4, $5, $6: Inputpath, Height, Width
/usr/local/bin/povray +I$input $width $height +SR$start +ER$end +FT +O$output.tga
#create outputcontrol file to check whether job ran completely
touch $output.control
