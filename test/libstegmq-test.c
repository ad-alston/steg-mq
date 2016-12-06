/**
 * Simple test for the libsteg-mq shared library.
 *
 * Aubrey Alston (ada2145@columbia.edu)
 */

#include <steg-mq/lib-steg-mq.h>
#include <stdio.h>

int main(int argc, char** argv){
	publish_stegotext("a","abcdefghijklmnopqrstuvwxyz012345");
	publish_stegotext("b","abcdefghijklmnopqrstuvwxyz012345");
	publish_stegotext("abc","abcdefghijklmnopqrstuvwxyz012345");
	publish_stegotext("b","abcdefghijklmnopqrstuvwxyz012345");
	publish_stegotext("a","abcdefghijklmnopqrstuvwxyz012345");

	char dest[16];
	int j;
	int result;
	for(j = 0; j < 2; j++){
		fprintf(stdout, "Consumption %d:\n", (j+1));
		result = consume_stegotext(dest, 16);
		
		if(result == 0){
			fprintf(stdout, "No stegotext retrieved.\n");
			continue;
		}		

		int k;
		for(k = 0; k < 16; k++){
			fprintf(stdout,"%02x ", (unsigned char) dest[k]);
		}
		fprintf(stdout, "\n");
	}
}
