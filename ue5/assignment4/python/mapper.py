#!/usr/bin/python

import sys
data = sys.stdin.read()
for line in data.split():
	s = line.split()
	for item in s:
		sys.stdout.write(item+"\t1\n")

