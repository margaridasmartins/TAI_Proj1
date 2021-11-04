#include "fcm.hpp"

int main(int argc, char *argv[]) {
  if (argc < 4) {
    printf("ERR: Incorrect number of arguments\n");
    printf(
        "\ncd bin/ && ./generator <file-from-example-folder> <context-size> <alpha>\n"
        "# example\n./generator example 2 2\n");
    printf(
        "\nOptions:\n-p <prior>\t start text generator with prior\n"
        "-s <textSize>\t define number of characters of output text\n");
    exit(1);
  }

  uint k;
  float a;
  char filename[100];
  sprintf(filename, "../example/%s.txt", argv[1]);
  k = atoi(argv[2]);
  a = atof(argv[3]);
  printf("%f\n", a);

  FILE *fptr;

  if ((fptr = fopen(filename, "r")) == NULL) {
    printf("ERR: File not found\n");
    exit(1);
  }

  char prior[k];
  prior[0] = 0;
  uint textSize = 1000;
  int option;

  while ((option = getopt(argc, argv, "p:s:h")) !=
         -1) {  // get option from the getopt() method
    switch (option) {
      case 'p':
        strcpy(prior, optarg);
        break;
      case 's':
        textSize = atoi(optarg);
        break;
      case 'h':
        printf(
            "\ncd bin/ && ./generator <file-from-example-folder> "
            "<context-size> <alpha>\n# example\n./generator example 2 2\n");
        printf(
            "\nOptions:\n-p <prior>\t start text generator with prior\n-s "
            "<textSize>\t define number of characters of output text\n");
        break;
    }
  }

  printf("Prior: %s\n", prior);
  printf("Text Size: %d\n", textSize);
  FCM *fcm = new FCM(k);
  fcm->train(fptr);
  fcm->print_table();
  fcm->generate_text(a, prior, textSize);

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