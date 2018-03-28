#include "util.hpp"
#include <iostream>
#include <sstream>

using namespace std;

Json::Value to_json(const string &s) {
    Json::CharReaderBuilder rbuilder;
    Json::Value holder;
    string errs;

    stringstream ss(s);
    bool ok = Json::parseFromStream(rbuilder, ss, &holder, &errs);
    if (!ok) {
        cout << errs << endl;
    }
    return holder;
}

/**
 * Recursively check if a root matches the query by doing DFS.
 *
 * If the query is a list, the root is defined to be matched if all objects
 * within the query exists in root as well. (i.e root = [1,2,3,4], query = [1,3]
 * is considered match) (i.e root = [1,2,3,4], query = [1,3,5] is considered not
 * match)
 */
bool match(const Json::Value &root, const Json::Value &query) {
    // Object
    if (root.isObject() && query.isObject()) {
        for (const string &key : query.getMemberNames()) {
            if (!match(root[key], query[key])) {
                return false;
            }
        }
        return true;
        // List
    } else if (root.isArray() && query.isArray()) {
        for (uint i = 0; i < query.size(); i++) {
            bool found = false;
            for (uint j = 0; j < root.size(); j++) {
                if (match(root[j], query[i])) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }
        return true;
    } else if (!root.size() && !query.size()) {
        return root == query;
    }

    return false;
}