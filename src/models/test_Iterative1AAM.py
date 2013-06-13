import random
import unittest
import pdb

from Iterative1AAM import Iterative1AAM

class TestSequenceFunctions(unittest.TestCase):
	
	def setUp(self):
		self.c_size = random.randint(1, 1000)
		self.c_count = random.randint(1, 1000)

	def test_networkSize(self):
		# ensure the network accepts proper sizes and reject improper ones
        
        
		try:
			Iterative1AAM(self.c_size, self.c_count)
		except Exception:
			self.fail("Iterative1AAM init raised an exception on valid input")

		self.assertRaises(Exception, Iterative1AAM, (0, self.c_count) )
		self.assertRaises(Exception, Iterative1AAM, (self.c_size, 0) )

	def test_messageLength(self):
		
		AAM_ut = Iterative1AAM(self.c_size, self.c_count)
		validMsg = [random.randrange(self.c_size) for i in range(self.c_count)]
		#try:
			#AAM_ut.storeMessage(validMsg)
		#except Exception:
			#self.fail("Iterative1AAM store raised an exception on valid input")
		AAM_ut.storeMessage(validMsg)
		
		invalidMsg1 = [random.randrange(self.c_size) \
		        for i in range(self.c_count + random.randint(1,50))]
		invalidMsg2 = [random.randrange(self.c_size) \
		        for i in range(self.c_count - random.randint(1,self.c_count))]
		
		self.assertRaisesRegexp(Exception, "Inappropriate message length.", \
                        AAM_ut.storeMessage, invalidMsg1)
		self.assertRaisesRegexp(Exception, "Inappropriate message length.", \
                        AAM_ut.storeMessage, invalidMsg2)
        
        # TODO: check for character size (and not just count)

	def test_singleStRd(self):
		AAM_ut = Iterative1AAM(12, 5)
		msg = [2,5,1,2,11]
		AAM_ut.storeMessage(msg)
		AAM_ut.graph.drawGraph('myGraph.gv')
		query = [2, None, 1, 2, None]
		AAM_ut.readMessage(query)
		self.assertEqual(msg, query)
	
	def test_multipleStRd(self):
		cSize = 50
		cCount = 10
		AAM_ut = Iterative1AAM(cSize, cCount)
		msgs = [ [random.randrange(cSize) for i in range(cCount)] \
		            for j in range(10)]
		for msg in msgs:
			AAM_ut.storeMessage(msg)
		
		msgsErased = []
		for j in range(cCount):
			r = random.randrange(cCount)
			msgsErased.append([None if r == i else msgs[j][i] for i in range(cCount)])
		
		for msg in msgsErased:
			AAM_ut.readMessage(msg)
		
		self.assertEqual(msgs, msgsErased)
	
	# TODO: create a test that verifies behavior when AAM becomes saturated

if __name__ == '__main__':
    unittest.main()
