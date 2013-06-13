import pdb

import math
import itertools
import collections
import pydot

import AAM
from graphs import ClusteredGraph

class Iterative1AAM(AAM.AAM):
	
	def __init__(self, clusterSize, clusterCount):
		super(Iterative1AAM, self).__init__(clusterSize, clusterCount)
		self.graph = ClusteredGraph(clusterSize, clusterCount)
		
		
	
	def storeMessage(self, message):
		# call to super just checks length
		super(Iterative1AAM, self).storeMessage(message)
		self.graph.makeClique(enumerate(message))
	
	def readMessage(self, partialMessage, gamma = 1):
		Vm = {v for v in enumerate(partialMessage) \
		                           if v[1] != None}
		iterations = 0
		progress = True
		while progress:
			Vw = set()
			
			
			nodeValues = collections.defaultdict(lambda:collections.defaultdict(int))
			for (i,j) in Vm:
				nodeValues[i][j] += gamma
				for (ni,nj) in self.graph.getAdj( (i,j) ):
					nodeValues[ni][nj] += 1
			
			for (i, cluster) in nodeValues.iteritems():
				maxVal = max(cluster.itervalues())
				Vw |= {(i,j) for j,v in cluster.iteritems() if v == maxVal}
				
			if Vw == Vm:
				progress = False
			
			Vm = Vw			
			iterations += 1
		
		# can optimize this
		for i in range(len(partialMessage)):
			if partialMessage[i] == None:
				for (c, n) in Vm:
					if c == i:
						partialMessage[i] = n
						Vm.remove((c,n))
						break
		return iterations	
			#for (i,c) in graph.iteritems():
				#highestSym = {}
				#if partialMessage[i] != None:
					
					#highestNv[partialMessage[i]] = gamma
					#connectedClusters = set(c for (c,n) in graph.nodes.getAdj( (i,j) ))
				#for (j,v) in c.iteritems():
					#connectedClusters = set(c for (c,n) in graph.nodes.getAdj( (i,j) ))
					#highestN
