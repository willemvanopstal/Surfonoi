# author: Willem van Opstal
# date: april 10 2018

from utility_functions import *

import numpy as np
from scipy.spatial import Delaunay, Voronoi, voronoi_plot_2d
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import time
try:
    import pickle
except:
    print '> pickle not available\n'


class Measurement(object):

    _instances = []
    _easyIndex = {}
    _iterations = 0
    _changes = {}
    DT = False
    VD = False

    depth_queue = None


    def __init__(self, initList, identification, densifier=False):
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

        self.densePoint = densifier


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
                print item.depth_queue
                print item.updateNr
                # print item.depth_queue


    @classmethod
    def status(cls):
        cls.findMinMax()

        print '\n/////// STATUS REPORT ///////'
        print 'Total objects:\t\t', cls.totalObjects()
        print 'Delaunay:\t\t', str(cls.DT)
        print 'Voronoi:\t\t', str(cls.VD)
        print 'Iterations:\t\t', cls._iterations
        print 'X min max:\t\t{0} {1}'.format(cls.xmin, cls.xmax)
        print 'Y min max:\t\t{0} {1}'.format(cls.ymin, cls.ymax)
        print 'Z min max:\t\t{0} {1}'.format(cls.dmin, cls.dmax)
        print '///////  CHANGE LOG  ///////'
        totalUpdates = 0
        for key in cls._changes:
            print 'iter {0} had {1} changes'.format(key, cls._changes[key])
            totalUpdates += cls._changes[key]
        print 'total: ', totalUpdates
        print '/////// STATUS REPORT ///////\n'


    @classmethod
    def findMinMax(cls):
        xmin = 9999999999.9
        xmax = 0.0
        ymin = 9999999999.9
        ymax = 0.0

        dmin = -2.5
        dmax = 0

        for item in cls._instances:
            if item.depth_current == 99999.9:
                continue
            if item.loc_x < xmin:
                xmin = item.loc_x
            if item.loc_x > xmax:
                xmax = item.loc_x
            if item.loc_y < ymin:
                ymin = item.loc_y
            if item.loc_y > ymax:
                ymax = item.loc_y
            if item.depth_current > dmax:
                dmax = item.depth_current
            if item.depth_current < dmin:
                dmin = item.depth_current

        cls.xmin = xmin
        cls.xmax = xmax
        cls.ymin = ymin
        cls.ymax = ymax
        cls.dmin = dmin
        cls.dmax = dmax


    @classmethod
    def visTriangles(cls):
        #https://stackoverflow.com/questions/43393162/how-to-plot-3d-triangles-in-matplotlib-with-triangles-verticess-coordinates-9
        visTime = time.time()
        cls.findMinMax()

        fList = []
        for triangle in cls.DT.points[cls.DT.simplices]:
            if cls._easyIndex[str(triangle[0])].depth_current == 99999.9 or cls._easyIndex[str(triangle[1])].depth_current == 99999.9 or cls._easyIndex[str(triangle[2])].depth_current == 99999.9 :
                continue
            else:
                vList = []
                for vertex in triangle:
                    vList.append(vertex[0])
                    vList.append(vertex[1])
                    vList.append(cls._easyIndex[str(vertex)].depth_current)
                vArray = np.array(vList)
                fList.append(vArray)
        fArray = np.array(fList)

        cmap = cm.get_cmap('ocean')
        poly3d = [[ fArray[i, j*3:j*3+3] for j in range(3)  ] for i in range(fArray.shape[0])]
        fc = [cmap(((fArray[i][-1]+fArray[i][-4]+fArray[i][-7])/3)/(cls.dmax-cls.dmin)) for i in range(fArray.shape[0])]

        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')
        ax.add_collection3d(Poly3DCollection(poly3d, facecolors=fc, linewidths=0))
        ax.set_xlim(cls.xmin, cls.xmax)
        ax.set_ylim(cls.ymin, cls.ymax)
        ax.set_zlim(cls.dmin, cls.dmax)
        ax.invert_zaxis()

        print 'visualizationTime:\t', time.time()-visTime
        plt.gca().set_aspect('equal', adjustable='box')
        #ax.set_aspect(1)
        plt.show()


    @classmethod
    def fromCsv(cls, source, delimiter=','):
        cls.sourceFile = source.split('.')[0]
        with open(source) as srci:
            identifier = 0
            for line in srci.readlines()[1:]:
                itemList = []
                for item in line.split(delimiter):
                    itemList.append(float(item))
                cls(itemList, identifier)
                identifier += 1
        return


    @classmethod
    def asCsv(cls, filepath):
        with open(filepath, 'w') as fh:
            fh.write('x;y;depth\n')
            for item in cls._instances:
                stringer = str(item.loc_x) + ';' + str(item.loc_y) + ';' + str(item.depth_current) + '\n'
                fh.write(stringer)


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
    def establishNetwork(cls):
        array = cls.npArray()
        cls.DT = Delaunay(array)
        cls.VD = Voronoi(array)
        return


    @classmethod
    def updateQueue(cls):
        marker = False
        updates = 0
        for item in cls._instances:
            if item.depth_queue:
                item.depth_current = item.depth_queue
                item.updateNr += 1
                marker = True
                updates += 1
            item.depth_queue = None
        if marker:
            cls._iterations += 1
            cls._changes[cls._iterations] = updates
        print 'instances updated', updates


    @classmethod
    def cleanNeighbors(cls):
        for item in cls._instances:
            item.neighbors = set()
            item.voronoiRing = []


    @classmethod
    def densify(cls):
        index = cls._easyIndex
        triangles = cls.DT.points[cls.DT.vertices]

        latestId = cls._instances[-1].identification
        startId = latestId + 1
        print latestId

        for item in triangles:
            triArea = area(item[0], item[1], item[2])
            if triArea > 1000:                                       # THRESHOLD
                latestId += 1
                print triArea
                newPoint = circumcenter(item[0], item[1], item[2])
                newList = newPoint.tolist()
                newList.append(99999.9)
                #print newList, latestId

                cls(newList, latestId, True) # create new instances, including hint on dense point
                cls.DT = False
                cls.VD = False
        endId = latestId

        print 'establishing network/neighbors again..'
        cls.cleanNeighbors()
        cls.establishNetwork()
        cls.establishNeighbors()

        queued = 0
        for item in cls._instances:
            if item.identification >= startId and item.identification <= endId:
                sumOfWeights = 0.0
                upper = 0.0
                if len(item.voronoiRing) != 0:  # now im only an inner point in the VD
                    for neighbor in item.neighbors:
                        if neighbor.depth_current == 99999.9:
                            continue
                        # print 'neighbor:\t{0}'.format(neighbor)
                        dtDistance = distance(item.loc_x, item.loc_y, neighbor.loc_x, neighbor.loc_y)
                        # print 'DT distance:\t{0}'.format(dtDistance)

                        # check formulas...
                        for p in range(len(item.voronoiRing) - 1):
                            checker = intersect(item.loc_x, neighbor.loc_x, item.voronoiRing[p][0],
                                                item.voronoiRing[p + 1][0], item.loc_y, neighbor.loc_y,
                                                item.voronoiRing[p][1], item.voronoiRing[p + 1][1])
                            if checker:
                                vdDistance = distance(item.voronoiRing[p][0], item.voronoiRing[p][1],
                                                      item.voronoiRing[p + 1][0], item.voronoiRing[p + 1][1])
                                break
                            # print 'VD distance:\t{0}'.format(vdDistance)
                        weightNbr = vdDistance / dtDistance
                        sumOfWeights += weightNbr
                        upper += (weightNbr * neighbor.depth_current)
                    print item.depth_measurement
                    if sumOfWeights == 0.0:
                        interpolated_depth = 99999.9
                    else:
                        interpolated_depth = upper / sumOfWeights
                    print 'interpolated depth:\t{0}'.format(interpolated_depth)
                    if interpolated_depth < item.depth_current:
                        item.depth_queue = interpolated_depth
                        queued += 1
                    else:
                        continue
            else:
                continue
        print 'queued points:\t\t', queued

        cls.updateQueue()

        return


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
            if -1 not in cls.VD.regions[ring]:
                vList = []
                for i in range(len(cls.VD.regions[ring])):
                    if not cls.VD.regions[ring][i] == -1:
                        vList.append(cls.VD.vertices[cls.VD.regions[ring][i]])
                    else:
                        continue
                if not cls.VD.regions[ring][0] == -1:
                    vList.append(cls.VD.vertices[cls.VD.regions[ring][0]])
                else:
                    continue

                index[str(point)].voronoiRing = vList
            else:
                continue
        return


    @classmethod
    def iterateAll(cls):
        queued = 0
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
                            break
                        # print 'VD distance:\t{0}'.format(vdDistance)
                    weightNbr = vdDistance /dtDistance
                    sumOfWeights += weightNbr
                    upper += (weightNbr * neighbor.depth_current)
                # print item.depth_measurement
                interpolated_depth = upper /sumOfWeights
                # print 'interpolated depth:\t{0}'.format(interpolated_depth)
                if interpolated_depth < item.depth_current:
                    item.depth_queue = interpolated_depth
                    queued += 1
                else:
                    continue
        print 'queued points:\t\t', queued
        # print 'queue:\t\t:{0}'.format(item.depth_queue)
        #return