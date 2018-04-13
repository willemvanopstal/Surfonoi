from pysurfonoi import *

Measurement.fromCsv('small_portion.csv')
Measurement.establishNetwork()
Measurement.establishNeighbors()

Measurement.status()
Measurement.visTriangles()

Measurement.densify()

Measurement.status()
Measurement.visTriangles()