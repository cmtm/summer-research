import itertools
import pydot

class ClusteredGraph(object):
	def __init__(self, clusterSize, clusterCount):
		# self.nodes[c][n] to access the nth node in cluster c
		# a node is a set of it's neighbors as (c,n) tuples
		self.clusterSize = clusterSize
		self.clusterCount = clusterCount
		self.nodes = {c: { n:set() for n in range(clusterSize) } for c in range(clusterCount)}
	
	def add_edge(self, n1, n2):
		self.nodes[n1[0]][n1[1]].add(n2)
		self.nodes[n2[0]][n2[1]].add(n1)
	
	def makeClique(self, nodes):
		edges = itertools.combinations(nodes, 2)
		
		for e in edges:
			self.add_edge(*e)
	
	def getAdj(self, n):
		return self.nodes[n[0]][n[1]]

	def drawGraph(self, path):	
		dotGraph = pydot.Dot(graph_name="AAM state", graph_type = 'graph')
		dotGraph.set('splines', 'false')
		clusters = []
		# fill graph with nodes
		for i in range(self.clusterCount):
			subgraph = pydot.Cluster(graph_name='{}'.format(i))
			for j in range(self.clusterSize):
				width = int(self.clusterSize ** 0.5)
				n = pydot.Node(name='{}_{}'.format(i, j))
				n.set('pos', '"{},{}"'.format(5*j/width, 5*(j%width)))
				#n.set('pos', (j/width, j%width))
				subgraph.add_node(n)
			dotGraph.add_subgraph(subgraph)
			
		# add edges
		edges = set()
		for (i, c) in self.nodes.iteritems():
			for (j, n) in c.iteritems():
				for other in n:
					edges.add(frozenset(('{}_{}'.format(i,j), '{}_{}'.format(*other))))
		for e in edges:
			dotGraph.add_edge(pydot.Edge(*tuple(e)))
			
		dotGraph.write(path)
