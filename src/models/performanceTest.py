import collections
from random import randrange, sample

from Iterative1AAM import Iterative1AAM


class PerformanceTest(object):
	def __init__(self, aamConstructor, clusterSize, clusters, msgCount):
		self.clusterSize = clusterSize
		self.clusters = clusters
		self.msglen = clusters
		self.msgCount = msgCount
		self.aam_ut = aamConstructor(clusterSize, clusters)
		self.msgs = [ [randrange(clusterSize) for i in range(self.msglen)] \
		            for j in range(msgCount)]
		for msg in self.msgs:
			self.aam_ut.storeMessage(msg)
	
	def readExisting(self, erasures, messages = None):
		fails = 0
		iterations = []
		def erase(msg, erasures):
			for i in sample(range(self.msglen), erasures):
				msg[i] = None
		# messages of None causes the program to go through all the messages
		if messages == None:
			for msg in self.msgs:
				# need to copy as the read acts in place
				copy = msg[:]
				erase(copy, erasures)
				iterations.append(self.aam_ut.readMessage(copy) - 1)
				if copy != msg:
					fails += 1
		return (fails, iterations)	
		
		
			
				
		for i in range(messages):
			msg = [randrange(self.clusterSize) for i in range(self.clusters)]

if __name__ == '__main__':
	for i in range(1, 2000, 100):
		p = PerformanceTest(Iterative1AAM, 128, 16, i)
		r = p.readExisting(2)
		print("messages: {} fails: {} average iterations: {}".format( \
		       i, r[0], sum(r[1])/len(r[1])))
