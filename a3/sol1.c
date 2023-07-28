#include<stdio.h>
#include<string.h>
int main(){
	/* characters '9' and '5' are already decided by the corresponding to char byte as the 2nd and 4th position 
	 * of the input are fixed to 3 and 4*/
	char enc[32] = "5ESCKCAH";
	int n = strlen(enc);
	int i,j;
	/*Print code for '9' and '0' = (int)'9' - 4, (int)'0' - 4*/
	printf("053044");
	/*Iterate throught the rest of the characters '5', 'E' .... 'K' and print their respective code by subtracting 4 
	 * from their ascii value*/

	/*in fact ypu can keep adding the value of 256 to any of the j until j<1000 and the corresponding 
	 * number be still valid
	 * As there char conversion sees the byte value only*/
	for(i=0;i<n;i++){
		int iVar = enc[i] - '\x04';
		for(j=0;j<100;j++){
			//printf("----------------\n");
			if ((char)j == iVar) {
				printf("0%d",j);		
			}
			//printf("----------------\n");
		}
		//printf("code for %c is %d\n",enc[i],j);
	}
	return 0;
}
