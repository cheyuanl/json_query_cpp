#include <json/json.h>
#include <iostream>
#include <list>
#include "storage.hpp"

/**
 * List storage for Json documents.
 *
 * @author Che-Yuan Liang, Mar. 2018
 */
class ListStorage : public Storage {
   private:
    list<pair<Json::Value, string> > storage;

   public:
    bool add(const string &s);
    vector<string> get(const string &s);
    bool del(const string &s);
};