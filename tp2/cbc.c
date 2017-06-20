#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ITERATIONS 3
#define MASKLENGTH 8

unsigned char key[] = "password";

void printCharInBits(char input) {
  printf("PRINTING (in bits) : %c\n", input);
  int i;
  for(i=0;i<8;i++){
      printf("%d", (input & (1 << i)) >> i);
      if(i==7) {
        printf("\n");
      }
    }
}

char encryptDecryptChar(char input) {
	return input ^ key[0];
}


int main() {
  printf("##########################\n");
  printf("PROGRAM START\n");
  printf("##########################\n");


  // printf("Enter a password to encrypt your file :\n");
  // scanf("%s", &key);
  char input = 'a';

  FILE *test;
  test = fopen("encryptedTest.txt", "rb");
  FILE *encryptedTest = fopen("encryptedTest2.bmp", "wb");
  if(test==NULL) {
    printf("NOTHING IN FILE\n");
  } else {
    printf("FILE LOADED\n");

    unsigned char out[sizeof(test)];
    int car;
    int i = 0;
    while((car = fgetc(test)) != EOF)
    {
      char in = car;
      fputc(encryptDecryptChar(in), encryptedTest);
        printf("%d\n", i);
        i++;
    }
  }


  fclose(test);
  fclose(encryptedTest);

  printf("##########################\n");
  printf("PROGRAM END\n");
  printf("##########################\n");
  return 0;
}
