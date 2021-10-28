#include "fcm.hpp"

Table::Table(uint k, float a) {
  this->k = k;
  this->a = a;
}

TableArr::TableArr(uint k, float a, map<char, uint> alphabet) : Table(k, a) {
  this->alphabet = alphabet;
  this->total = 0;
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
    table[i] = new uint[n+1]();  // create array with 0s
  }

  char context[k]; 

  // first k letters
  fgets(context, k, fptr);

  // k+1 letter
  char next_char=fgetc(fptr);
  do{
    
    table[get_index(context)][alphabet[next_char]+1]++;
    table[get_index(context)][0]++;
    this->total++;

    // slide one
    for(uint i =0; i<k-1;i++){
      context[i]= context[i+1];
    }

    context[k-1]= next_char;
    next_char=fgetc(fptr);

  }while (next_char != EOF );
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
        printf("%4d ", table[id][alphabet[pair.first]+1]);
      }
      printf("\n");
      continue;
    }

    for (auto pair : alphabet) {
      st.push(context + pair.first);
    }
  }
}

double TableArr::get_entropy(float a){
  double ent=0;
  uint r=pow(alphabet.size(),k);
  double contextEnt=0;
  double letterProb;
  for(uint id=0; id<r; id++){
    for (uint j=0;j<alphabet.size();j++) {
        if (table[id][0]!=0){
          letterProb=(float)(table[id][j+1]+a)/(float)(table[id][0]+a*alphabet.size());
          if (letterProb!=0){
            contextEnt-=letterProb*log(letterProb);
          }
        }
    }
    ent+=((float)table[id][0]/(float)this->total)*contextEnt;
    contextEnt=0;
  }
  return ent;
}

void TableArr::generate_text(char prior[]){
  return;
}
///////////////////////////////////////////////////////////////////////

TableHash::TableHash(uint k, float a, set<char> symbols) : Table(k, a) {
  this->symbols = symbols;
  this->total = 0;
}

void TableHash::train(FILE *fptr) {

  char context[k]; 

  // first k letters
  fgets(context, k+1, fptr);
  
  // k+1 letter
  char next_char=fgetc(fptr);
  do{
    
    if(table.find(context) != table.end()){
      table[context].occorrencies[next_char]++;
      table[context].sum ++;
      this->total++;
    }
    else{
      std::map<char, int> occ;
      occ[next_char]=1;
      state st = {  occ ,1};
      table[context]=st;
      this->total++;
    }
    
    // slide one
    for(uint i =0; i<k-1;i++){
      context[i]= context[i+1];
    }

    context[k-1]= next_char;
    next_char=fgetc(fptr);

  }while (next_char != EOF );
  
          
}

char TableHash::get_state(string context) {

  // if there is an entry in the hash table with such context
  if(table.find(context) != table.end()){

    auto it = table[context].occorrencies.begin();
    int max =0;
    char max_char;

    //see which is the most probable letter
    while (it!= table[context].occorrencies.end())
    {
      if(it->second>max){
        max=it->second;
        max_char= it->first;
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
    }
    printf("\n");
  }
}

double TableHash::get_entropy(float a){
  double ent=0;
  double contextEnt=0;
  double letterProb;
  for (auto pair:table){
    auto it=pair.second.occorrencies.begin();
    while(it != pair.second.occorrencies.end()){
      letterProb=((float)(*it).second+a)/((float)pair.second.sum+a*symbols.size());
      //printf("%f ",letterProb);
      if (letterProb!=0){
        contextEnt-=letterProb*log(letterProb);
      }
      it++;
    }
    //printf("%f\n",contextEnt);
    ent+=((float)pair.second.sum/(float)this->total)*contextEnt;
    contextEnt=0;
  }
  return ent;
}

void TableHash::generate_text(char prior[]){
  return;
}
///////////////////////////////////////////////////////////////////////

FCM::FCM(uint k, float a) {
  this->k = k;
  this->a = a;
}

void FCM::train(FILE *fptr) {
  // check the characters in the file
  uint id = 0;
  /*
  char line[100];
  while (fgets(line, 100, fptr) != NULL) {
    if (symbols.size() > 30) {  // TODO: ELIMINAR NA ENTREGA!!
      break;
    }
    for (auto c : line) {
      printf("%c",c);
      auto res = symbols.insert(c);
      if (res.second) {
        // new symbol
        alphabet[c] = id++;
      }
    }
  }
  */
  char c= fgetc( fptr);
  do{
    if (symbols.size() > 200) {  // TODO: ELIMINAR NA ENTREGA!!
      break;
    }
    auto res = symbols.insert(c);
    if (res.second) {
      // new symbol
      alphabet[c] = id++;
    }
    c= fgetc(fptr);
  }while (c !=EOF);

  rewind(fptr);  // move the file pointer back to the start of the file

  double tablesize = (pow(symbols.size(), k + 1)) / 1024 / 1024;
  printf("Size of table: %f MB\n", tablesize);

  if (tablesize < 0) { // Change this for hash/array table testing
    printf("Creating hash table...\n");
    table = new TableHash(k, a, symbols);
  } else {
    printf("Creating array table...\n");
    table = new TableArr(k, a, alphabet);
  }
  table->train(fptr);
}

char FCM::get_state(string context) { return table->get_state(context); }

double FCM::get_entropy(uint a) {
  return table->get_entropy(a);
}

void FCM::print_table() { table->print(); }

///////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("ERR: Incorrect number of arguments\n");
    exit(1);
  }

  uint k;
  float a;
  char filename[100];
  sprintf(filename, "../example/%s.txt", argv[1]);
  k = atoi(argv[2]);
  a = atof(argv[3]);

  FILE *fptr;

  if ((fptr = fopen(filename, "r")) == NULL) {
    printf("ERR: File not found\n");
    exit(1);
  }

  FCM *fcm = new FCM(k, a);
  fcm->train(fptr);
  fcm->print_table();
  printf("Entropy is:%4f\n",fcm->get_entropy(a));

  fclose(fptr);

  return 0;
}
