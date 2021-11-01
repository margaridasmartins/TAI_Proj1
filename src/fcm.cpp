#include "fcm.hpp"

#include <sciplot/sciplot.hpp>
using namespace sciplot;

void plot_graphs(FILE *fptr) {
 
  // Create values for your x-axis
  Vec x = linspace(0.0, 5.0, 100);

  // Create a Plot object
  Plot plot;

  // Set color palette
  plot.palette("set2");

  // Draw a sine graph putting x on the x-axis and sin(x) on the y-axis
  plot.drawCurve(x, sin(x)).label("sin(x)").lineWidth(4);

  // Draw a cosine graph putting x on the x-axis and cos(x) on the y-axis
  plot.drawCurve(x, cos(x)).label("cos(x)").lineWidth(4);

  // Show the plot in a pop-up window
  plot.show();

  // Save the plot to a PDF file
  plot.save("plot.pdf");
}

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

  plot_graphs(fptr);

  FCM *fcm = new FCM(k, a);
  fcm->train(fptr);
  fcm->print_table();
  printf("Entropy is: %4f\n", fcm->get_entropy(a));

  fclose(fptr);

  return 0;
}
