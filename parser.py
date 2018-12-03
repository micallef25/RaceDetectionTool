#!/usr/bin/python
import re
import SimpleHTTPServer
import SocketServer
PORT = 8000
# a bad function for parsing a file and looking for effected threads from the the tool
# looks for the start token when we hit the line with start we begin storing every line until we see the end token
def create_list(file, List):
	begin = 0 
	for line in file:
		startObj = re.findall("START_PIN_LIST",line,re.M|re.I)
		if(begin == 1):
			matchObj = re.findall("END_PIN_LIST",line,re.M|re.I)
			if matchObj:
				break;
			# strip the line as the map file does not end with a new line character
			List.append(line.strip("\n"))
		if startObj:
			begin = 1

			
# function for parsing the data
# arg text is the line of text to analyze
# arg addr is the address we are looking for
# List is a list to append the text to if we find a match
def data_parser(text,addr,List):
	
	# search for our addr in line if found strip WS and store
	matchObj = re.findall(addr, text, re.M|re.I)
	if matchObj:
		#remove leading and lagging newlines and spaces check to make sure first byte is a 0 from address
		# if it is then store it
		text = text.strip()
		if(text[0] == "0"):
			#strip the address and then strip the new lines so that what is stored is just the variable
			text = text[18:]
			text = text.strip()
			# print text
			#replace all spaces now we should have a compact unit now
			List.append(text)

# gets the high and low addresses from the main executable as told by pin
def get_high_address(file,List):
	begin = 0 
	for line in file:
		startObj = re.findall("PIN_LOW_ADDR",line,re.M|re.I)
		#print line
		if startObj:
			List.append(line.strip())
			break;

	for line in file:
		startObj = re.findall("PIN_HIGH_ADDR",line,re.M|re.I)
		if startObj:
			List.append(line.strip())
			break;

def get_function(high_addr,low_addr,file,List):
	# get the address ranges
	print  int(high_addr,base=16)
	print int(low_addr,base=16)
	for line in file:
		line = line.strip()
		if (line != "" and line[0] == "0"):
			try: 
				var = int(line[:18],base=16)
				if (var < int(high_addr,base=16) and var > int(low_addr,base=16)):
					List.append(var)
			except ValueError:
				print "error "
			# if(int(line[18:],base=16) < int(high_addr,base=16) and int(line[18:],base=16) > int(low_addr,base=16)):
				# print line


#def fnc_parse(text,high,low):
def get_addrs(f_name):
	#file_name = "naiverace"
	file_name = f_name
	path = "race_tests/"
	inputfile = file_name
	inputfile = path + file_name + ".map"
	inputfile = open(inputfile)
	mapfile = open('out')
	templist = []
	sortedlist = []
	high_addr = ""
	low_addr = ""

	get_high_address(mapfile,templist)

	low_addr = templist[0]
	high_addr = templist[1]
	high_addr = high_addr[14:]
	low_addr = low_addr[13:]

	get_function(high_addr,low_addr,inputfile,sortedlist)
	
	sortedlist.sort()
	print sortedlist	
	inputfile.close()
	mapfile.close()
	return templist

def parse_map(f_name):
	file_name = f_name
	#file_name = "naiverace"
	path = "race_tests/"
	inputfile = file_name
	inputfile = path + file_name + ".map"
	inputfile = open(inputfile)
	# inputfile = open('race_tests/unsaferace.map')
	mapfile = open('out')
	#outputfile = open('test.map', 'w')
	addrlist = []
	varlist = []
	# from the addresses in the out file we create our list of addresses to search for
	create_list(mapfile,addrlist)

	#for i in range(0): inputfile.next() # skip first four lines
	for line in inputfile:
		for l in addrlist: 
    			data_parser(line,l,varlist)

	#for var in varlist: 
	#	print var

	
	inputfile.close()
	mapfile.close()
	return varlist

#if __name__ == "__main__":
#	main()