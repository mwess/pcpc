#!/bin/bash
#jobstring="qsub -v path=$inputpath,$width,$height,nodes=$nodes,count=$i runjob.sh"
input=${path}
#echo $input
width="Width=${Width}"
width1=${width##Width=}
#echo $width1
height="Height=${Height}"
height1=${height##Height=}
#echo $height1
nodes=${nodes}
nodes1=${nodes##nodes=}
#echo $nodes
countNode=${count}
countNode1=${countNode##count=}
#echo $countNode
# Datei anlegen
touch $countNode1.tga
range=$((height1/nodes))
#echo "Runjobs: range is $range"
nodes1=$(($nodes1-1))
#echo "Aktueller: $countNode1, Anzahl: $nodes1"
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
#/usr/local/bin/povray $2 $3 $4 SR$start ER$end
/usr/local/bin/povray +I$input $width $height +SR$start +ER$end +SC1 +SC$width1 +FT +O$countNode1.tga
