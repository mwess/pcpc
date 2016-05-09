#!/bin/bash
#jobstring="qsub -v path=$inputpath,$width,$height,nodes=$nodesPerPic,count=$i,pic=$j runjob.sh"
echo $PWD
touch abc
input=${path}
width=${Width}
width1=${width##Width=}
height=${Height}
height1=${height##Height=}
nodes=${nodes}
nodes1=${nodes##nodes=}
countNode=${count}
countNode1=${countNode##count=}
pic=${pic}
pic1=${pic##pic=}
# Datei anlegen
tmp="$pic"_"$countNode1"
touch /var/local/torque/pbsuser1/exercise2copy/exercise2/$tmp.tga
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
/usr/local/bin/povray +I$input $width $height +SR$start +ER$end +FT +O/var/local/torque/pbsuser1/exercise2copy/exercise2/$tmp.tga
echo "/usr/local/bin/povray +I$input Width=$width Height=$height +SR$start +ER$end +FT +O/var/local/torque/pbsuser1/exercise2copy/exercise2/$tmp.tga"
touch /var/local/torque/pbsuser1/exercise2copy/exercise2/"$tmp"_control
echo "fertig"
