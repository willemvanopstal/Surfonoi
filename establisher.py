# author: Willem van Opstal
# date: april 10 2018

from pysurfonoi import *

def establishPoints(source):
	with open(source) as srci:
		identifier = 0
		for line in srci.readlines()[1:50]:
			itemList = []
			for item in line.split(';'):
				itemList.append(float(item))
			Measurement(itemList, identifier)
			identifier += 1
	return

establishPoints('hdwest.csv')
print Measurement.totalObjects()

Measurement.fullDelaunay()
Measurement.fullVoronoi()
Measurement.establishNeighbors()

Measurement.iterateAll()
Measurement.updateQueue()

Measurement.printAllInstances()
