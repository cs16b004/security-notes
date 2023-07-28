#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

  char buf[1] = {'A'};
  char src[1] = {'\0'};

       /* Stry copy puts zero towards the end of destination even if the first byte in source is null byte */

   strcpy(buf, src);
   printf("new buf : %d\n", buf[0]);

  return 0;
}
