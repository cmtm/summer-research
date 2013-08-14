#include "aam_driver.h"
#include <string.h>
#include <stdio.h>

int main(void) {
	init_driver();
	word myMessage[ASSOC_LENGTH] = {5,8,2,7 }; // 0, 0, 0, ...
	storeAssociation(myMessage);
	eraseMessage(myMessage, 1);
	eraseMessage(myMessage, 2);
	// message is now [5, X, X, 7]
	word* rslt = queryAssociation(myMessage);

	printf("\nresult is [%u, %u, %u, %u]\n\n", rslt[0], rslt[1], rslt[2], rslt[3]);

	printf("now lets try our getRespsForCluster function\n\n");

	word* respsForSecond = getRespsForCluster(rslt, 1, 2);
	//should produce a pointer 4 units greater than original rslt
	printf("%d should be equal to 3", respsForSecond - rslt);

}