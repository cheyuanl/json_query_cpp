#include "list_storage.hpp"
#include <sstream>
#include "util.hpp"

bool ListStorage::add(const string &s) {
    Json::Value root = to_json(s);
    if (!root) {
        return false;
    }

    storage.push_back(pair<Json::Value, string>(to_json(s), string(s)));
    return true;
}

vector<string> ListStorage::get(const string &s) {
    Json::Value query = to_json(s);
    if (!query) {
        return vector<string>();
    }

    vector<string> output;
    for (const pair<Json::Value, string> &p : storage) {
        if (match(p.first, query)) {
            output.push_back(p.second);
        }
    }
    return output;
}

bool ListStorage::del(const string &s) {
    Json::Value query = to_json(s);
    if (!query) {
        return false;
    }

    storage.remove_if([query, this](pair<Json::Value, string> p) {
        return match(p.first, query);
    });
    return true;
}