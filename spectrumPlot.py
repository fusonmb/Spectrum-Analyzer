# spectrumPlot.py
#       Plots spectrum data recieved from stdin
#       by Jay Dial, 5/16/2011

from zellegraphics import *
from string import *
import sys, time

lines = []
points = []
BANDS = 100

def main():
	win = GraphWin("Spectrum Analyzer", 1600, 1000)
	win.setCoords(0, -60, 16000, 0)
	counter = 0;

	while(1):
		line = sys.stdin.readline()
		if line:
			x, y = line.split()
			p = Point(eval(x), eval(y))
			print eval(x), eval(y)
			points.append(p)
			drawLine(win, counter)
			counter += 1
			if (counter == BANDS):
				for i in range(20*BANDS):
				#while(line):
					line = sys.stdin.readline()
				undrawLines()
				counter = 0
				for i in range(len(points)):
					points.pop()

def drawLine(window, length):	
	l = Line(points[length-1], points[length])
	lines.append(l)
	l.draw(window)
		
def undrawLines():	
	for i in range(len(lines)):
		l = lines.pop()
		l.undraw()
main()

