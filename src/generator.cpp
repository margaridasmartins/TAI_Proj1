#include "fcm.hpp"

int main(int argc, char *argv[]) {
  string help_text =
      "Usage:\n"
      "  ./generator filename context_size alpha [options]\n"
      "Required:\n"
      "  filename       The name of the file inside the example folder to train the generator\n"
      "  context_size   The size of the context which translates into the order of the model\n"
      "  alpha          The value for the smoothing parameter\n"
      "Options:\n"
      "  -p | --prior=<context>     The initial context to feed the generator (random if not provided)\n"
      "  -s | --text-size=<size>    The size of the text to be generated (default is 1000)\n"
      "  -t | --threshold=<size>    The maximum table size in MB to use an array based model rather than hash based (default is 500)\n"
      "  -h | --help                Print a helper message to use the program\n"
      "       --show-random         Distinguish symbols generated randomly for not having a trained context\n"
      "       --relative-random     Generate symbols with no trained context according to the character frequency\n"
      "Example:\n"
      "  ./generator example 2 0.5\n";

  if (argc < 4) {
    printf("ERR: Incorrect number of arguments\n\n%s", help_text.c_str());
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
    exit(2);
  }

  char *prior = new char[k];
  prior[0] = 0;
  float threshold = 256;
  uint text_size = 1000;
  int option, option_index = 0;
  int relative_random = 0, show_random = 0;

  static struct option long_options[] = {
      {"relative-random", no_argument, &relative_random, 1},
      {"show-random", no_argument, &show_random, 1},
      {"prior", required_argument, 0, 'p'},
      {"text-size", required_argument, 0, 's'},
      {"threshold", required_argument, 0, 't'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};

  while ((option = getopt_long(argc, argv, "p:s:t:h", long_options,
                               &option_index)) != -1) {
    switch (option) {
      case 'p':
        if (strlen(optarg) >= k) {
          strncpy(prior, optarg + (strlen(optarg) - k), k);
        } else {
          strncpy(prior, optarg, k);
        }
        break;
      case 's':
        text_size = atoi(optarg);
        break;
      case 't':
        threshold = atof(optarg);
        break;
      case 'h':
        printf("%s", help_text.c_str());
        exit(0);
      default:
        abort();
    }
  }

  FCM *fcm = new FCM(k);
  fcm->train(fptr, threshold);
  // fcm->print_table();
  printf("p %s\n", prior);
  fcm->generate_text(a, prior, text_size, relative_random, show_random);

  fclose(fptr);

  return 0;
}
