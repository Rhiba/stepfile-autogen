import sys
import os
import math
import subprocess
import re
histogram = dict()
markov_num = 3
def get():
	path = "/var/tmp/dcs-get/stepmania-4eab6ff/stepmania-5.0/Songs/"
	os.chdir(path)
	#gets all things in current dir
	for pack in os.listdir(path):
		#only gets things that are directories
		if os.path.isdir(path + pack):
			newpath = path + pack + "/"
			for songdir in os.listdir(newpath):
				if os.path.isdir(newpath + songdir):
					finalpath = newpath + songdir + "/"
					for fileName in os.listdir(finalpath):
						if fileName.lower().endswith('.sm'):
							add_to_histogram(finalpath+fileName)
	print (str(markov_num) + "\n")
	for key,value in histogram.items():
		print(key + ":")
		total = 0.0
		for k,v in value.items():
			total += v
		for k,v in value.items():
			print(k + ": " + str(v/total))
		print(';\n')



def add_to_histogram(path):
	f = open(path,'r')
	reading = False
	currLine = ""
	notesLines = []
	readNotes = False
	while True:
		c = f.read(1)
		if not c:
			break
		if readNotes == True and c != ';':
			currLine += c
		elif readNotes == True and c == ';':
			currLine += c
			readNotes = False
			currLine = re.sub('//.*\r\n','',currLine)
			currLine = re.sub('//.*\n','',currLine)
			currLine = re.sub('\r','',currLine)
			currLine = re.sub('\n','',currLine)
			currLine = re.sub('\s+','',currLine)
			notesLines += [currLine]
			currLine = ""
		elif c == '#':
			currLine += c
		elif currLine == '#' and c == 'N':
			currLine += c
		elif currLine == '#' and c != 'N':
			currLine = ""
		elif currLine == '#N' and c == 'O':
			currLine += c
		elif currLine == '#N' and c != 'O':
			currLine = ""
		elif currLine == '#NO' and c == 'T':
			currLine += c
		elif currLine == '#NO' and c != 'T':
			currLine = ""
		elif currLine == '#NOT' and c == 'E':
			currLine += c
		elif currLine == '#NOT' and c != 'E':
			currLine = ""
		elif currLine == '#NOTE' and c == 'S':
			currLine += c
			readNotes = True
		elif currLine == '#NOTE' and c != 'S':
			currLine = ""
		elif currLine.startswith('#NOTES'):
			currLine = ""
	f.close()
	for notes in notesLines:
		notes = notes.split(':')[6][:-1]
		notes = [x for x in notes if x != ',']
		notes = ''.join(notes)
		for i in range(0,len(notes) - (markov_num*4) - 4,4):
			chunk = notes[i:i+(markov_num*4)]
			follower = notes[i+(markov_num*4):i+(markov_num*4)+4]
			if chunk in histogram.keys():
				if follower in histogram[chunk].keys():
					histogram[chunk][follower] += 1
				else:
					histogram[chunk][follower] = 1
			else:
				histogram[chunk] = dict()
				histogram[chunk][follower] = 1


if __name__ == "__main__":
	get()
