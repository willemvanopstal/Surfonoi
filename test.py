# author: Willem van Opstal
# date: april 10 2018

import numpy as np
from scipy.spatial import Delaunay, Voronoi, voronoi_plot_2d
import matplotlib.pyplot as plt
from math import sqrt

dummySource = "3.5,4.0,2.1\n3.6,4.0,2.2\n3.7,4.2,2.1\n3.8,4.4,2.4\n3.6,4.5,2.0\n3.6,3.9,2.2"

def distance(xone, yone, xtwo, ytwo):
	xdif = pow((xtwo - xone),2)
	ydif = pow((ytwo - yone),2)
	return sqrt(xdif + ydif)

def ccw(Ax, Bx, Cx, Ay, By, Cy):
    return (Cy-Ay) * (Bx-Ax) > (By-Ay) * (Cx-Ax)

def intersect(Ax, Bx, Cx, Dx, Ay, By, Cy, Dy):
    return ccw(Ax, Cx, Dx, Ay, Cy, Dy) != ccw(Bx, Cx, Dx, By, Cy, Dy) and ccw(Ax, Bx, Cx, Ay, By, Cy) != ccw(Ax, Bx, Dx, Ay, By, Dy)	
	
def checkIntersect(Ax, Bx, Cx, Dx, Ay, By, Cy, Dy):
	# https://stackoverflow.com/questions/3838329/how-can-i-check-if-two-segments-intersect
	
	
	
	
	'''slope=(Dy-Cy)/(Dx-Cx)
	dist1=slope*(Cx-Ax)+Ay-Cy
	dist2=slope*(Dx-Bx)+By-Dy
	# return dist1*dist2<0'''
	
	'''if min(Ax, Bx) < max(Cx, Dx) and min(Ay, By) < max(Cy, Dy) and max(Ax, Bx) > min(Cx, Dx) and max(Ay, By) > min(Cy, Dy):
		print 'bbox is intersecting'
	else:
		print ''
	'''
	
	

def establishPoints(source):
	with open(source) as srci:
		identifier = 0
		for line in srci.readlines()[1:]:
			itemList = []
			for item in line.split(';'):
				itemList.append(float(item))
			Measurement(itemList, identifier)
			identifier += 1
	return

def establishNeighbors():
	index = Measurement._easyIndex
	triangles = Measurement.DT.points[Measurement.DT.vertices]
	for item in triangles:
		index[str(item[0])].neighbors.add(index[str(item[1])])
		index[str(item[0])].neighbors.add(index[str(item[2])])
		index[str(item[1])].neighbors.add(index[str(item[0])])
		index[str(item[1])].neighbors.add(index[str(item[2])])
		index[str(item[2])].neighbors.add(index[str(item[0])])
		index[str(item[2])].neighbors.add(index[str(item[1])])

	# Natural Neighbors > Measurement.VD
	for k, point in enumerate(Measurement.VD.points):
		
		ring = Measurement.VD.point_region[k]
		if -1 in Measurement.VD.regions[ring]:
			continue
		else:
			vList = []
			for i in range(len(Measurement.VD.regions[ring])):
				if not Measurement.VD.regions[ring][i] == -1:
					vList.append(Measurement.VD.vertices[Measurement.VD.regions[ring][i]])
				else:
					print ''
			if not Measurement.VD.regions[ring][0] == -1:
				vList.append(Measurement.VD.vertices[Measurement.VD.regions[ring][0]])
			else:
				print ''

			index[str(point)].voronoiRing = vList
	
	return

class Measurement(object):

	_instances = []
	_easyIndex = {}

	def __init__(self, initList, identification):
		self.neighbors = set()
		self.voronoiRing = []
		self.loc_x = initList[0]
		self.loc_y = initList[1]
		self.depth_measurement = initList[2]
		self.depth_original = initList[2]
		self.identification = identification
		self._instances.append(self)
		self.easyIndexer(self.loc_x, self.loc_y)
	
	def __str__(self):
		return str(self.loc_x) + ";" + str(self.loc_y) + ";" + str(self.depth_measurement)

	def easyIndexer(self, lox, loy):
		self._easyIndex[str(np.array([lox, loy]))] = self

	@classmethod
	def printAllInstances(cls):
		# Use me for testing purposes :)
		for item in cls._instances:
			if len(item.voronoiRing) != 0:
				print '---'
				print item.neighbors
				print item.voronoiRing

	@classmethod
	def totalObjects(cls):
		return len(cls._instances)

	@classmethod
	def npArray(cls):
		npList = []
		for item in cls._instances:
			npList.append([item.loc_x, item.loc_y])
		return np.array(npList)

	@classmethod
	def fullDelaunay(cls):
		array = cls.npArray()
		cls.DT = Delaunay(array)
		return Delaunay(array)

	@classmethod
	def fullVoronoi(cls):
		array = cls.npArray()
		cls.VD = Voronoi(array)
		return Voronoi(array)

	@classmethod
	def iterateAll(cls):
		# Use me for testing purposes :)
			
		for item in cls._instances:
			sumOfWeights = 0.0
			upper = 0.0
			if len(item.voronoiRing) != 0: #now im only an inner point in the VD
				print '---'
				print 'location:\t{0};{1}'.format(item.loc_x, item.loc_y)
				print 'depth:\t\t{0} m'.format(item.depth_measurement)
				print 'neighbors:\t{0}'.format(len(item.voronoiRing)-1)
				for neighbor in item.neighbors:
					print 'neighbor:\t{0}'.format(neighbor)
					dtDistance = distance(item.loc_x, item.loc_y, neighbor.loc_x, neighbor.loc_y)
					print 'DT distance:\t{0}'.format(dtDistance)
					
					# check formulas...
					for p in range(len(item.voronoiRing)-1):
						checker = intersect(item.loc_x, neighbor.loc_x, item.voronoiRing[p][0], item.voronoiRing[p+1][0], item.loc_y, neighbor.loc_y, item.voronoiRing[p][1], item.voronoiRing[p+1][1])
						if checker:
							vdDistance = distance(item.voronoiRing[p][0], item.voronoiRing[p][1], item.voronoiRing[p+1][0], item.voronoiRing[p+1][1])
							print 'VD distance:\t{0}'.format(vdDistance)
					weightNbr = vdDistance/dtDistance
					sumOfWeights += weightNbr
					upper += (weightNbr * neighbor.depth_measurement)
				print item.depth_measurement
				print upper/sumOfWeights
			
		
establishPoints('hdwest.csv')
Measurement.fullDelaunay()
Measurement.fullVoronoi()
establishNeighbors()
print '\n########'

#Measurement.printAllInstances()
Measurement.iterateAll()


'''
plt.triplot(Measurement.npArray()[:,0], Measurement.npArray()[:,1], Measurement.DT.simplices.copy())
plt.show()

vor_original = Measurement.fullVoronoi()
voronoi_plot_2d(vor_original)
plt.show()
'''