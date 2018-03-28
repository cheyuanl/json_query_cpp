#include <json/json.h>
#include <iostream>
#include <list>
#include <sstream>
#include <unordered_map>
#include "storage.hpp"

typedef list<string>::iterator rid;  // index of record

/**
 * The storage element for GraphStorage.
 */
class Node {
   private:
    // the endpoints, where the value cannot be hashed anymore.
    list<pair<Json::Value, rid> > leafs;
    unordered_map<string, shared_ptr<Node> > map;

    void debug_print(int level);

   public:
    /**
     * Append the rid index to the tree.
     */
    void create_index(const rid &it, Json::Value root);

    /**
     * Find the minimum subset of documents (leafs) that matches the query.
     */
    void collect_index(list<pair<Json::Value, rid> > &min_leafs,
                       Json::Value query);
};
/**
 * Store the index of the documents in the tree structure.
 *
 * @author Che-Yuan Liang, Mar. 2018
 * @issues 1. the reverse index won't be cleaned up after deletion. Currently
 *            the way to delete a document is done by setting the record as
 *            empty.
 */
class GraphStorage : public Storage {
   private:
    list<string> storage;
    Node root;

    void debug_print() {
        for (const string &s : storage) {
            cout << s.empty() << " " << s.size() << " " << s << endl;
        }
    }

   public:
    bool add(const string &s);
    vector<string> get(const string &s);
    bool del(const string &s);
};
