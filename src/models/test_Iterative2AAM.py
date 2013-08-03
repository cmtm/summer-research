import random
import unittest
import pdb

from Iterative2AAM import Iterative2AAM

class TestSequenceFunctions(unittest.TestCase):
	
	def setUp(self):
		self.c_size = random.randint(1, 1000)
		self.c_count = random.randint(1, 1000)

	def test_networkSize(self):
		# ensure the network accepts proper sizes and reject improper ones
        
        
		try:
			Iterative2AAM(self.c_size, self.c_count)
		except Exception:
			self.fail("Iterative2AAM init raised an exception on valid input")

		self.assertRaises(Exception, Iterative2AAM, (0, self.c_count) )
		self.assertRaises(Exception, Iterative2AAM, (self.c_size, 0) )

	def test_messageLength(self):
		
		AAM_ut = Iterative2AAM(self.c_size, self.c_count)
		validMsg = [random.randrange(self.c_size) for i in range(self.c_count)]
		#try:
			#AAM_ut.storeMessage(validMsg)
		#except Exception:
			#self.fail("Iterative2AAM store raised an exception on valid input")
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
		AAM_ut = Iterative2AAM(12, 5)
		msg = [2,5,1,2,11]
		AAM_ut.storeMessage(msg)
		query = [2, None, 1, 2, None]
		result = AAM_ut.readMessage(query)
		self.assertEqual(result, [1,1,5,11])
	
	def test_multipleResponder(self):
		AAM_ut = Iterative2AAM(12,5)
		msgs = [[2,5,1,2,11],
		        [2,5,1,2,7], 
		        [2,5,1,2,2], 
		        [2,5,1,9,3]]
		for msg in msgs:
			AAM_ut.storeMessage(msg)
		result = AAM_ut.readMessage([2,5,1,None,None])
		self.assertEqual(result, [2,4,2,9,2,3,7,11])
	
	# TODO: create a test that verifies behavior when AAM becomes saturated

if __name__ == '__main__':
    unittest.main()
