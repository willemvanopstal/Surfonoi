# author: Willem van Opstal
# date: april 10 2018


dummySource = "3.5,4.0,2.1\n3.6,4.0,2.2\n3.7,4.2,2.1\n3.8,4.4,2.4\n3.6,4.5,2.0\n3.6,3.9,2.2"


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


print 'establish points'
establishPoints('hdwest.csv')
print 'established points'
print 'pointcount:', Measurement.totalObjects()
print 'start DT'
Measurement.fullDelaunay()
print 'finish DT'
print 'start VD'
Measurement.fullVoronoi()
print 'finish VD'
print 'find neighbors'
Measurement.establishNeighbors()
print 'found neighbors'
print '\n########'

#Measurement.printAllInstances()
Measurement.iterateAll()
Measurement.updateQueue()

Measurement.printAllInstances()


'''
plt.triplot(Measurement.npArray()[:,0], Measurement.npArray()[:,1], Measurement.DT.simplices.copy())
plt.show()

vor_original = Measurement.fullVoronoi()
voronoi_plot_2d(vor_original)
plt.show()
'''