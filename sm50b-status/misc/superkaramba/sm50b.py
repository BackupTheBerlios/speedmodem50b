# Script by Ivica Ico Bukvic (SlipStreamScapes)
# http://meowing.ccm.uc.edu/~ico/
# Distributed under the GPL license
# Version 1.0 (July 2004)

import karamba
import os

# width of the desktop, width of the opened bar, bar height, and padding of the retracted menu
desktop = 1280
width = 144
padding = 36
height = 160

vector = 0
retracted = 0
this = "sm50b"

def writeConfigEntryNow(widget, string, replace):
	filename = os.path.expanduser("~") + "/.superkaramba/" + this + ".rc"
	file = open( filename, "r")
	output = file.read()
	file.close()
	output = output.splitlines()
	
	i = 0
	desktop = ""
	for i in output:
		param = i.split()
		if param and param[0].find(string) != -1:
			desktop = desktop + string + "=" + replace + "\n"
		else:
			desktop = desktop + i +"\n"
	
	file = open( filename, "w")
	file.write(desktop)
	file.close

###############################################################################
## Function:    
##   detectResolution ( )
## 
## 
## Description: 
##   Runs xwininfo and sets the current XFree86 resolution to the golbal
##   variables resolutionX and resolutionY
## 
##  Thanks to Adam Geitgey for this code!
###############################################################################
def detectResolution():
	global desktop
	
	havexwininfo = os.system("which xwininfo >/dev/null")
	if (havexwininfo == 0):
		pass
	
	fp = os.popen("xwininfo -root -stats")
	output = fp.read()
	output = output.splitlines()
	
	i = 0
	for i in output:
		param = i.split()
		if (len(param) > 1):
			if param[0].find("Width:") != -1:
				desktop = int(param[1])

##########################################################
# Superkaramba API functions                             #
##########################################################
#this is called when your widget is initialized
def initWidget(widget):
	global desktop
	global padding
	global retracted

	detectResolution()

	retracted = karamba.readConfigEntry (widget, "retracted")
	x = karamba.readConfigEntry (widget, "widgetPosX")
	y = karamba.readConfigEntry (widget, "widgetPosY")
	
	if ( desktop - x != width ):
		karamba.moveWidget(widget, desktop - width, y)
		writeConfigEntryNow(widget, "widgetPosX", str(x))
		writeConfigEntryNow(widget, "widgetPosY", str(y))
	if ( retracted == 1 ):
		karamba.translateAll(widget,width - padding,0)
		karamba.redrawWidget(widget)
		karamba.toggleWidgetRedraw(widget, 0)

#This gets called everytime our widget is clicked.
#Notes:
#  widget = reference to our widget
#  x = x position (relative to our widget)
#  y = y position (relative to our widget)
#  botton = button clicked:
#                    1 = Left Mouse Button
#                    2 = Middle Mouse Button
#                    3 = Right Mouse Button, but this will never happen
#                        because the right mouse button brings up the
#                        Karamba menu.
#                    4,5 = Scroll wheel up and down
def widgetClicked(widget, x, y, button):
	global desktop
	global padding
	global height
	global width
	global retracted

	if (button == 1):
		if (retracted == 1) and (x >= width - padding) or (retracted == 0):
			if ( retracted == 1 ):
				retracted = 0
				karamba.toggleWidgetRedraw( widget, 1 )
				os.popen("artsplay $HOME/Karamba/Borealis/sounds/slide_out_rev2.wav")
				for i in range( width - padding ):
					karamba.translateAll(widget,-1,0)
					karamba.redrawWidget( widget )
			else:
				os.popen("artsplay $HOME/Karamba/Borealis/sounds/slide_in_rev2.wav")
				for i in range( width - padding ):
					karamba.translateAll(widget,1,0)
					karamba.redrawWidget( widget )
				retracted = 1
				karamba.toggleWidgetRedraw( widget, 0 )
			writeConfigEntryNow(widget, "retracted", str(retracted))
