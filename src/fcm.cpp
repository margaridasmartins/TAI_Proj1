#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>

#include "utils.hpp"

using namespace std;

struct state {
  map<char, int> occorrencies;
  int sum;
};

map<string, state> table;
set<char> symbols;
uint k, a, threshold = 5;
char line[100];

void print_table() {
  printf("%6s ", "");
  for (auto s : symbols) {
    string c;
    c += s;
    printf("%4s ", c != "\n" ? c.c_str() : "\\n");
  }
  printf("\n");

  for (auto t : table) {
    string c = replace_all(t.first, "\n", "\\n");
    printf("%6s ", c.c_str());
    for (auto s : symbols) {
      printf("%4d ", t.second.occorrencies[s]);
    }
    printf("\n");
  }
}

void init_table_arr(string context = "") {
  if (context.size() == k) {
    table[context] = {{}, 0};
    return;
  }
  for (auto s : symbols) {
    init_table_arr(context + s);
  }
}

void alphabet(FILE *fptr) {
  // estrutura com todos os simbolos
  while (fgets(line, 100, fptr) != NULL) {
    if (symbols.size() == 49) {  // ELIMINAR NA ENTREGA!!
      break;
    }
    for (auto c : line) {
      symbols.insert(c);
    }
  }
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
    exit(1);
  }

  alphabet(fptr);  // check the characters in the file
  rewind(fptr);  // move the file position pointer back to the start of the file

  double tablesize = (pow(symbols.size(), k + 1)) / 1024 / 1024;

  if (tablesize > threshold) {
    // TODO: init_table_map();
  } else {
    init_table_arr();
  }
  print_table();

  printf("Size of table: %f MB\n", tablesize);

  while (fgets(line, 100, fptr) != NULL) {
    // printf("The line is: %s\n", line);
  }

  fclose(fptr);

  return 0;
}
