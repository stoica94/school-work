#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

int send_msg(msg *t) {
	int res;
	/* cleanup msg */
	memset(t, 0, sizeof(msg));

	/* gonna send an empty msg */
	t->len = MSGSIZE;

	/* send msg */
	res = send_message(t);
	if (res < 0) {
		perror("[SENDER] Send error. Exiting.\n");
		return -1;
	}
	return res;
}


int main(int argc, char *argv[])
{
	msg t;
	int i, res;
	int window;

	//window = atoi(argv[1]) * 1000 / (8 * MSGSIZE);
	window = COUNT / 2;
	
	printf("[SENDER] Starting.\n");	
	init(HOST, PORT);
	if(window > COUNT) {
		window = COUNT;
	}

	printf("WINDOW %d\n", window);
	printf("[SENDER]: BDP=%d\n", atoi(argv[1])); 
	
	for (i = 0; i < window; i++) {
		res = send_msg(&t);
	}

	for (i = 0; i < COUNT - window; i++) {
		/* wait for ACK */
		res = recv_message(&t);
		if (res < 0) {
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		}
		res = send_msg(&t);
	}

	for(i = 0; i < window; i++) {
		/* wait for ACK */
		res = recv_message(&t);
		if (res < 0) {
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		}
	}
	printf("[SENDER] Job done, all sent.\n");
		
	return 0;
}
