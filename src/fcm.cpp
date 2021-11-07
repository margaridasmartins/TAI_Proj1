#include "fcm.hpp"
#include <chrono>
#include <bits/stdc++.h>
#include <sciplot/sciplot.hpp>
using namespace sciplot;

void plot_graph_alpha(FILE *fptr) {
  Plot plot;

  plot.fontSize(5);
  plot.legend()
      .atOutsideTop()
      .displayHorizontal()
      .displayExpandWidthBy(2)
      .fontSize(4);

  plot.ylabel("Entropy (bits/letter)");
  plot.xlabel("Alfa α");

  Vec x = {0.001, 0.01, 0.1, 0.2, 0.5, 1, 2};
  Vec k = {1, 2, 3, 4, 5};

  for (int i = 0; i < k.size(); i++) {
    Vec y;
    y.resize(x.size());

    FCM *fcm = new FCM(k[i]);
    fcm->train(fptr);

    for (int j = 0; j < x.size(); j++) {
      y[j] = fcm->get_entropy(x[j]);
      printf("k= %2d a= %2.5f  ent= %2.7f\n", (uint)k[i], x[j], y[j]);
    }

    char label[100];
    sprintf(label, "k= %1.3f", k[i]);
    plot.drawCurve(x, y).lineWidth(1).label(label);
  }

  Figure fig = {{plot}};

  fig.palette("moreland");
  fig.title("Influence of alfa value in the entropy");
  fig.show();
  fig.save("graph_alfa2.pdf");
}

void plot_graph_context(FILE *fptr) {
  Plot plot;

  plot.fontSize(5);
  plot.legend()
      .atOutsideBottom()
      .displayHorizontal()
      .displayExpandWidthBy(2)
      .fontSize(4);

  plot.ylabel("Entropy (bits/letter)");
  plot.xlabel("Context size k");

  Vec x = {1, 2, 3, 4, 5, 6, 7, 8};
  Vec a = {0.001, 0.1, 0.5, 1, 5};
  Vec *y = new Vec[a.size()];

  for (int j = 0; j < a.size(); j++) y[j].resize(x.size());

  for (int i = 0; i < x.size(); i++) {
    FCM *fcm = new FCM(x[i]);
    fcm->train(fptr);

    for (int j = 0; j < a.size(); j++) {
      y[j][i] = fcm->get_entropy(a[j]);
      printf("k= %2d a= %2.5f  ent= %2.7f\n", (uint)x[i], a[j], y[j][i]);
    }
  }

  for (int j = 0; j < a.size(); j++) {
    char label[100];
    sprintf(label, "alpha= %1.3f", a[j]);
    plot.drawCurve(x, y[j]).lineWidth(1).label(label);
  }

  Figure fig = {{plot}};

  fig.palette("moreland");
  fig.title("Influence of context size in the entropy");
  fig.show();
  fig.save("graph_context.pdf");
}

void plot_graph_time(FILE *fptr) {

  Plot plot;

  plot.fontSize(5);
  plot.ylabel("Execution time(s)");
  plot.xlabel("Context size k");

  plot.legend().hide();

  Vec x = {1, 2, 3, 4, 5, 6,7,8};


  Vec *y = new Vec[1];
  y[0].resize(x.size());

  for (int i = 0; i < x.size(); i++) {

    // measuring time
    auto start = std::chrono::high_resolution_clock::now();

    FCM *fcm = new FCM(x[i]);
    
    fcm->train(fptr);


    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    
    y[0][i] = elapsed.count();

    printf("k= %2d   time= %2.7f\n", (uint)x[i], y[0][i]);
    
  }

  float largest = y[0][0];
  for(int i = 1;i < x.size(); i++) {
    if(largest < y[0][i])
      largest = y[0][i];
  }

  plot.yrange("0", to_string(largest));
  plot.drawCurve(x, y[0]).lineWidth(1);

 
  Figure fig = {{plot}};

  fig.palette("moreland");
  fig.title("Execution times for different contexts");
  fig.show();
  fig.save("graph_time.pdf");
}

int main(int argc, char *argv[]) {
  string help_text =
      "Usage:\n"
      "  ./fcm filename context_size alpha [options]\n"
      "Required:\n"
      "  filename       The name of the file inside the example folder to train the model\n"
      "  context_size   The size of the context which translates into the order of the model\n"
      "  alpha          The value for the smoothing parameter\n"
      "Options:\n"
      "  -i           Plot additional statistical information about the model\n"
      "Example:\n"
      "  ./fcm example 2 0.5\n";

  if (argc < 4) {
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

  bool show_stats;
  int option, option_index = 0;
  static struct option long_options[] = {{"stats", no_argument, 0, 'i'},
                                         {"help", no_argument, 0, 'h'},
                                         {0, 0, 0, 0}};

  while ((option = getopt_long(argc, argv, "ih", long_options,
                               &option_index)) != -1) {
    switch (option) {
      case 'i':
        show_stats = true;
        break;
      case 'h':
        printf("%s", help_text.c_str());
        exit(0);
      default:
        abort();
    }
  }

  FCM *fcm = new FCM(k);
  fcm->train(fptr);
  // fcm->print_table();
  printf("Entropy: %2.5f\n", fcm->get_entropy(a));

  if (show_stats) {
    printf("\nCreating plots...\n");
    plot_graph_context(fptr);
    plot_graph_alpha(fptr);
    plot_graph_time(fptr);
  }

  fclose(fptr);

  return 0;
}
