#include <string>
#include <vector>
using namespace std;


/**
 * The interface for query by example json.
 */
class Storage {
   public:
    // return true on success.
    virtual bool add(const string &s) = 0;
    // return the matched docments.
    virtual vector<string> get(const string &s) = 0;
    // delete the matched documents.
    virtual bool del(const string &s) = 0;

    virtual ~Storage(){};
};