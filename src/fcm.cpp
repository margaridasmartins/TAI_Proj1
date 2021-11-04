#include "fcm.hpp"

#include <vector>
#include <sciplot/sciplot.hpp>
using namespace sciplot;

void plot_graphs(FILE *fptr) {
  // Create values for your x-axis
  Vec x = {1, 2, 3, 4, 5, 6};
  // vector<uint> x = {2, 1, 3, 4, 5, 6};

  // Create a Plot object
  Plot plot;

  // Set color palette
  plot.palette("set2");

  plot.fontSize(5);

  // plot.le

  float alphas[4] = {0.001, 0.1, 0.5, 1};
  Vec *y = new Vec[4];
  // vector<double> y[4];

  for (int j = 0; j < 4; j++)
    y[j].resize(6);
  
  for (int i = 0; i < x.size(); i++) {
    FCM *fcm = new FCM(x[i]);
    fcm->train(fptr);
    // fcm->print_table();
    for (int j = 0; j < 4; j++) {
      y[j][i] = fcm->get_entropy(alphas[j]);
      // y[j].push_back(fcm->get_entropy(alphas[j]));

      printf("k= %2d a= %2.5f  ent= %2.7f\n", (uint)x[i], alphas[j], y[j][i]);
    }
  }
  
  for (int j = 0; j < 4; j++) {
    char label[100];
    sprintf(label, "alpha= %1.3f", alphas[j]);
    plot.drawCurve(x, y[j]).label(label).lineWidth(1);
  }

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

  // FCM *fcm = new FCM(k);
  // fcm->train(fptr);
  // fcm->print_table();
  // printf("Entropy is: %4f\n", fcm->get_entropy(a));

  fclose(fptr);

  return 0;
}
