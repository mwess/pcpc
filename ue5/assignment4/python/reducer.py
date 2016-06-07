#!/usr/bin/python
import sys

word = ""
count = 0


data = sys.stdin.read().split("\n")
for line in data:
	if len(line) == 0:
		sys.stdout.write(word+"\t"+str(count)+"\n")
		continue
	spl = line.split("\t")
	newword = spl[0]
	if newword == word:
		count += 1
	else:
		if count > 0:
			sys.stdout.write(word+"\t"+str(count)+"\n")
		word = newword
		count = 1


#newword = 
#
#summ = 0
#word = ""
#data = sys.stdin.read("\t")#.split("\n")
#for line in data:
#	spl = line.split("\t")
#	prefix = spl[0]
#	i = spl[1]
#	word = prefix
#	summ += int(i)
#sys.stdout.write(word+"\t"+str(summ))
#
