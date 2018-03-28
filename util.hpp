#include <json/json.h>
#include <string>

using namespace std;

Json::Value to_json(const string &s);

bool match(const Json::Value &root, const Json::Value &query);