#include <iostream>
#include <string>
#include "graph_storage.hpp"

using namespace std;

int main() {
    unique_ptr<Storage> js(new GraphStorage());
    string line;

    while (getline(cin, line)) {
        size_t space_pos = line.find(" ");
        const string &op = line.substr(0, space_pos);
        const string &data = line.substr(space_pos + 1, line.size());

        if (op == "get") {
            for (const string &s : js->get(data)) {
                cout << s << endl;
            }
        } else if (op == "add") {
            js->add(data);
        } else if (op == "delete") {
            js->del(data);
        } else {
            throw invalid_argument("Invalid input format.");
        }
    }
    
    return 0;
}