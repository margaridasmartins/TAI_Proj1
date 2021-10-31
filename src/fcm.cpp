#include "fcm.hpp"


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
