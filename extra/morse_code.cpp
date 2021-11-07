#include <fstream>
#include <iostream>
#include <map>
#include <string>

using namespace std;

map<char, string> ascii_to_morse = {
    {'a', ".-"},     {'b', "-..."},   {'c', "-.-."},    {'d', "-.."},
    {'e', "."},      {'f', "..-."},   {'g', "--."},     {'h', "...."},
    {'i', ".."},     {'j', ".---"},   {'k', "-.-"},     {'l', ".-.."},
    {'m', "--"},     {'n', "-."},     {'o', "---"},     {'p', ".--."},
    {'q', "--.-"},   {'r', ".-."},    {'s', "..."},     {'t', "-"},
    {'u', "..-"},    {'v', "...-"},   {'w', ".--"},     {'x', "-..-"},
    {'y', "-.--"},   {'z', "--.."},   {'0', "-----"},   {'1', ".----"},
    {'2', "..---"},  {'3', "...--"},  {'4', "....-"},   {'5', "....."},
    {'6', "-...."},  {'7', "--..."},  {'8', "---.."},   {'9', "----."},
    {'.', ".-.-.-"}, {',', "--..--"}, {':', "---..."},  {';', "..-.-"},
    {'?', "..--.."}, {'!', "..--."},  {'\'', ".----."}, {' ', "/"},
    {'\n', "//"}};

map<string, char> morse_to_ascii;

int main() {
  // build the reversed map
  for (auto kv : ascii_to_morse) morse_to_ascii[kv.second] = kv.first;

  // ascii to morse test
  {
    ifstream is{"../example/example.txt"};
    if (!is) return -1;

    ofstream os{"../example/example-morse.txt"};
    if (!os) return -2;

    // ascii to morse
    char c;
    while (is.get(c)) os << ascii_to_morse[tolower(c)] << ' ';
  }

  // morse to ascii test
  {
    ifstream is{"../example/example-morse.txt"};
    if (!is) return -1;

    ofstream os{"../example/example-morse-check.txt"};
    if (!os) return -2;

    // morse to ascii
    string m;
    while (is >> m) os << morse_to_ascii[m];
  }
}
