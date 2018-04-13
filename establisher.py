# author: Willem van Opstal
# date: april 10 2018

import time
from pysurfonoi import *




startTime = time.time()

Measurement.fromCsv('small_portion.csv')
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
Measurement.visTriangles()

