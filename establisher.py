# author: Willem van Opstal
# date: april 10 2018

import time
from pysurfonoi import *

def establishPoints(source):
    with open(source) as srci:
        identifier = 0
        for line in srci.readlines()[1:]:
            itemList = []
            for item in line.split(','):
                itemList.append(float(item))
            Measurement(itemList, identifier)
            identifier += 1
    return


startTime = time.time()

establishPoints('small_portion.csv')
#Measurement.asCsv('set_points.csv')
print '--------------------------------\ntotal objects:\t\t', Measurement.totalObjects()

midTime = time.time()
print 'initiated points in:\t', midTime - startTime

Measurement.establishNetwork()
Measurement.establishNeighbors()
midmidTime = time.time()
print 'network initited in:\t', midmidTime - midTime

Measurement.iterateAll()
Measurement.updateQueue()

endTime = time.time()
print 'iterated and updated:\t', endTime - midmidTime
print 'total elapsed:\t\t', endTime - startTime
print '--------------------------------\n'

Measurement.asCsv('one_iteration_later.csv')
print 'csv exported'

print 'starting visualization'
#Measurement.printAllInstances()
Measurement.visTin()

