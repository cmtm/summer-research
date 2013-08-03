import pdb

import math
import itertools
import collections
import AAM
from graphs import ClusteredGraph

class Iterative2AAM(AAM.AAM):
	
	def __init__(self, clusterSize, clusterCount):
		super(Iterative2AAM, self).__init__(clusterSize, clusterCount)
		self.graph = ClusteredGraph(clusterSize, clusterCount)
		
		
	
	def storeMessage(self, message):
		# call to super just checks length
		super(Iterative2AAM, self).storeMessage(message)
		self.graph.makeClique(enumerate(message))
	
	def readMessage(self, partialMessage, gamma = 1):
		Vm = set()
		responders = collections.OrderedDict()
		for v in enumerate(partialMessage):
			if v[1] == None:
				#it's an erasure, add an empty responders set
				responders[v[0]] = set()
			else:
				Vm.add(v)

		iterations = 0
		progress = True
		while progress:
			Vw = set()
			
			nodeAdjClusters = collections.defaultdict(lambda:collections.defaultdict(set))
			nodeValues = collections.defaultdict(lambda:collections.defaultdict(int))
			for (i,j) in Vm:
				nodeValues[i][j] += gamma
				for (ni,nj) in self.graph.getAdj( (i,j) ):
					nodeAdjClusters[ni][nj].add(i)

			for (c, ns) in nodeAdjClusters.iteritems():
				for (n, adjacents) in ns.iteritems():
					nodeValues[c][n] += len(adjacents)

			
			for (i, cluster) in nodeValues.iteritems():
				maxVal = max(cluster.itervalues())
				Vw |= {(i,j) for j,v in cluster.iteritems() if v == maxVal}
				
			if Vw == Vm:
				progress = False
			
			Vm = Vw			
			iterations += 1
		
		for v in Vm:
			if v[0] in responders:
				responders[v[0]].add(v[1])

		# responderCounts = []
		# responderArray = []
		# for (cluster, neurons) in responders.items():
		# 	responderCounts.append(len(neurons))
		# use list comprehension instead
		responderCounts = [len(r) for r in responders.values()]
		responderVals = [sorted(r) for r in responders.values()]
		respondersFlat = list(itertools.chain.from_iterable(responderVals))
		#iterations isn't used for now
		#return iterations
		return (responderCounts + respondersFlat)