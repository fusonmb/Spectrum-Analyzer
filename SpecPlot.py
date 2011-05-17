# spectrumPlot.py
#       Plots spectrum data recieved from stdin
#       by Jay Dial, 5/14/2011

from zellegraphics import *
from string import *
import sys, time

lines = []
points = []
BANDS = 100
winWidth = 800 
winHeight = 500
maxFreq = 16000
minDb = -75

def main():
	win = GraphWin("Spectrum Analyzer", winWidth, winHeight)
	win2 = GraphWin("Cursers", 500, 150)
	win.setBackground('white')
	win2.setBackground('white')
	counter = 0;
	cursor = 0
	delta = 0

	# Vert grid lines
	vertLines = winWidth/100
	for i in range(vertLines):
		l = Line(Point(i*winWidth/vertLines, 0), Point(i*winWidth/vertLines,winHeight))
		l.setFill('gray')
		l.setWidth(.5)
		l.draw(win)
	
	# Horz lines
	horzLines = winHeight/100
	for i in range(horzLines):
		l = Line(Point(0, i*winHeight/horzLines), Point(winWidth, i*winHeight/horzLines))
		l.setFill('gray')
		l.setWidth(.5)
		l.draw(win)
	
	# "Dotted" Line segments
	for i in range(47):
		l1 = Line(Point(i*winWidth/47, 0), Point(i*winWidth/47,winHeight))
		l2 = Line(Point(0, i*winHeight/47), Point(winWidth, i*winHeight/47))
		l1.setFill('white')
		l2.setFill('white')
		l1.setWidth(3)
		l2.setWidth(3)
		l1.draw(win)
		l2.draw(win)
	
	# Axis labels
	horzText = Text(Point(winWidth/2, winHeight-25), "Frequency (Hz) (" + str(maxFreq/float(horzLines)) +" Hz/div)")
	horzText.draw(win)
	vertText = Text(Point(35, winHeight/2), "Power\n(dB)\n" + str(minDb/float(vertLines)) + "\ndB/div")
	vertText.draw(win)

	# Cursor stuff
	t1 = Text(Point(250,25), "")
	t1Text = ""
	t1.setFill('red')
	c1 = Circle(Point(0, 0), 3)
	c1.setFill("red")
	t2 = Text(Point(250,75), "")	
	t2Text = ""
	t2.setFill('blue')
	c2 = Circle(Point(0, 0), 3)
	c2.setFill("blue")
	t1.draw(win2)
	c1.draw(win)
	t2.draw(win2)	
	c2.draw(win)
	dText = Text(Point(250, 125), "")
	dText.draw(win2)
	
	win.setCoords(0, minDb, maxFreq, 0)

	while(win.isClosed() != 1):
		
		# Cursor updator
		if (cursor == 0):
			if (str(win.mouseX) != 'None'):
				if (str(win.mouseX) + ', ' + str(win.mouseY) != t2Text):
					p1 = win.checkMouse()
					pc = c1.getCenter()
					c1.move(p1.getX()-pc.getX(), p1.getY()-pc.getY())
					t1.setText('P1: ' + str(p1.getX()) + ' Hz   ' + str(p1.getY()) + ' dB')
					t1Text = str(win.mouseX) + ', ' + str(win.mouseY)
					cursor = 1
		else:
			if (str(win.mouseX) + ', ' + str(win.mouseY) != t1Text):
				p2 = win.checkMouse()
				pc = c2.getCenter()
				c2.move(p2.getX()-pc.getX(), p2.getY()-pc.getY())
				t2.setText('P2: ' + str(p2.getX()) + ' Hz    ' + str(p2.getY()) + ' dB')
				t2Text = str(win.mouseX) + ', ' + str(win.mouseY)
				cursor = 0
				delta = 1
		
		if (delta == 1):
			dText.setText('delta (P2-P1): ' + str(p2.getX()-p1.getX()) + ' Hz    ' + str(p2.getY()-p1.getY()) + ' dB')

		line = sys.stdin.readline()
		if line:
			x, y = line.split()
			p = Point(eval(x), eval(y))
			points.append(p)
			drawLine(win, counter)
			counter += 1
			if (counter == BANDS):
				for i in range(20*BANDS):
					line = sys.stdin.readline()
				undrawLines(win)
				counter = 0
				for i in range(len(points)):
					points.pop()

def drawLine(window, length):	
	l = Line(points[length-1], points[length])
	lines.append(l)
	if(window.isClosed() == 0):
		l.draw(window)
		
def undrawLines(window):	
	for i in range(len(lines)):
		l = lines.pop()
		if(window.isClosed() == 0):	
			l.undraw()
main()

