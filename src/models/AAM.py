

class AAM(object):
	
	def __init__(self, clusterSize, clusterCount):
		
		if clusterSize == 0 or clusterCount == 0:
			raise Exception("AAM with zero capacity created.")
		

		self.clusterSize = clusterSize
		self.clusterCount = clusterCount
		
		self.fanalCount = clusterSize * clusterCount
	
	def storeMessage(self, message):
		if len(message) != self.clusterCount:
			raise Exception("Inappropriate message length.")
		# TODO: check size of characters. Ensure they're small enough
	
	def readMessage(self, partialMessage):
		pass
	
	
	
