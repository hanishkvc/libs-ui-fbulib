#!/usr/bin/python
#
# bdf2cfile.py - A utility to generate a C header file corresponding
# to the given BDF font file. This is used by my menuk program for
# its font needs.
# v08Jan2012_1500
# HKVC, GPL, Jan2012
#

import sys

def find_string(fF, findString, offset):
	while True:
		l = fF.readline()
		if(l == ""):
			return "__EOF__"
		if(l.find(findString) != offset):
			continue
		return l


fF=open(sys.argv[1]);

state=0
newCharPos = 0
prevCharPos = 0

l=find_string(fF,"FONT",0);
l=l.strip()
print "char FK_FontName[] = \"{0}\";".format(l)
l=find_string(fF,"FONTBOUNDING",0);
l=l.strip()
l=l.split(' ')
print "int FK_FontXLen = {0};\nint FK_FontYLen = {1};\n".format(l[1],l[2])
sBitmapsArray = "int *FK_FontBitmapsArray[256] = { \n"
while True:
	l=find_string(fF,"STARTCHAR",0);
	if(l.find("__EOF__") == 0):
		break
	l=find_string(fF,"ENCODING",0);
	if(l.find("__EOF__") == 0):
		break
	newCharPos=int(l.split(' ')[1]);
	if(newCharPos > 255):
		break
	if (newCharPos > (prevCharPos+1)):
		for prevCharPos in range(prevCharPos+1, newCharPos):
			sBitmapsArray += "\tNULL,\n"
	#sBitmapsArray += "\t&FK_Bitmap_{0},\n".format(newCharPos)
	sBitmapsArray += "\tFK_Bitmap_{0},\n".format(newCharPos)
	print "int FK_Bitmap_{0}[] = {{".format(newCharPos)
	l = find_string(fF,"BITMAP",0);
	if(l.find("__EOF__") == 0):
		break
	while True:
		l=fF.readline()
		if(l == ""):
			break
		l=l.strip()
		if (l.find("ENDCHAR") != 0):
			print "\t0x{0},".format(l)
		else:
			print "\t};\n"
			break

	prevCharPos = newCharPos

sBitmapsArray +="};\n\n"
print sBitmapsArray

