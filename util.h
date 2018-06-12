//
// Created by bong on 2018. 4. 15..
//

#ifndef DECISIONTREE_UTIL_H
#define DECISIONTREE_UTIL_H

#include <string>
using namespace std;

vector<string> split(const string &s, const string& delimiter) {
  vector<string> tokens;
  string token;
  size_t delimiter_length = delimiter.length();
  size_t prev_pos = 0;
  size_t pos = s.find_first_of(delimiter);

  while (pos != string::npos) {
    token = s.substr(prev_pos, (pos - prev_pos));
    tokens.push_back(token);
    prev_pos = pos + delimiter_length;
    pos = s.find_first_of(delimiter, pos + 1);
  }

  if (prev_pos != s.length()) {
    token = s.substr(prev_pos);
    tokens.push_back(token);
  }
  return tokens;
}
#endif //DECISIONTREE_UTIL_H
