#ifndef _FCM_HPP_
#define _FCM_HPP_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <utility>

#include "utils.hpp"

using namespace std;

struct state {
  map<char, int> occorrencies;
  int sum;
};

class Table {
 protected:
  uint k, a;

 public:
  Table(uint k, float a);

  virtual void train(FILE *fptr) = 0;
  virtual char get_state(string context) = 0;
  virtual double get_entropy(float a) = 0;
  virtual void generate_text(char prior[]) = 0;
  virtual void print() = 0;
};

class TableArr : public Table {
 private:
  uint **table, total;
  map<char, uint> alphabet;

 public:
  TableArr(uint k, float a, map<char, uint> alphabet);

  uint get_index(string context);
  void train(FILE *fptr);
  char get_state(string context);
  double get_entropy(float a);
  void generate_text(char prior[]);
  void print();
};

class TableHash : public Table {
 private:
  map<string, state> table;
  set<char> symbols;
  uint total;

 public:
  TableHash(uint k, float a, set<char> symbols);

  void train(FILE *fptr);
  char get_state(string context);
  double get_entropy(float a);
  void generate_text(char prior[]);
  void print();
};

class FCM {
 private:
  uint k;
  float a;
  Table *table;
  set<char> symbols;
  map<char, uint> alphabet;

 public:
  FCM(uint k, float a);

  void train(FILE *fptr);
  char get_state(string context);
  double get_entropy(uint a);
  void generate_text(char prior[]);
  void print_table();
};

#endif
