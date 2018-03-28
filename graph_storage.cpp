#include "graph_storage.hpp"
#include "util.hpp"

void Node::debug_print(int level = 0) {
    for (const auto &e : map) {
        for (int k = 0; k < level; k++) {
            cout << " ";
        }
        cout << "key: " << e.first << endl;
        e.second->debug_print(level + 2);
    }
}

void Node::create_index(const rid &it, Json::Value root) {
    // list, cannot be hashed anymore.
    if (root.isArray()) {
        leafs.push_back(pair<Json::Value, rid>(root, it));
        return;
    }

    // value (i.e. string, int, float, etc, hash its string representation.
    if (!root.size()) {
        string key = root.asString();
        if (map.find(key) == map.end()) {
            map[key] = shared_ptr<Node>(new Node());
        }
        map[key]->leafs.push_back(pair<Json::Value, rid>(root, it));
        return;
    }

    // object (expand the tree)
    for (const string &key : root.getMemberNames()) {
        if (map.find(key) == map.end()) {
            map[key] = shared_ptr<Node>(new Node());
        }
        map[key]->create_index(it, root[key]);
    }
}

void Node::collect_index(list<pair<Json::Value, rid> > &min_leafs,
                         Json::Value query) {
    // list, cannot be hashed anymore.
    if (query.isArray()) {
        if (min_leafs.empty() || leafs.size() < min_leafs.size()) {
            min_leafs = leafs;
        }
        return;
    }

    // value (i.e. string, int, float, etc, hash its string representation.
    if (!query.size()) {
        string key = query.asString();
        if (map.find(key) != map.end()) {
            if (min_leafs.empty() ||
                map[key]->leafs.size() < min_leafs.size()) {
                min_leafs = map[key]->leafs;
            }
        }
        return;
    }

    // object (keep searching the tree)
    for (const string &key : query.getMemberNames()) {
        if (map.find(key) != map.end()) {
            map[key]->collect_index(min_leafs, query[key]);
        }
    }
    return;
}

bool GraphStorage::add(const string &s) {
    storage.push_back(s);
    rid it = storage.end();

    Json::Value data = to_json(s);
    root.create_index(--it, data);

    return true;
}

/**
 * First find the minium subset of documents (leaf) that matches the query.
 * Then do linear search within the subset.
 */
vector<string> GraphStorage::get(const string &s) {
    Json::Value query = to_json(s);
    vector<string> output;

    if (query.empty()) {
        for (const string &s : storage) {
            if (!s.empty()) {
                output.push_back(s);
            }
        }
        return output;
    }

    list<pair<Json::Value, rid> > min_leafs;
    root.collect_index(min_leafs, query);

    // TODO: better way to mark deletion?
    for (pair<Json::Value, rid> p : min_leafs) {
        if (!p.second->empty() && match(to_json(*(p.second)), query)) {
            output.push_back(*(p.second));
        }
    }

    return output;
}

bool GraphStorage::del(const string &s) {
    Json::Value query = to_json(s);

    if (query.empty()) {
        return true;
    }

    list<pair<Json::Value, rid> > min_leafs;
    root.collect_index(min_leafs, query);

    // TODO: better way to mark deletion?
    for (pair<Json::Value, rid> p : min_leafs) {
        if (!p.second->empty() && match(to_json(*(p.second)), query)) {
            p.second->clear();
        }
    }
    return true;
}
