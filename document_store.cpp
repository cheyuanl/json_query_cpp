#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <json/json.h>

using namespace std;

/**
 * The storage element for GraphJsonStorage.
 */
class Node {
private:
    list<pair<Json::Value, list<string>::iterator>> leafs;
    
    unordered_map<string, Node*> map;
    
    void debug_print(int i=0) {
        for (auto e : map) {
            for (int k = 0; k < i; k++) {
                cout << " ";
            }
            cout << "key: " << e.first << endl;
            e.second->debug_print(i+2); 
        }
    }
    
public:
    void create_index(const list<string>::iterator &it, Json::Value root) {
        // List
        if (root.isArray()) {
            leafs.push_back(pair<Json::Value, list<string>::iterator>(root, it));
            return;
        }
        
        // Value (i.e. string, int, float, etc)
        if (!root.size()) {
            string key = root.asString();
            if (map.find(key) == map.end()) {
                map[key] = new Node();
            }
            map[key]->leafs.push_back(pair<Json::Value, list<string>::iterator>(root, it));
            return;
        }
        
        // Object
        for (const string &key : root.getMemberNames()) {
            if (map.find(key) == map.end()) {
                map[key] = new Node();
            }
            map[key]->create_index(it, root[key]);
        }
    }
    
    void collect_index(list<pair<Json::Value, list<string>::iterator>> &min_leafs, Json::Value query) {
        // List
        if (query.isArray()) {
            if (min_leafs.empty() || leafs.size() < min_leafs.size()){
                min_leafs = leafs;
            }
            return;
        }
        
        // Value (i.e. string, int, float, etc) 
        if (!query.size()) {
            string key = query.asString();
            if (map.find(key) != map.end()) {
                if (min_leafs.empty() || map[key]->leafs.size() < min_leafs.size()){
                    min_leafs = map[key]->leafs;
                }
            }
            return;
        }
        
        //Object
        for (const string &key : query.getMemberNames()) {
            if (map.find(key) != map.end()) {
                map[key]->collect_index(min_leafs, query[key]);
            }
        }
        return;
    }
};


/**
 * The interface for query by example json.
 */
class JsonStorage {
public:
    // return true on success.
    virtual bool add(const string &s) = 0;
    // return the matched docments.
    virtual vector<string> get(const string &s) = 0;
    // delete the matched documents.
    virtual bool del(const string &s) = 0;
};


/**
 * (Second attempt.) 
 * Index the structure of the ducment with a tree. When new documents 
 * are added the tree will store the pointer (iterator) at the end point.
 *
 * author Che-Yuan Liang, Mar. 2018
 * issues 1. the reverse index won't be cleaned after deletion.
 *        2. destructor of nodes.
 *        3. currently the way to delete a document is simply setting the string as empty.
 */
class GraphJsonStorage : public JsonStorage {
private:
    list<string> storage;
    Node root;
    Json::Reader reader;
    Json::Value holder;
    
    inline Json::Value to_json(const string &s) {
        if (!reader.parse(s, holder)) {
            cout << reader.getFormattedErrorMessages() << endl;
        }
        return holder;
    }
    
    bool match(const Json::Value &root, const Json::Value &query) {        
        if (root.isObject() && query.isObject()) {       
            for (const string &key : query.getMemberNames()) {
                if(!match(root[key], query[key])) {
                    return false;
                }
            }
            return true;
        } else if (root.isArray() && query.isArray()) {            
            for (int i = 0; i < query.size(); i++) {
                bool found = false;
                for (int j = 0; j < root.size(); j++) {
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

    void debug_print() {
        for (const string &s : storage) {
            cout << s.empty() << " " << s.size() << " " << s << endl;
        }
    }

public:
    bool add(const string &s) {
        storage.push_back(s);
        list<string>::iterator it = storage.end();
        
        Json::Value data = to_json(s);
        root.create_index(--it, data);
    
        return true;
    }
    
    vector<string> get(const string &s) {
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
        
        list<pair<Json::Value, list<string>::iterator>> min_leafs;
        root.collect_index(min_leafs, query);
        
        
        for (pair<Json::Value, list<string>::iterator> p : min_leafs) {
            if (!p.second->empty() && match(to_json(*(p.second)), query)) {
                output.push_back(*(p.second));
            }
        }

        return output;
    }
    
    bool del(const string &s) {
        Json::Value query = to_json(s);
        
        if (query.empty()) {
            return true;
        }
        
        list<pair<Json::Value, list<string>::iterator>> min_leafs;
        root.collect_index(min_leafs, query);
        
        for (pair<Json::Value, list<string>::iterator> p : min_leafs) {

            if (!p.second->empty() && match(to_json(*(p.second)), query)) {
                p.second->clear();
            }
        }
        return true;
    }
};

/**
 * First attempt (for refernece) 
 * Simply store document in a list, and perform linear search.
 *
 * @author Che-Yuan Liang, Mar. 2018
 */
class ListJsonStorage : public JsonStorage {
private:
    list<pair<Json::Value, string>> storage;
    Json::Reader reader;
    Json::Value holder;
    
    inline Json::Value to_json(const string &s) {
        if (!reader.parse(s, holder)) {
            cout << reader.getFormattedErrorMessages() << endl;
        }
        return holder;
    }
    
    bool match(const Json::Value &root, const Json::Value &query) {        
        if (root.isObject() && query.isObject()) {       
            for (const string &key : query.getMemberNames()) {
                if(!match(root[key], query[key])) {
                    return false;
                }
            }
            return true;
        } else if (root.isArray() && query.isArray()) {            
            for (int i = 0; i < query.size(); i++) {
                bool found = false;
                for (int j = 0; j < root.size(); j++) {
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
    
public:
    bool add(const string &s) {
        Json::Value root = to_json(s);
        if (!root) {
            return false;
        }

        storage.push_back(pair<Json::Value, string>(to_json(s), string(s)));
        return true;
    }
                                                
    vector<string> get(const string &s) {
        Json::Value query = to_json(s);
        if (!query) {
            return {};
        }
        
        vector<string> output;    
        for (const pair<Json::Value, string> &p : storage) {
            if (match(p.first, query)){
                output.push_back(p.second);
            }
        }
        return output;
    }
                          
    bool del(const string &s) {
        Json::Value query = to_json(s);
        if (!query) {
            return false;
        }
                
        storage.remove_if(
            [query, this](pair<Json::Value, string> p){
                return match(p.first, query);
            });
        return true;
    }
};

int main() {
    JsonStorage *js = new GraphJsonStorage();
    string line;
    
    while (getline(cin, line)){
        size_t space_pos = line.find(" ");
        const string &op = line.substr(0, space_pos);
        const string &data = line.substr(space_pos+1, line.size());
        
        if (op == "get") {
            vector<string> output = js->get(data);
            for (const string &s : output) {
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

    delete js; // TODO: recursively destroy the nodes.
    return 0;
}