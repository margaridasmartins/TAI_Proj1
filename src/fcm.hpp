#ifndef _FCM_HPP_
#define _FCM_HPP_

#include <assert.h>
#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <utility>

#include "utils.hpp"

using namespace std;

struct state {
  map<char, int> occorrencies;
  uint sum;
};

class Table {
 protected:
  uint k;
  map<char, uint> symbols;  // overall symbol frequency
  uint total;               // number of symbols in the file

 public:
  Table(uint k, map<char, uint> symbols);

  virtual void train(FILE *fptr) = 0;
  virtual char get_state(string context) = 0;
  virtual double get_entropy(float a) = 0;
  virtual void generate_text(float a, char prior[], uint text_size,
                             bool relative_random, bool show_random) = 0;
  virtual void print() = 0;
};

class TableArr : public Table {
 private:
  uint **table, total;
  map<char, uint> alphabet;  // symbols ID

 public:
  TableArr(uint k, map<char, uint> symbols, map<char, uint> alphabet);

  uint get_index(string context);
  void train(FILE *fptr);
  char get_state(string context);
  double get_entropy(float a);
  void generate_text(float a, char prior[], uint text_size,
                     bool relative_random, bool show_random);
  void print();
};

class TableHash : public Table {
 private:
  map<string, state> table;

 public:
  TableHash(uint k, map<char, uint> symbols);

  void train(FILE *fptr);
  char get_state(string context);
  double get_entropy(float a);
  void generate_text(float a, char prior[], uint text_size,
                     bool relative_random, bool show_random);
  void print();
};

class FCM {
 private:
  uint k;
  Table *table;
  map<char, uint> symbols;
  map<char, uint> alphabet;

 public:
  FCM(uint k);

  void train(FILE *fptr, float threshold);
  char get_state(string context);
  double get_entropy(float a);
  void generate_text(float a, char prior[], uint text_size,
                     bool relative_random, bool show_random);
  void print_table();
};

Table::Table(uint k, map<char, uint> symbols) {
  this->k = k;
  this->symbols = symbols;
  this->total = 0;
}

TableArr::TableArr(uint k, map<char, uint> symbols, map<char, uint> alphabet)
    : Table(k, symbols) {
  this->alphabet = alphabet;
}

uint TableArr::get_index(string context) {
  // for k=3, index = id3 * n**2 + id2 * n + id1
  uint index = 0, n = 1;
  for (auto c : context) {
    uint id = alphabet[c];
    index += id * n;
    n *= alphabet.size();
  }
  return index;
};

void TableArr::train(FILE *fptr) {
  rewind(fptr);

  uint n = alphabet.size();
  uint r = pow(n, k);
  table = new uint *[r];
  for (uint i = 0; i < r; i++) {
    table[i] = new uint[n + 1]();  // create array with 0s
  }

  char context[k];

  // first k letters
  fgets(context, k + 1, fptr);

  // k+1 letter
  char next_char = fgetc(fptr);
  do {
    symbols[next_char]++;
    this->total++;

    table[get_index(context)][alphabet[next_char] + 1]++;
    table[get_index(context)][0]++;

    // slide one
    for (uint i = 0; i < k - 1; i++) {
      context[i] = context[i + 1];
    }
    context[k - 1] = next_char;

    next_char = fgetc(fptr);

  } while (next_char != EOF);
}

char TableArr::get_state(string context) {
  // TODO:
  return 0;
}

void TableArr::print() {
  // print header
  printf("%6s ", ".    .");
  for (auto pair : alphabet) {
    string c;
    c += pair.first;
    c = replace_all(c, "\n", "\\n");
    printf("%4s ", c.c_str());
  }
  printf("\n");

  // print rows from all permutations
  stack<string> st;
  st.push("");
  while (!st.empty()) {
    string context = st.top();
    st.pop();

    if (context.size() == k) {
      string c = replace_all(context, "\n", "\\n");
      printf("%6s ", c.c_str());

      uint id = get_index(context);
      for (auto pair : alphabet) {
        printf("%4d ", table[id][alphabet[pair.first] + 1]);
      }
      printf("\n");
      continue;
    }

    for (auto pair : alphabet) {
      st.push(context + pair.first);
    }
  }
}

double TableArr::get_entropy(float a) {
  assert(a > 0);
  double ent = 0;
  uint r = pow(alphabet.size(), k);
  double contextEnt = 0;
  double letterProb;
  for (uint id = 0; id < r; id++) {
    for (uint j = 0; j < alphabet.size(); j++) {
      letterProb = (double)(table[id][j + 1] + a) /
                   (table[id][0] + a * alphabet.size());
      contextEnt -= letterProb * log2(letterProb);
    }
    ent += (double)table[id][0] / this->total * contextEnt;
    contextEnt = 0;
  }
  return ent;
}

void TableArr::generate_text(float a, char *prior, uint text_size,
                             bool relative_random, bool show_random) {
  assert(a >= 0);
  char context[k];
  float random;
  float prob;
  uint id;

  if (prior[0] == 0) {
    // create random prior
    for (uint i = 0; i < k; i++) {
      auto it = symbols.begin();
      advance(it, rand() % symbols.size());
      prior[i] = (*it).first;
    }
  }

  printf("\n\33[4m%s\33[0m", prior);
  // write 1000 characters
  for (uint i = 0; i < text_size; i++) {
    prob = 0;
    id = get_index(prior);
    random = (float)rand() / RAND_MAX;  // generate target probability
    int sum = 0;

    for (auto pair : alphabet) {
      sum += table[id][alphabet[pair.first] + 1];
    }

    if (sum != 0) {
      for (auto pair : alphabet) {
        prob += (float)(table[id][alphabet[pair.first] + 1] + a) /
                (sum + a * alphabet.size());

        if (prob > random) {
          printf("%c", pair.first);
          for (uint j = 0; j < k - 1; j++) {
            prior[j] = prior[j + 1];
          }
          prior[k - 1] = pair.first;
          break;
        }
      }
    } else {
      char rand_char;
      if (relative_random) {
        // put a random char relative to the letter occorrency
        for (auto pair : symbols) {
          prob += (float)pair.second / this->total;
          if (prob > random) {
            rand_char = pair.first;
            break;
          }
        }
      } else {
        // put a random char uniformly distributed
        auto it = symbols.begin();
        advance(it, rand() % symbols.size());
        rand_char = (*it).first;
      }

      if (show_random)
        printf("\33[34m%c\33[0m", rand_char);
      else
        printf("%c", rand_char);

      for (uint j = 0; j < k - 1; j++) {
        prior[j] = prior[j + 1];
      }
      prior[k - 1] = rand_char;
    }
  }
  printf("\n");
}

///////////////////////////////////////////////////////////////////////

TableHash::TableHash(uint k, map<char, uint> symbols) : Table(k, symbols) {}

void TableHash::train(FILE *fptr) {
  rewind(fptr);
  table.clear();

  char context[k];

  // first k letters
  fgets(context, k + 1, fptr);

  // k+1 letter
  char next_char = fgetc(fptr);
  do {
    symbols[next_char]++;
    this->total++;

    if (table.find(context) != table.end()) {
      table[context].occorrencies[next_char]++;
      table[context].sum++;
    } else {
      map<char, int> occ;
      occ[next_char] = 1;
      state st = {occ, 1};
      table[context] = st;
    }

    // slide one
    for (uint i = 0; i < k - 1; i++) {
      context[i] = context[i + 1];
    }
    context[k - 1] = next_char;

    next_char = fgetc(fptr);

  } while (next_char != EOF);
}

char TableHash::get_state(string context) {
  // if there is an entry in the hash table with such context
  if (table.find(context) != table.end()) {
    auto it = table[context].occorrencies.begin();
    int max = 0;
    char max_char;

    // see which is the most probable letter
    while (it != table[context].occorrencies.end()) {
      if (it->second > max) {
        max = it->second;
        max_char = it->first;
      }
      it++;
    }
    return max_char;
  }
  return 0;
}

void TableHash::print() {
  // print header
  printf("%6s ", "");
  for (auto s : symbols) {
    string c;
    c += s.first;
    c = replace_all(c, "\n", "\\n");
    printf("%4s ", c.c_str());
  }
  printf("\n");

  // print rows from all existant contexts
  for (auto pair : table) {
    string c = replace_all(pair.first, "\n", "\\n");
    printf("%6s ", c.c_str());

    auto it1 = symbols.begin();
    auto it2 = pair.second.occorrencies.begin();

    while (it1 != symbols.end()) {
      if ((*it1).first == (*it2).first) {
        printf("%4d ", (*it2).second);
        it1++;
        it2++;
      } else {
        // place 0 for characters not saved
        printf("%4d ", 0);
        it1++;
      }
    }
    printf("\n");
  }
}

double TableHash::get_entropy(float a) {
  assert(a >= 0);
  double ent = 0;
  double contextEnt = 0;
  double letterProb;
  for (auto pair : table) {
    auto it = pair.second.occorrencies.begin();
    while (it != pair.second.occorrencies.end()) {
      letterProb = (double)((*it).second + a) / 
                   (pair.second.sum + a * symbols.size());
      contextEnt -= letterProb * log2(letterProb);
      it++;
    }
    // letters not present in occorrencies
    int n = symbols.size() - pair.second.occorrencies.size();
    letterProb = (double)a / (pair.second.sum + a * symbols.size());
    contextEnt -= (letterProb * log2(letterProb)) * n;

    ent += (double)(pair.second.sum + a) /
           (this->total + a * pow(symbols.size(), k)) * contextEnt;
    contextEnt = 0;
  }
  // contexts not present in table
  double n = pow(symbols.size(), k) - table.size();
  letterProb = (double)1 / symbols.size();
  contextEnt = -(letterProb * log2(letterProb)) * symbols.size();

  ent += (double)a * symbols.size() / 
         (this->total + a * symbols.size() * pow(symbols.size(), k)) * contextEnt * n;

  return ent;
}

void TableHash::generate_text(float a, char *prior, uint text_size,
                              bool relative_random, bool show_random) {
  assert(a >= 0);
  float random;
  float prob;

  if (prior[0] == 0) {
    // create random prior
    for (uint i = 0; i < k; i++) {
      auto it = symbols.begin();
      advance(it, rand() % symbols.size());
      prior[i] = (*it).first;
    }
  }

  printf("\n\33[4m%s\33[0m", prior);
  // write 1000 characters
  for (uint i = 0; i < text_size; i++) {
    prob = 0;
    random = (float)rand() / RAND_MAX;  // generate target probability

    if (table.find(prior) != table.end()) {
      for (auto pair : symbols) {
        if (table[prior].occorrencies.find(pair.first) !=
            table[prior].occorrencies.end()) {
          // symbol registed on map
          prob += (double)(table[prior].occorrencies[pair.first] + a) /
                  (table[prior].sum + a * symbols.size());
        } else {
          // symbol not found
          prob += (double)a / (table[prior].sum + a * symbols.size());
        }

        if (prob > random) {
          printf("%c", pair.first);
          for (uint j = 0; j < k - 1; j++) {
            prior[j] = prior[j + 1];
          }
          prior[k - 1] = pair.first;
          break;
        }
      }
    } else {
      char rand_char;
      if (relative_random) {
        // put a random char relative to the letter occorrency
        for (auto pair : symbols) {
          prob += (float)pair.second / this->total;
          if (prob > random) {
            rand_char = pair.first;
            break;
          }
        }
      } else {
        // put a random char uniformly distributed
        auto it = symbols.begin();
        advance(it, rand() % symbols.size());
        rand_char = (*it).first;
      }

      if (show_random)
        printf("\33[34m%c\33[0m", rand_char);
      else
        printf("%c", rand_char);

      for (uint j = 0; j < k - 1; j++) {
        prior[j] = prior[j + 1];
      }
      prior[k - 1] = rand_char;
    }
  }
  printf("\n");
}

///////////////////////////////////////////////////////////////////////

FCM::FCM(uint k) { this->k = k; }

void FCM::train(FILE *fptr, float threshold = 0) {
  rewind(fptr);  // move the file pointer back to the start of the file
  symbols.clear();
  alphabet.clear();

  // check the characters in the file
  uint id = 0;
  char c = fgetc(fptr);
  do {
    if (symbols.find(c) == symbols.end()) {
      // new symbol
      symbols[c] = 0;
      alphabet[c] = id++;
    }
    c = fgetc(fptr);
  } while (c != EOF);

  double tablesize = (pow(symbols.size(), k + 1)) / 1024 / 1024;
  printf("Size of table: %f MB\n", tablesize);

  if (tablesize > threshold) {
    printf("Creating hash table...\n");
    table = new TableHash(k, symbols);
  } else {
    printf("Creating array table...\n");
    table = new TableArr(k, symbols, alphabet);
  }
  table->train(fptr);
}

char FCM::get_state(string context) { return table->get_state(context); }

double FCM::get_entropy(float a) { return table->get_entropy(a); }

void FCM::print_table() { table->print(); }

void FCM::generate_text(float a, char prior[], uint text_size,
                        bool relative_random, bool show_random) {
  table->generate_text(a, prior, text_size, relative_random, show_random);
}

#endif
