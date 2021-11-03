#include "fcm.hpp"


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

  FCM *fcm = new FCM(k);
  fcm->train(fptr);
  fcm->print_table();


  // if a prior is not passed the program will generate one
  if (argc == 5)
  {
    fcm->generate_text(a, argv[4]);
  }
  else
  {
    char prior[k];
    prior[0]=0;
    fcm->generate_text(a, prior);
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
  -> 
  ->
*/