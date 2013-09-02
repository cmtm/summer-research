// The AAM stores 'associations' and an association is composed of several
// 'words'. A word takes a certain number of bytes to represent. This number
// of bytes is constant given a certain AAM. No matter the number of bytes
// in a word, the word value where all the bits are '1' represents an 
// erasure and so can't be used as a distinct word. The number of bytes
// per word limits the range of possible words.
//
// For example:
// 1 byte would allow for 2^(1*8) - 1 = 255 distinct words 
// (notice the subtraction of 1. This is because of the erasure value)
// 4 bytes would allow for 2^(4*8) ~ 4.295 Billion distinct words.

#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>

#define ASSOC_LENGTH 4
#define WORD_SIZE 4
#define AAM_ADDR 0x70000000
#define ERASURE 0xFFFFFFFF

#define MAP_SIZE (ASSOC_LENGTH * WORD_SIZE + 1)

// this typedef is based on the WORD_SIZE
typedef uint32_t word;
typedef unsigned int uint;

int init_driver();

void storeAssociation(const word* assoc);

// returns pointer to result. Pointer is directly to IO device
word* queryAssociation(const word* assoc);

extern void eraseMessage(word* assoc, uint erasurePos);

word* getRespsForCluster(const word* queryResult, uint erasurePos, word erasures);
