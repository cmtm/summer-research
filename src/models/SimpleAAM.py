import networkx as nx
import math
import itertools
import AAM

class SimpleAAM(AAM.AAM):
	
	def __init__(self, clusterSize, clusterCount):
		super(SimpleAAM, self).__init__(clusterSize, clusterCount)
		self.network = nx.Graph()
		
		
	
	def storeMessage(self, message):
		super(SimpleAAM, self).storeMessage(message)

		edges = itertools.combinations(enumerate(message), 2)
		
		self.network.add_edges_from(edges)
	
	def readMessage(self, partialMessage):
		knownCharacters = {node for node in \
		      enumerate(partialMessage) if node[1] != None}
		
		# erasures = [position for position in \
		#       xrange(self.clusterCount) if partialMessage[position] == None
		      
		nodes = self.network.nodes()
		"""
		for char in knownCharacters:
			if not char in nodes:
				return None
		"""
		try:
			neighborsList = [frozenset( nx.all_neighbors(self.network, char) ) \
								for char in knownCharacters]
		except nx.NetworkXError:
			return None
		
		activatedNodes = frozenset.intersection(*neighborsList)
		
		
		# this implementation produces an "arbitrary" character
		# in the case of multiple characters per position
		for node in activatedNodes:
			partialMessage[node[0]] = node[1]
		
		
		
		"""
		# This implmentation returns lists where single characters
		# would have been instead
		for node in activatedNodes:
			if partialMessage[node[0]] == None:
				partialMessage[node[0]] = [node[1]]
			else:
				partialMessage[node[0]].append(node[1])
		"""
		
		



