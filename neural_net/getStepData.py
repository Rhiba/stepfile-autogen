from __future__ import print_function
import sys
import os
import math
import subprocess
import re
data_arr = []
#diff, steps/length, jumps/length, holds/length, rolls/length, mines/length, max notes per measure, number of bpms, max bpm, min bpm, bpm diff, length
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
					got_both = 0
					length = 0.0
					notestats = []
					sp = ""
					for fileName in os.listdir(finalpath):
						if fileName.lower().endswith('.sm'):
							#finally got to an sm file! now to do processing on that file.
							notestats = getNotes(finalpath + fileName)
							sp = finalpath + getSongPath(finalpath + fileName)
							length = getLength(sp)
							#print sp
					if sp == "":
						pass
					else:
						#length becomes the last thing in the list
						if length > 0:
							for arr in notestats:
								arr.append(length)
							data_arr.extend(notestats)
	#print data_arr
	min_diff = 10000
	max_diff = 0
	min_steps = 10000
	max_steps = 0
	min_jumps = 10000
	max_jumps = 0
	min_holds = 10000
	max_holds = 0
	min_rolls = 10000
	max_rolls = 0
	min_mines = 10000
	max_mines = 0
	min_npm = 10000
	max_npm = 0
	min_num_bpm = 10000
	max_num_bpm = 0
	min_max_bpm = 10000
	max_max_bpm = 0
	min_min_bpm = 10000
	max_min_bpm = 0
	min_diff_bpm = 10000
	max_diff_bpm = 0
	min_len = 10000
	max_len = 0
	mean = 0
	not_inc = 0
	data_arr_2 = [i for i in data_arr if int(i[0]) <= 20]
	for song in data_arr_2:
		if int(song[0]) <= 20:
			if int(song[0]) > max_diff:
				max_diff = int(song[0])
			if int(song[0]) < min_diff:
				min_diff = int(song[0])
			if float(song[1])/float(song[-1]) > max_steps:
				max_steps = float(song[1])/float(song[-1])
			if float(song[1])/float(song[-1]) < min_steps:
				min_steps = float(song[1])/float(song[-1])
			if float(song[2])/float(song[-1]) > max_jumps:
				max_jumps = float(song[2])/float(song[-1])
			if float(song[2])/float(song[-1]) < min_jumps:
				min_jumps = float(song[2])/float(song[-1])
			if float(song[3])/float(song[-1]) > max_holds:
				max_holds = float(song[3])/float(song[-1])
			if float(song[3])/float(song[-1]) < min_holds:
				min_holds = float(song[3])/float(song[-1])
			if float(song[4])/float(song[-1]) > max_rolls:
				max_rolls = float(song[4])/float(song[-1])
			if float(song[4])/float(song[-1]) < min_rolls:
				min_rolls = float(song[4])/float(song[-1])
			if float(song[5])/float(song[-1]) > max_mines:
				max_mines = float(song[5])/float(song[-1])
			if float(song[5])/float(song[-1]) < min_mines:
				min_mines = float(song[5])/float(song[-1])
			if int(song[6]) > max_npm:
				max_npm = int(song[6])
			if int(song[6]) < min_npm:
				min_npm = int(song[6])
			if int(song[7]) > max_num_bpm:
				max_num_bpm = int(song[7])
			if int(song[7]) < min_num_bpm:
				min_num_bpm = int(song[7])
			if int(song[8]) > max_max_bpm:
				max_max_bpm = int(song[8])
			if int(song[8]) < min_max_bpm:
				min_max_bpm = int(song[8])
			if int(song[9]) > max_min_bpm:
				max_min_bpm = int(song[9])
			if int(song[9]) < min_min_bpm:
				min_min_bpm = int(song[9])
			if int(song[10]) > max_diff_bpm:
				max_diff_bpm = int(song[10])
			if int(song[10]) < min_diff_bpm:
				min_diff_bpm = int(song[10])
			if int(song[11]) > max_len:
				max_len = int(song[11])
			if int(song[11]) < min_len:
				min_len = int(song[11])

			mean += float(song[0])
		else:
			not_inc += 1

	print("Max: " + str(max_diff), file=sys.stderr)
	mean /= len(data_arr_2) - not_inc
	mean_square = 0
	for song in data_arr_2:
		mean_square += (float(song[0]) - mean)**2

	mean_square /= len(data_arr_2)
	std_dev = math.sqrt(mean_square)
	#i have the mean and standard dev but i dont need them right now unless i want to standardise

	for song in data_arr_2:
		if int(song[0]) <= 20:
			song[0] = (float(song[0]) - min_diff) / (max_diff - min_diff)
			song[1] = ((float(song[1])/float(song[-1]))-min_steps)/(max_steps - min_steps)
			song[2] = ((float(song[2])/float(song[-1]))-min_jumps)/(max_jumps - min_jumps)
			song[3] = ((float(song[3])/float(song[-1]))-min_holds)/(max_holds - min_holds)
			song[4] = ((float(song[4])/float(song[-1]))-min_rolls)/(max_rolls - min_rolls)
			song[5] = ((float(song[5])/float(song[-1]))-min_mines)/(max_mines - min_mines)
			song[6] = (float(song[6]) - min_npm) / (max_npm - min_npm)
			song[7] = (float(song[7]) - min_num_bpm) / (max_num_bpm - min_num_bpm)
			song[8] = (float(song[8]) - min_max_bpm) / (max_max_bpm - min_max_bpm)
			song[9] = (float(song[9]) - min_min_bpm) / (max_min_bpm - min_min_bpm)
			song[10] = (float(song[10]) - min_diff_bpm) / (max_diff_bpm - min_diff_bpm)
			song[11] = (float(song[11]) - min_len) / (max_len - min_len)
	output_to_file(data_arr_2,"songdata")

def getSongPath(path):
	f = open(path,'r')
	regex = re.search('(?<=#MUSIC:)(?P<path>[^;]+);',f.read())
	final = regex.group('path')
	return final

def output_to_file(data,filename):
	for song in data:
		s = ""
		for val in song:
			s += str(val) + " "
		if float(song[0]) == 0.0:
			pass
		else:
			print(s[:-1])

def getLength(filepath):
	p1 = subprocess.Popen(['ffmpeg','-i',filepath],stderr=subprocess.PIPE)
	p1.wait()
	string = str(p1.communicate())
	regex = re.search('(?<=Duration: )(?P<dur>[0-9:\.]+),.*',string)
	if regex:
		num = regex.group('dur')
		arr = num.split(':')
		secs = float(arr[0]) * 3600 + float(arr[1]) * 60 + float(arr[2])
		return secs
	else:
		return 0

def getNotes(filepath):
	#print filepath
	f = open(filepath,'r')
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
	f = open(filepath,'r')
	readBPMS = False
	bpmLines = []
	while True:
		c = f.read(1)
		if not c:
			break
		if readBPMS == True and c != ';':
			currLine += c
		elif readBPMS == True and c == ';':
			currLine += c
			readBPMS = False
			currLine = re.sub('//.*\r\n','',currLine)
			currLine = re.sub('//.*\n','',currLine)
			currLine = re.sub('\r','',currLine)
			currLine = re.sub('\n','',currLine)
			currLine = re.sub('\s+','',currLine)
			currLine = re.sub(':[0-9\.]+=',':',currLine)
			currLine = re.sub(',[0-9\.]+=',',',currLine)
			bpmLines += [currLine]
			currLine = ""
		elif c == '#':
			currLine += c
		elif currLine == '#' and c == 'B':
			currLine += c
		elif currLine == '#' and c != 'B':
			currLine = ""
		elif currLine == '#B' and c == 'P':
			currLine += c
		elif currLine == '#B' and c != 'P':
			currLine = ""
		elif currLine == '#BP' and c == 'M':
			currLine += c
		elif currLine == '#BP' and c != 'M':
			currLine = ""
		elif currLine == '#BPM' and c == 'S':
			currLine += c
			readBPMS = True
		elif currLine == '#BPM' and c != 'S':
			currLine = ""
		elif currLine.startswith('#BPMS'):
			currLine = ""
	diff = 0
	jumps = 0
	steps = 0
	holds = 0
	rolls = 0
	mines = 0
	max_npm = 0 #notes per measure max
	ret_arr = []
	for x in notesLines:
		curr_arr = []
		diff = 0
		jumps = 0
		steps = 0
		holds = 0
		rolls = 0
		mines = 0
		max_npm = 0 #notes per measure max
		diff = x.split(':')[4] #difficulty
		d = x.split(':')[6]
		for n in d.split(','):
			if len(n)/4 > max_npm:
				max_npm = len(n)/4

			chunks=[n[x:x+4] for x in xrange(0, len(n), 4)]
			for c in chunks:
				mines += c.count('M') + c.count('m')
				if c.count('1') > 1:
					jumps += 1
				if c.count('1') == 1:
					steps += 1
				holds += c.count('2')
				rolls += c.count('4')
	
		#print("\nDATA\n====\n")
		#print("Difficulty: " + str(diff))
		#print("Steps: " + str(steps))
		#print("Jumps: " + str(jumps))
		#print("Holds: " + str(holds))
		#print("Rolls: " + str(rolls))
		#print("Mines: " + str(mines))
		#print("Max notes per measure: " +str(max_npm))
		curr_arr.append(diff)
		curr_arr.append(steps)
		curr_arr.append(jumps)
		curr_arr.append(holds)
		curr_arr.append(rolls)
		curr_arr.append(mines)
		curr_arr.append(max_npm)

		for x in bpmLines:
			#print ("BPM: " + x.split(':')[1])
			curr_arr.append(len(x.split(':')[1][:-1].split(',')))
			curr_arr.append(max([float(i) for i in x.split(':')[1][:-1].split(',')]))
			curr_arr.append(min([float(i) for i in x.split(':')[1][:-1].split(',')]))
			curr_arr.append(max([float(i) for i in x.split(':')[1][:-1].split(',')]) - min([float(i) for i in x.split(':')[1][:-1].split(',')]))

		ret_arr.append(curr_arr)
	return ret_arr

if __name__ == "__main__":
	get()
