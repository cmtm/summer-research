

class AAM:
	
	def __init__(self, clusterSize, clusterCount):
		
		if clusterSize == 0 or clusterCount == 0:
			raise Exception("AAM with zero capacity created.")
		

		self.clusterSize = clusterSize
		self.clusterCount = clusterCount
		
		self.fanalCount = clusterSize * clusterCount
	
	def storeMessage(self, message):
		pass
	
	def readMessage(self, partialMessage):
		pass
	
	
	
