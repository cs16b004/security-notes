
/* WARNING: Function: __i686.get_pc_thunk.bx replaced with injection: get_pc_thunk_bx */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
 /*
  * The encrypted/encoded strings used to compare the input
  * */
char DAT_00012008[20] = {0x62, 0x2f, 0x2a,0x23,0x45,0x69,0x63,0x69,0x69,0x7f,0x79,0x45,0x28,0x2a,0x28,0x29,0x00};

/**
 *Encrypted Fail message
 *
 * */
char DAT_00012051[20] = {0x4d, 0x68, 0x75, 0x74,0x7d, 0x3a,0x71,0x7f,0x63,0x34,0x00};

/**
 * THis function decrypts the string (param1) and returns the plain text
 * */
char* FUN_000111e9(char *param_1)

{
  char *pvVar1;
  int sVar2;
   int local_10;

  pvVar1 = calloc(0x100,1);
  sVar2 = strlen(param_1);
  for (local_10 = 0; local_10 < sVar2; local_10 = local_10 + 1) {
    /*Decrypt each character by XORing it with 0x1a*/
    *((char *)(local_10 + (long long int)pvVar1)) = param_1[local_10] ^ 0x1a;
  }
  /**  VERY IMP I put a printf statement here to get the decoded key,
   *   As this function is used by the function below to decode the key and 
   *   make comparison against the input
   */
  printf("%s\n",pvVar1);
  return pvVar1;
}
/*This functions compare the string Param1 with the decrypted key value 
 * uses the function above to decrypt and then permos a char by char comaprision
 * of the strings*/
int FUN_0001125a(char *param_1)

{
  int sVar1;
  char* iVar2;
  int uVar3;
  int local_10;

  sVar1 = strlen(param_1);
  iVar2 = FUN_000111e9(DAT_00012008);
  printf("%s",iVar2);
  if (sVar1 == 0x10) {
    for (local_10 = 0; local_10 < 0x10; local_10 = local_10 + 1) {
      if (*(char *)(local_10 + (long long int)iVar2) != param_1[local_10]) {
        return 0;
      }
    }
    uVar3 = 1;
  }
  else {
    uVar3 = 0;
  }
  return uVar3;
}
/*Junk function nver used in input or output*/
int FUN_000112df(int param_1)

{
  return param_1 + 10;
}

/*Main function from __libc_start_main*/

int main(int param_1,const char *param_2[])

{
  int uVar1;
  int iVar2;
  char local_11c [256];
  char *local_1c;
  int local_18;
  int local_14;
  /*
   *If the program is called with the input string 
   * */ 
  if (param_1 == 2) {
	  /*Copy the input from argv to local_1lc */
    strncpy(local_11c,(char *)param_2[1],0x100);
    local_14 = 0;
    /*JUNK CODE NOT RELEVANT*/
    // for (local_18 = 0; local_18 < 0x27a; local_18 = local_18 + 1) {
    //   iVar2 = rand();
    //   local_14 = local_14 + iVar2;
    // }
    // local_14 = FUN_000112df(local_14);

    /*Compare the input string local_1lc with the decrypted key, if equal print sucess else print message*/
    iVar2 = FUN_0001125a(local_11c);
    if (iVar2 == 1) {
      /*local_1c = (char *)FUN_000111e9(&DAT_0001202c);
      puts(local_1c);*/
      printf("We succceed\n");
    }
    else {
      local_1c = (char *)FUN_000111e9(DAT_00012051);
      puts(local_1c);
    }
  //  free(local_1c);
    uVar1 = 0;
  }
  else {
    printf("Usage: %s <key>\n",*param_2);
    uVar1 = 1;
  }
  return uVar1;
}
