/**
 * Simple test for the libsteg-mq shared library.
 *
 * Aubrey Alston (ada2145@columbia.edu)
 */

#include <steg-mq/lib-steg-mq.h>

int main(int argc, char** argv){
	publish_stegotext("a","b");
}
