# author: Willem van Opstal
# date: april 10 2018

import numpy as np
from scipy.spatial import Delaunay, Voronoi, voronoi_plot_2d
import matplotlib.pyplot as plt
from math import sqrt

def distance(xone, yone, xtwo, ytwo):
	xdif = pow((xtwo - xone),2)
	ydif = pow((ytwo - yone),2)
	return sqrt(xdif + ydif)

def ccw(Ax, Bx, Cx, Ay, By, Cy):
    return (Cy-Ay) * (Bx-Ax) > (By-Ay) * (Cx-Ax)

def intersect(Ax, Bx, Cx, Dx, Ay, By, Cy, Dy):
    return ccw(Ax, Cx, Dx, Ay, Cy, Dy) != ccw(Bx, Cx, Dx, By, Cy, Dy) and ccw(Ax, Bx, Cx, Ay, By, Cy) != ccw(Ax, Bx, Dx, Ay, By, Dy)

class Measurement(object):

    _instances = []
    _easyIndex = {}

    def __init__(self, initList, identification):
        self.depth_queue = None
        self.updateNr = 0
        self.neighbors = set()
        self.voronoiRing = []
        self.loc_x = initList[0]
        self.loc_y = initList[1]
        self.depth_measurement = initList[2]
        self.depth_current = initList[2]
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
                print item.identification
                print item.depth_measurement
                print item.depth_current
                print item.updateNr
                #print item.depth_queue

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
    def updateQueue(cls):
        for item in cls._instances:
            if item.depth_queue:
                item.depth_current = item.depth_queue
                item.updateNr += 1
            item.depth_queue = None
        print 'instances updated'


    @classmethod
    def establishNeighbors(cls):
        index = cls._easyIndex
        triangles = cls.DT.points[cls.DT.vertices]
        for item in triangles:
            index[str(item[0])].neighbors.add(index[str(item[1])])
            index[str(item[0])].neighbors.add(index[str(item[2])])
            index[str(item[1])].neighbors.add(index[str(item[0])])
            index[str(item[1])].neighbors.add(index[str(item[2])])
            index[str(item[2])].neighbors.add(index[str(item[0])])
            index[str(item[2])].neighbors.add(index[str(item[1])])

        # Natural Neighbors > Measurement.VD
        for k, point in enumerate(cls.VD.points):

            ring = cls.VD.point_region[k]
            if -1 in cls.VD.regions[ring]:
                continue
            else:
                vList = []
                for i in range(len(cls.VD.regions[ring])):
                    if not cls.VD.regions[ring][i] == -1:
                        vList.append(cls.VD.vertices[cls.VD.regions[ring][i]])
                    else:
                        print ''
                if not cls.VD.regions[ring][0] == -1:
                    vList.append(cls.VD.vertices[cls.VD.regions[ring][0]])
                else:
                    print ''

                index[str(point)].voronoiRing = vList

        return

    @classmethod
    def iterateAll(cls):
        for item in cls._instances:
            sumOfWeights = 0.0
            upper = 0.0
            if len(item.voronoiRing) != 0:  # now im only an inner point in the VD
                '''
                print '---'
                print 'location:\t{0};{1}'.format(item.loc_x, item.loc_y)
                print 'depth:\t\t{0} m'.format(item.depth_measurement)
                print 'neighbors:\t{0}'.format(len(item.voronoiRing ) -1)
                '''
                for neighbor in item.neighbors:
                    # print 'neighbor:\t{0}'.format(neighbor)
                    dtDistance = distance(item.loc_x, item.loc_y, neighbor.loc_x, neighbor.loc_y)
                    # print 'DT distance:\t{0}'.format(dtDistance)

                    # check formulas...
                    for p in range(len(item.voronoiRing ) -1):
                        checker = intersect(item.loc_x, neighbor.loc_x, item.voronoiRing[p][0], item.voronoiRing[ p +1][0], item.loc_y, neighbor.loc_y, item.voronoiRing[p][1], item.voronoiRing[ p +1][1])
                        if checker:
                            vdDistance = distance(item.voronoiRing[p][0], item.voronoiRing[p][1], item.voronoiRing[ p +1][0], item.voronoiRing[ p +1][1])
                        # print 'VD distance:\t{0}'.format(vdDistance)
                    weightNbr = vdDistance /dtDistance
                    sumOfWeights += weightNbr
                    upper += (weightNbr * neighbor.depth_current)
                # print item.depth_measurement
                interpolated_depth = upper /sumOfWeights
                # print 'interpolated depth:\t{0}'.format(interpolated_depth)
                if interpolated_depth < item.depth_current:
                    item.depth_queue = interpolated_depth
                else:
                    item.depth_queue = None
                # print 'queue:\t\t:{0}'.format(item.depth_queue)