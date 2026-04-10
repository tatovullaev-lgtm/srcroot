#pragma once

#include <sstream>
#include <string>
#include <vector>

using std::istringstream;
using std::string;
using std::vector;

namespace IndentedConfigParser
{

struct Node
{
    vector<Node> children;
    string value;
};

void PrintConfig(const Node& n, int indent = 0);

void ParseIStringStream(std::istringstream& iss, Node& root);

void ParseString(string& str, Node& root);

}   // namespace IndentedConfigParser
