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


int main() {
  printf("##########################\n");
  printf("PROGRAM START\n");
  printf("##########################\n");


  // printf("Enter a password to encrypt your file :\n");
  // scanf("%s", &key);

  FILE *test;
  test = fopen("test.txt", "rb");
  FILE *encryptedTest = fopen("test2.bmp", "wb");/*
  test = fopen("test.bmp", "rb");
  FILE *encryptedTest = fopen("test.txt", "wb");*/
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
      fputc(in ^ key[i % sizeof(key)], encryptedTest);
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
