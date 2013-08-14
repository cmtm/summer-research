#include "aam_driver.h"

enum Cmd {QUERY = 0, STORE = 1};

static word* device_base;

// this must be called before driver use
int init_driver() {
	int fd;
	if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
		return -1;
	else {
		device_base = (word*) mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, AAM_ADDR);
		return 0;
	}
}

void storeAssociation(const word* assoc) {
	memcpy(device_base + 1, assoc, ASSOC_LENGTH * WORD_SIZE);
	msync(device_base + 1, ASSOC_LENGTH * WORD_SIZE, MS_SYNC);
	*device_base = STORE;
	msync(device_base, 1, MS_SYNC);
}

// returns pointer to result. Pointer is directly to IO device
word* queryAssociation(const word* assoc) {
	memcpy(device_base + 1, assoc, ASSOC_LENGTH * WORD_SIZE);
	msync(device_base + 1, ASSOC_LENGTH * WORD_SIZE, MS_SYNC);
	*device_base = QUERY;
	msync(device_base, 1, MS_SYNC);
}

inline void eraseMessage(word* assoc, uint erasurePos) {
	assoc[erasurePos] = ERASURE;
}

word* getRespsForCluster(const word* queryResult, uint erasurePos, word erasures) {
	// offset is the number of words before the cluster we're interested in
	// it's the sum of the number of erased clusters and the total responders
	// before the cluster we're interested in.
	
	uint offset = erasures;

	for(int i = 0; i < erasurePos; i++)
		offset += queryResult[i];

	// device base is used instead of query result incase the caller copied the
	// return from queryAssociation and supplied a pointer to the copy
	return device_base + offset;
}