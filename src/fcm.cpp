#include "fcm.hpp"

Table::Table(uint k, uint a) {
  this->k = k;
  this->a = a;
}

TableArr::TableArr(uint k, uint a, map<char, uint> alphabet) : Table(k, a) {
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
  uint n = alphabet.size();
  uint r = pow(n, k);
  table = new uint *[r];
  for (uint i = 0; i < r; i++) {
    table[i] = new uint[n]();  // create array with 0s
  }

  // TODO: read file again and fill table
  // uint id = alphabet[c];
  // table[get_index(context)][id] = ...;
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
        printf("%4d ", table[id][alphabet[pair.first]]);
      }
      printf("\n");
      continue;
    }

    for (auto pair : alphabet) {
      st.push(context + pair.first);
    }
  }
}

TableHash::TableHash(uint k, uint a, set<char> symbols) : Table(k, a) {
  this->symbols = symbols;
}

void TableHash::train(FILE *fptr) {
  // TODO: read file again and fill table
}

char TableHash::get_state(string context) {
  // TODO:
  return 0;
}

void TableHash::print() {
  // print header
  printf("%6s ", "");
  for (auto s : symbols) {
    string c;
    c += s;
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
      if (*it1 == (*it2).first) {
        printf("%4d ", (*it2).second);
        it1++;
        it2++;
      } else {
        // place 0 for characters not saved
        printf("%4d ", 0);
        it1++;
      }
      printf("\n");
    }
  }
}

FCM::FCM(uint k, uint a) {
  this->k = k;
  this->a = a;
}

void FCM::train(FILE *fptr) {
  // check the characters in the file
  uint id = 0;
  char line[100];
  while (fgets(line, 100, fptr) != NULL) {
    if (symbols.size() > 20) {  // TODO: ELIMINAR NA ENTREGA!!
      break;
    }
    for (auto c : line) {
      auto res = symbols.insert(c);
      if (res.second) {
        // new symbol
        alphabet[c] = id++;
      }
    }
  }
  rewind(fptr);  // move the file pointer back to the start of the file

  double tablesize = (pow(symbols.size(), k + 1)) / 1024 / 1024;
  printf("Size of table: %f MB\n", tablesize);

  if (tablesize > 8000) {
    printf("Creating hash table...\n");
    table = new TableHash(k, a, symbols);
  } else {
    printf("Creating array table...\n");
    table = new TableArr(k, a, alphabet);
  }
  table->train(fptr);
}

char FCM::get_state(string context) { return table->get_state(context); }

double FCM::get_entropy() {
  // TODO:
  return 0;
}

void FCM::print_table() { table->print(); }

int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("ERR: Incorrect number of arguments\n");
    exit(1);
  }

  uint k, a;
  char filename[100];
  sprintf(filename, "../example/%s.txt", argv[1]);
  k = atoi(argv[2]);
  a = atoi(argv[3]);

  FILE *fptr;

  if ((fptr = fopen(filename, "r")) == NULL) {
    printf("ERR: File not found\n");
    exit(1);
  }

  FCM *fcm = new FCM(k, a);
  fcm->train(fptr);
  fcm->print_table();

  fclose(fptr);

  return 0;
}
