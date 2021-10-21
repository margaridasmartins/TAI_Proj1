#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iterator>
#include <map>

using namespace std;


struct state {
  map<string, int>::iterator occorrencies;
  int sum;
};

map<string, state>;



void read_file() {
    // estrutura com todos os simbolos
}


int main() {

    // ler k e alpha do comando, e ficheiro


  int num;
  FILE *fptr;

  if ((fptr = fopen("../example/example.txt", "r")) == NULL) {
    printf("Error! opening file");

    // Program exits if the file pointer returns NULL.
    exit(1);
  }



  char line[100];
  while (fgets(line, 100, fptr) != NULL) {
    printf("The line is: %s\n", line);
  }

  fclose(fptr);

  return 0;
}
