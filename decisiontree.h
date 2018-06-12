//
// Created by bong on 2018. 4. 15..
//

#ifndef DECISIONTREE_DECISIONTREE_H
#define DECISIONTREE_DECISIONTREE_H

#include <vector>
#include <string>
#include <set>
#include <map>
#include <math.h>
#include <time.h>
#include <cstdlib>

using namespace std;

class TreeNode;

typedef string attr_name;
typedef string attr_value;
typedef vector< vector<attr_value>> Tuples;
typedef map<attr_name, set<attr_value>> Attributes;
typedef map<attr_value, TreeNode*> NodeMap;

class TreeNode {
  NodeMap node_map;
  attr_name name;
  string label;
  bool isLeaf;

public:
  TreeNode() {
    isLeaf = false;
  }

  TreeNode* CreateChildNode(string value) {
    TreeNode *node = new TreeNode();
    node_map.insert(make_pair(value, node));
    return node;
  }

  TreeNode* GetChildNode(string value) {
    return node_map.at(value);
  }

  void SetAttr(string name_) {
    name = name_;
  }

  void SetLabel(string label_) {
    label = label_;
  }

  string GetName() {
    return name;
  }

  string GetLabel() {
    return label;
  }

  void setOnLeaf() {
    isLeaf = true;
  }

  void setOffLeaf() {
    isLeaf = false;
  }

  bool IsLeaf() {
    return isLeaf;
  }

};

class DecisionTree {
  TreeNode *root;
  Attributes attributes_;
  Tuples tuples_;
  vector<string> attr_names;
  map<attr_name, int> index_;
  set<string> labels;
  bool isconstruct;

public :
  DecisionTree(string filename) {
    root = new TreeNode();
    ReadDataFile(filename);
    labels = attributes_.at(attr_names.back());
    attributes_.erase(attr_names.back());
    Construct(root, attributes_, tuples_);
    isconstruct = true;
    clear();
  }

  void clear() {
    attributes_.clear();
    tuples_.clear();
    index_.clear();
    labels.clear();
  }

  bool IsAllSameLabel(Tuples tuples) {
    vector<string> tuple = tuples.at(0);

    for (size_t i = 1; i < tuples.size(); i++) {
      if (tuple.back() != tuples.at(i).back()) {
        return false;
      }
    }
    return true;
  }

  void Construct(TreeNode* node, Attributes attributes, Tuples tuples) {
    srand (time(NULL));
    if (IsAllSameLabel(tuples)) {
      node->setOnLeaf();
      node->SetLabel(tuples.at(0).back());
    } else {
      string name = SelectAttribute(attributes, tuples);
      set<string> values = attributes.at(name);

      node->SetAttr(name);

      size_t index = index_.at(name);
      attributes.erase(name);

      for (auto value : values) {
        TreeNode *child = node->CreateChildNode(value);
        Tuples sub_tuples;
        for (size_t i = 0; i < tuples.size(); i++) {
          vector<string> tuple = tuples.at(i);
          if (tuple.at(index) == value) {
            sub_tuples.push_back(vector<attr_value>());
            for (auto element : tuple) {
              sub_tuples.back().push_back(element);
            }
            tuples.erase(tuples.begin() + i);
            --i;
          }
        }
        if (sub_tuples.size() != 0) {
          Construct(child, attributes, sub_tuples);
        } else {
          int label_index = rand() % labels.size();
          int count = 0;
          string label;
          for (auto labelname : labels) {
            if (count == label_index) {
              label = labelname;
              break;
            }
            ++count;
          }
          child->SetLabel(label);
          child->setOnLeaf();
        }
        child = nullptr;
      }
    }
  }

  string SelectAttribute(Attributes attributes, Tuples tuples) {
    double gain = -1;
    double max_gain = -1;
    string max_attribute = "";
    double total_entropy = 0.0;
    double D = tuples.size();
    double splitInfo = 0.0;

    for (auto const label : labels) {
      double count = 0;
      for (auto const tuple : tuples) {
        if (tuple.back() == label) {
          ++count;
        }
      }
      if (count != 0) {
        total_entropy += (-1 * (((double) count / D) * log2((double) count / D)));
      }
    }

    for (auto attribute : attributes) {
      gain = ComputeInformationGain(attribute, tuples, total_entropy);
      if (gain > max_gain) {
        max_gain = gain;
        max_attribute = attribute.first;
      }
    }
    return max_attribute;
  }

  double ComputeInformationGain(pair<string, set<string>> attribute, Tuples tuples, double total_entropy) {
    double D = tuples.size();
    double split_info = 0.0;
    size_t index = index_.at(attribute.first);
    set<string> values = attribute.second;

    double entropy_sum = 0.0;

    for (auto const value : values) {
      Tuples sub_tuples;
      for (size_t i = 0; i < tuples.size(); i++) {
        vector<string> tuple = tuples.at(i);
        if (tuple.at(index) == value) {
          sub_tuples.push_back(vector<attr_value>());
          for (auto element : tuple) {
            sub_tuples.back().push_back(element);
          }
          tuples.erase(tuples.begin() + i);
          --i;
        }
      }

      double subD = sub_tuples.size();
      if (subD != 0) {
        split_info += (-1 * (((double) subD / D) * log2((double) subD / D)));
      }

      for (auto label : labels) {
        size_t count = 0;
        for (auto tuple : sub_tuples) {
          if (tuple.back() == label) {
            ++count;
          }
        }
        if (count != 0) {
          entropy_sum += (subD / D) * (-1 * (((double) count / subD) * log2((double)count / subD)));
        }
      }
    }

    return (total_entropy - entropy_sum)/split_info;
  }

  void ReadDataFile(string filename) {
    ifstream file(filename, ifstream::in);
    vector<string> tuple;
    string line;
    size_t count = 0;

    while (getline(file, line)) {
      size_t number = 0;
      tuple = split(line, "\t");
      for (auto column : tuple) {
        size_t pos = 0;
        while ((pos = column.find("\r",pos)) != string::npos) {
          column.erase(pos, 1);
        }
        if (count == 0) {
          if (!isconstruct) {
            attr_names.push_back(column);
          }
          index_.insert(make_pair(column, number));
          attributes_.insert(make_pair(column, set<attr_value>()));
        } else {
          if (number == 0) {
            tuples_.push_back(vector<attr_value>());
          }

          tuples_.back().push_back(column);
          attributes_.at(attr_names.at(number)).insert(column);
        }
        ++number;
      }
      count++;
    }
  }

  void DataClassify() {
    TreeNode* child = nullptr;
    for (size_t i = 0; i < tuples_.size(); i++) {
      int index = index_.at(root->GetName());
      child = root->GetChildNode(tuples_.at(i).at(index));
      FindLabelAndSet(child, i);
    }
    child = nullptr;
  }

  void FindLabelAndSet(TreeNode* node, int tuple_number) {
    TreeNode* child = nullptr;
    if (node->IsLeaf()) {
        tuples_.at(tuple_number).push_back(node->GetLabel());
    } else {
      int index = index_.at(node->GetName());
      child = node->GetChildNode(tuples_.at(tuple_number).at(index));
      FindLabelAndSet(child, tuple_number);
    }
    child = nullptr;
  }

  void Classify(string input, string output) {
    ReadDataFile(input);
    DataClassify();
    WriteResult(output);
  }

  void WriteResult(string output) {
    ofstream outfile;
    outfile.open(output);

    int count = 0;
    for (auto name : attr_names) {
      if (count < (attr_names.size() - 1)) {
        outfile << name << "\t";
      } else {
        outfile << name << "\n";
      }
      ++count;
    }
    for (auto tuple : tuples_) {
      count = 0;
      for (auto element : tuple) {
        if (count < (tuple.size() - 1)) {
          outfile << element << "\t";
        } else {
          outfile << element << "\n";
        }
        ++count;
      }
    }
  }
};
#endif //DECISIONTREE_DECISIONTREE_H
