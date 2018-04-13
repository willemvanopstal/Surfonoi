# Surfonoi
[Voronoi-based method for generalization of hydrographic depth-contours](https://3d.bk.tudelft.nl/rypeters/pdfs/14marinegeo.pdf)

## Python prototype

```python
from pysurfonoi import *

Measurement.fromCsv( 'source', delimiter=',')
Measurement.establishNetwork()
Measurement.establishNeighbors()

Measurement.status()

for i in range(10):
  Measurement.iterateAll()
  Measurement.updateQueue()

Measurement.status()

Measurement.visTriangles()
```
