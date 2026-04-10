
#include "IndentedConfigParser.h"
// STL
#include <regex>
// FairRoot
#include <FairLogger.h>

namespace IndentedConfigParser
{

void PrintConfig(const Node& n, int indent)
{
    LOG(info) << std::string(indent, ' ') << " value : " << n.value << "\n";
    for (const auto& p : n.children)
        PrintConfig(p, indent + 4);
}

void ParseIStringStream(istringstream& iss, Node& root)
{
    std::regex rempty("\\s+\n*");
    string line;
    vector<Node*> stack{&root};
    while (std::getline(iss, line)) {
        if (line.empty())
            break;
        if (regex_match(line, rempty))
            break;
        size_t level = 0;
        while (level < line.size() && line[level] == '\t')
            ++level;
        string text = line.substr(level);
        stack.resize(level + 1);
        Node& node = stack.back()->children.emplace_back(Node{.value = text});
        node.value = text;
        stack.push_back(&node);
    }
    // PrintConfig(root);
}

void ParseString(string& str, Node& root)
{
    istringstream iss(str);
    ParseIStringStream(iss, root);
}

}   // namespace IndentedConfigParser
