#include <algorithm>
#include <vector>
#include <numeric>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <err.h>
#include <errno.h>
#include <cstring>


#define messageLength 4
#define aam_port 0x70000000
#define erasure_val 0xFFFFFFFF

#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
  __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)

#define MAP_SIZE 160UL

enum Cmd {READ = 0, STORE = 1};

template <class InputIterator>
void storeMessage(InputIterator msg, ulong* map_base) {
	std::copy(msg, msg + messageLength, map_base + 1);
	msync(map_base + 1, messageLength, MS_SYNC);
	*map_base = STORE;
	msync(map_base, 1, MS_SYNC);

}

template <class InputIterator>
std::vector<ulong> 
readMessage(InputIterator msg, ulong* map_base) {
	std::copy(msg, msg + messageLength, map_base + 1);
	msync(map_base + 1, messageLength, MS_SYNC);
	*map_base = READ;
	msync(map_base, 1, MS_SYNC);
	int erasures = std::count (msg, msg + messageLength, erasure_val);
	
	return std::vector<ulong>(map_base, map_base + erasures);
}

std::vector<ulong> 
getRespsForCluster(const std::vector<ulong>& respPerCluster,
              ulong erasurePos, ulong* map_base) {
	// offset is the number of words before the cluster we're interested in
	// it's the sum of the number of erased clusters and the total responders
	// before the cluster we're interested in.
	ulong offset;

	offset = respPerCluster.size();
	offset += std::accumulate(respPerCluster.begin(), respPerCluster.begin() + erasurePos, 0);


	return std::vector<ulong>(map_base + offset, map_base + offset +respPerCluster[erasurePos]);
}

void eraseMessage(std::vector<ulong>& msg, ulong erasurePos) {
	msg[erasurePos] = erasure_val;
}

int main(void) {
    int fd;
    ulong* map_base;

	if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) FATAL;
	map_base = (ulong*) mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, aam_port);
	printf("/dev/mem opened.\n"); 
	fflush(stdout);
	//let's store a message
	std::vector<ulong> myMessage;
	myMessage.push_back(4); myMessage.push_back(7); myMessage.push_back(2); myMessage.push_back(4);
	storeMessage(myMessage.begin(), map_base);
	std::cout << "Message [4,7,2,4] written." << std::endl;

	//let's see if read works properly
	eraseMessage(myMessage, 0);
	eraseMessage(myMessage, 3);
	std::vector<ulong> rslt = readMessage(myMessage.begin(), map_base);
	std::cout << "[X,7,2,X] read" << std::endl;
	std::cout << "Contends of result from read: size= "
	          <<rslt.size()<<" vals= ("<<rslt[0]<<", "<<rslt[1]<<")"<<std::endl;
	
}

