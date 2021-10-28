#include "fcm.hpp"

Table::Table(uint k, float a)
{
  this->k = k;
  this->a = a;
}

TableArr::TableArr(uint k, float a, map<char, uint> alphabet) : Table(k, a)
{
  this->alphabet = alphabet;
  this->total = 0;
}

uint TableArr::get_index(string context)
{
  // for k=3, index = id3 * n**2 + id2 * n + id1
  uint index = 0, n = 1;
  for (auto c : context)
  {
    uint id = alphabet[c];
    index += id * n;
    n *= alphabet.size();
  }
  return index;
};

void TableArr::train(FILE *fptr)
{
  uint n = alphabet.size();
  uint r = pow(n, k);
  table = new uint *[r];
  for (uint i = 0; i < r; i++)
  {
    table[i] = new uint[n + 1](); // create array with 0s
  }

  char context[k];

  // first k letters
  fgets(context, k, fptr);

  // k+1 letter
  char next_char = fgetc(fptr);
  do
  {

    table[get_index(context)][alphabet[next_char] + 1]++;
    table[get_index(context)][0]++;
    this->total++;

    // slide one
    for (uint i = 0; i < k - 1; i++)
    {
      context[i] = context[i + 1];
    }

    context[k - 1] = next_char;
    next_char = fgetc(fptr);

  } while (next_char != EOF);
}

char TableArr::get_state(string context)
{
  // TODO:
  return 0;
}

void TableArr::print()
{
  // print header
  printf("%6s ", ".    .");
  for (auto pair : alphabet)
  {
    string c;
    c += pair.first;
    c = replace_all(c, "\n", "\\n");
    printf("%4s ", c.c_str());
  }
  printf("\n");

  // print rows from all permutations
  stack<string> st;
  st.push("");
  while (!st.empty())
  {
    string context = st.top();
    st.pop();

    if (context.size() == k)
    {
      string c = replace_all(context, "\n", "\\n");
      printf("%6s ", c.c_str());

      uint id = get_index(context);
      for (auto pair : alphabet)
      {
        printf("%4d ", table[id][alphabet[pair.first] + 1]);
      }
      printf("\n");
      continue;
    }

    for (auto pair : alphabet)
    {
      st.push(context + pair.first);
    }
  }
}

double TableArr::get_entropy(float a)
{
  return 0;
}

void TableArr::generate_text(char prior[] = NULL)
{
  char context[k];
  float random;
  float prob;
  uint id;
  auto it=alphabet.begin();
  /*for(uint i=0; i<k;i++){
    context[i]='l';
  }*/

  if (prior == NULL)
  {
    //create random context
    do
    {
      for (uint i = 0; i < k; i++)
      {
        advance(it, rand() % alphabet.size());
        context[i] = (*it).first;
        it = alphabet.begin();
      }
    } while (table[get_index(context)][0] == 0);
    printf("%s\n", context);
  }

  printf("%s", prior);
  //write 1000 characters
  for (uint i = 0; i < 1000; i++)
  {
    prob = 0;
    id = get_index(prior);
    random = (float)rand() / (float)RAND_MAX; //generate target probability
    int sum = 0;

    for (auto pair : alphabet)
    {
      sum += table[id][alphabet[pair.first]];
    }

    if (sum != 0)
    {
      for (auto pair : alphabet)
      {
        prob += (float)(table[id][alphabet[pair.first]]) / (float)(sum);
        printf("%f", prob);
        if (prob > random)
        {
          printf("%c", pair.first);
          for (uint j = 0; j < k - 1; j++)
          {
            prior[j] = prior[j + 1];
          }
          prior[k - 1] = pair.first;
          break;
        }
      }
    }
    else
    {
      //put random char in front of context
      advance(it, rand() % alphabet.size());
      printf("%c", (*it).first);
      for (uint j = 0; j < k - 1; j++)
      {
        prior[j] = prior[j + 1];
      }
      prior[k - 1] = (*it).first;
      it = alphabet.begin();
    }
  }
  printf("\n");
}

///////////////////////////////////////////////////////////////////////

TableHash::TableHash(uint k, float a, set<char> symbols) : Table(k, a)
{
  this->symbols = symbols;
  this->total = 0;
}

void TableHash::train(FILE *fptr)
{

  char context[k];

  // first k letters
  fgets(context, k + 1, fptr);

  // k+1 letter
  char next_char = fgetc(fptr);
  do
  {

    if (table.find(context) != table.end())
    {
      table[context].occorrencies[next_char]++;
      table[context].sum++;
      this->total++;
    }
    else
    {
      std::map<char, int> occ;
      occ[next_char]=1;
      state st = {  occ ,1};
      table[context]=st;
      this->total++;
    }

    // slide one
    for (uint i = 0; i < k - 1; i++)
    {
      context[i] = context[i + 1];
    }

    context[k - 1] = next_char;
    next_char = fgetc(fptr);

  } while (next_char != EOF);
}

char TableHash::get_state(string context)
{

  // if there is an entry in the hash table with such context
  if (table.find(context) != table.end())
  {

    auto it = table[context].occorrencies.begin();
    int max = 0;
    char max_char;

    //see which is the most probable letter
    while (it != table[context].occorrencies.end())
    {
      if (it->second > max)
      {
        max = it->second;
        max_char = it->first;
      }
      it++;
    }
    return max_char;
  }
  return 0;
}

void TableHash::print()
{
  // print header
  printf("%6s ", "");
  for (auto s : symbols)
  {
    string c;
    c += s;
    c = replace_all(c, "\n", "\\n");
    printf("%4s ", c.c_str());
  }
  printf("\n");

  // print rows from all existant contexts
  for (auto pair : table)
  {

    string c = replace_all(pair.first, "\n", "\\n");
    printf("%6s ", c.c_str());

    auto it1 = symbols.begin();
    auto it2 = pair.second.occorrencies.begin();

    while (it1 != symbols.end())
    {
      if (*it1 == (*it2).first)
      {
        printf("%4d ", (*it2).second);
        it1++;
        it2++;
      }
      else
      {
        // place 0 for characters not saved
        printf("%4d ", 0);
        it1++;
      }
    }
    printf("\n");
  }
}

double TableHash::get_entropy(float a)
{
  return 0;
}

void TableHash::generate_text(char prior[] = NULL)
{
  char context[k];
  float random;
  float prob;
  auto it=symbols.begin();
    
  if (prior == NULL)
  {
    //create random context
    do
    {
      for (uint i = 0; i < k; i++)
      {
        advance(it, rand() % symbols.size());
        context[i] = (*it);
        it = symbols.begin();
      }
    } while (table.find(context) == table.end());
    printf("%s\n", context);
  }

  //write 1000 characters
  printf("%s", prior);
  for (uint i = 0; i < 1000; i++)
  {
    prob = 0;
    random = (float)rand() / (float)RAND_MAX; //generate target probability

    if (table.find(prior) != table.end())
    {
      for (auto pair : table[prior].occorrencies)
      {
        prob += (float)(pair.second + a) / (float)(table[prior].sum + (a * symbols.size()));
        if (prob > random)
        {
          printf("%c", pair.first);
          for (uint j = 0; j < k - 1; j++)
          {
            prior[j] = prior[j + 1];
          }
          prior[k - 1] = pair.first;
          break;
        }
      }
    }
    else
    {
      //put random char in front of context
      advance(it, rand() % symbols.size());
      printf("%c", (*it));
      for (uint j = 0; j < k - 1; j++)
      {
        prior[j] = prior[j + 1];
      }
      prior[k - 1] = (*it);
      it = symbols.begin();
    }
  }
  printf("\n");
}

///////////////////////////////////////////////////////////////////////

FCM::FCM(uint k, float a)
{
  this->k = k;
  this->a = a;
}

void FCM::train(FILE *fptr)
{
  // check the characters in the file
  uint id = 0;

  char c = fgetc(fptr);
  do
  {
    if (symbols.size() > 200)
    { // TODO: ELIMINAR NA ENTREGA!!
      break;
    }
    auto res = symbols.insert(c);
    if (res.second)
    {
      // new symbol
      alphabet[c] = id++;
    }
    c = fgetc(fptr);
  } while (c != EOF);

  rewind(fptr); // move the file pointer back to the start of the file

  double tablesize = (pow(symbols.size(), k + 1)) / 1024 / 1024;
  printf("Size of table: %f MB\n", tablesize);

  if (tablesize > 0)
  { // Change this for hash/array table testing
    printf("Creating hash table...\n");
    table = new TableHash(k, a, symbols);
  }
  else
  {
    printf("Creating array table...\n");
    table = new TableArr(k, a, alphabet);
  }
  table->train(fptr);
}

char FCM::get_state(string context) { return table->get_state(context); }

void FCM::generate_text(char prior[] = NULL)
{
  table->generate_text(prior);
}

void FCM::print_table() { table->print(); }

///////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  if (argc != 4 && argc != 5)
  {
    printf("ERR: Incorrect number of arguments\n");
    exit(1);
  }

  uint k, a;
  char filename[100];
  sprintf(filename, "../example/%s.txt", argv[1]);
  k = atoi(argv[2]);
  a = atoi(argv[3]);

  FILE *fptr;

  if ((fptr = fopen(filename, "r")) == NULL)
  {
    printf("ERR: File not found\n");
    exit(1);
  }

  FCM *fcm = new FCM(k, a);
  fcm->train(fptr);
  fcm->print_table();

  if (argc == 5)
  {
    fcm->generate_text(argv[4]);
  }
  else
  {
    fcm->generate_text();
  }

  fclose(fptr);

  return 0;
}

/*


TODO:
Refactoring
Possibilidade de gerar prior
Passar tamanho de texto a gerar como argumento
Por o Array a dar 
Graficos 

*/