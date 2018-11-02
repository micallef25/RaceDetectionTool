#!/usr/bin/python
import re

# a bad function for parsinga file and looking for effected threads from the the tool
def create_list(file, List):
	begin = 0 
	for line in file:
		startObj = re.findall("START",line,re.M|re.I)
		if(begin == 1):
			matchObj = re.findall("END",line,re.M|re.I)
			if matchObj:
				break;
			# strip the line as the map file does not end with a new line character
			List.append(line.strip("\n"))
		if startObj:
			begin = 1
			
# function for parsing the data
def data_parser(text,addr):
	# print("line " + addr)
	# re.findall(r'0x[0-9A-F]+', your_string, re.I)
	matchObj = re.findall(addr, text, re.M|re.I)
	if matchObj:
		print("found "+ text)

def main():
	inputfile = open('race_tests/saferace.map')
	mapfile = open('out')
	outputfile = open('test.map', 'w')
	addrlist = []
	# from the addresses in the out file we create our lit
	create_list(mapfile,addrlist)

	#for i in range(0): inputfile.next() # skip first four lines
	for line in inputfile:
		for l in addrlist: 
    			data_parser(line,l)

	inputfile.close()
	outputfile.close()

main()