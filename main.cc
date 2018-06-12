#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "util.h"
#include "decisiontree.h"

using namespace std;

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cerr << "USAGE : " << argv[0] << " [train_data_path] [test_data_path] [output_path]" << endl;
    exit(1);
  }

  cout << "Training...." << endl;
  string train_path = argv[1];
  string test_path = argv[2];
  string output_path = argv[3];
  DecisionTree dt(train_path);
  cout << "Complete Training" << endl;
  cout << endl;
  cout << "Testing..." << endl;
  dt.Classify(test_path, output_path);
  cout << "Complete Testing" << endl;

  return 0;
}
