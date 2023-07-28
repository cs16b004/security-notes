#include<stdio.h>
#include<string.h>

/*
 * Function called after a wrong input (prints fail message)
 *
 * Although int the crackme1 binary pointer to the location is used I just copied the string al together
 *
 * */
int FUN_00011209(){
   printf("Wrong Input\n");
   return 0;
}
/*
 *
 * The decomplied main function referenced in __libc_start_main function.
 * */
int main(){
  size_t sVar1; // holds the length of as the input number is 
		// encoded character by character, used to break
		// from the while loop
  size_t iVar2; // Hold the intermediate atoi value after reading digits from the input
  char local_6b; // first digit passed to atoi 
  char local_6a; // second digit passed to atoi
  char local_69; // third and final digit passed to atoi
  char local_67 [64]; // Hold the input scanf call ()
  char local_27 [11]; //hold the encoded Input
  int local_1c; // Hold the scanf return not important
  int local_18;  // Counter used to iterarte over the input
  int local_14; // counter used to iterarate over atoi digit
  int local_68; // Most likely used to set the boundary for atoi.
  
  printf("Address of three digits %p, %p,%p \n",&local_6b,&local_6a, &local_69);
  printf("Enter the right number: ");
  local_1c =scanf("%32s",local_67);
  if (local_1c != 1) {
    FUN_00011209();
  }
  /*Check if the third digit is '3'  in the input*/
  if (local_67[2] != '3') {
    FUN_00011209(); // Print wrong input and exit
  }
  /*Check if the 5th digit is '4' in the input */
  if (local_67[4] != '4') {
    FUN_00011209(); // print wrong input and exit
  }
  printf("here -- char  = %c\n",'9'-'\x04');  // Igonre the printfs I used it to see how the program works.
 // memset(local_27,0,0xb); // memset so that encoded input is initalized with som garbage value and the strlen works.
  local_68 = 0; // initlize all the counters
  local_14 = 0;
  local_18 = 0;
  int i=0;
  // The target String which will be used to compare methods.
  char a[32] = "905ESCKCAH";
  while( 1 ) {
    sVar1 = strlen(local_27);
    // If the the input number has been encoded to a strlen(target string) break and compare with target string.
    if (9 < sVar1) break;
    // If the input number ends while encoding before the length of target string is reached , break and compare.
    sVar1 = strlen(local_67);
    if (sVar1 <= local_14) break;

    // Set the first digit for atoi 
    local_6b = local_67[local_14];
    //set the second second digit for atoi
    local_6a = local_67[local_14 + 1];
    // set the third digit fpr atoi
    local_69 = local_67[local_14 + 2];
    iVar2 = atoi(&local_6b);
    printf("Atoi val is: %d\n",iVar2);

    local_27[local_18] = (char)iVar2;
    // Add the the offset to the atoi valu and put the value as encoded character 
    local_27[local_18] = local_27[local_18] + '\x04';
    local_14 = local_14 + 3; // Check the next 3 digits
    local_18 = local_18 + 1; // Increment the counter to put the next encoded character
    printf("Encoded character for input string in iter %d: %c\n",i,local_27[local_18-1]);
    i+=1;
    printf("Encoded String : %s \n\n", local_27);
  }
  // End the result string with '\0' so that strcmp works
  local_27[local_18] = '\0';
  // Compare the the encoded input with target and print success or fail message
  iVar2 = strcmp(local_27,"905ESCKCAH");
  if (iVar2 == 0) {
    printf("Your number corresponds to %s, well done!\n",local_27);
  }
  else {
    FUN_00011209();
  }
  return 0;
}
