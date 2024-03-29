#include <sys/io.h>
#include <algorithm>
#include <vector>
#include <numeric>
#include <stdio.h>
#include <iostream>

#define messageLength 4
#define aam_port 0x70000000
#define erasure_val 0xFFFFFFFF

enum Cmd {READ = 0, STORE = 1};

template <class InputIterator>
void outl_s (ulong __port, InputIterator first, ulong len) {
	do {
		len--;
		outl(*(first + len), __port + len);
	} while(len > 0);
}

std::vector<ulong>
inl_s (ulong __port, ulong len) {
	std::vector<ulong> in;
	in.reserve(len);
	do {
		len--;
		in[len] = inl(__port + len);
	} while(len > 0);

	return in;
}

template <class InputIterator>
void storeMessage(InputIterator msg) {
	outl_s(aam_port + 1, msg, messageLength);
	outl(STORE, aam_port);
}

template <class InputIterator>
std::vector<ulong> 
readMessage(InputIterator msg) {
	outl_s(aam_port + 1, msg, messageLength);
	outl(READ, aam_port);
	int erasures = std::count (msg, msg + messageLength, erasure_val);
	auto respsPerCluster(inl_s(aam_port, erasures));

	return respsPerCluster;
}

std::vector<ulong> 
getRespsForCluster(const std::vector<ulong>& respPerCluster,
              ulong erasurePos) {
	// offset is the number of words before the cluster we're interested in
	// it's the sum of the number of erased clusters and the total responders
	// before the cluster we're interested in.
	ulong offset;

	offset = respPerCluster.size();
	auto start = respPerCluster.begin();
	offset += std::accumulate(start, start + erasurePos, 0);


	return inl_s(aam_port + offset, respPerCluster[erasurePos]);
}

void eraseMessage(std::vector<ulong>& msg, ulong erasurePos) {
	msg[erasurePos] = erasure_val;
}

int main(void) {
	std::cout << "Requesting permission to access port..." << std::endl;
	// get permission to access IO port.
	int r = ioperm(aam_port, 4 * 4 * 10, 1);
	std::cout << "Response from ioperm: " << r << std::endl;

	//let's store a message
	std::vector<ulong> myMessage({4,7,2,4});
	storeMessage(myMessage.begin());
	std::cout << "Message [4,7,2,4] written." << std::endl;

	//let's see if read works properly
	eraseMessage(myMessage, 0);
	eraseMessage(myMessage, 3);
	auto rslt = readMessage(myMessage.begin());
	std::cout << "[X,7,2,X] read" << std::endl;
	std::cout << "Contends of result from read: size= "
	          <<rslt.size()<<" vals= ("<<rslt[0]<<", "<<rslt[1]<<")"<<std::endl;
	
}

