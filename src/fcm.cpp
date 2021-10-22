#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <set>

using namespace std;

struct state {
  map<char, int> occorrencies; // no need for ::iterator here
  int sum;
};

map<string, state> table;
set<char> symbols;
uint k, a;

void print_table() {
  printf("%6s ", "");
  for (auto s : symbols) {
    printf("%4c ", s);
  }
  printf("\n");

  for (auto t : table) {
    printf("%6s ", t.first.c_str());
    for (auto s : symbols) {
      printf("%4d ", t.second.occorrencies[s]);
    }
    printf("\n");
  }
}

void init_table(string context = "") {
  if (context.size() == k) {
    table[context] = {{}, 0};
    return;
  }
  for (auto s : symbols) {
    init_table(context + s);
  }
}

void read_file() {
  // estrutura com todos os simbolos
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("ERR: Incorrect number of arguments\n");
    exit(1);
  }

  char filename[100];
  sprintf(filename, "../example/%s.txt", argv[1]);
  k = atoi(argv[2]);
  a = atoi(argv[3]);

  FILE *fptr;

  if ((fptr = fopen(filename, "r")) == NULL) {
    printf("ERR: File not found\n");

    // Program exits if the file pointer returns NULL.
    exit(1);
  }

  char line[100];
  while (fgets(line, 100, fptr) != NULL) {
    printf("The line is: %s\n", line);
  }

  fclose(fptr);

  symbols = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};  // test example

  init_table();
  print_table();

  return 0;
}
