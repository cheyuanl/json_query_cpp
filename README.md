# json_query_cpp
A Json document storage object that supports query by example.

There are two implementations:

1. ListStorage() implements simple linear search. 
2. GraphStorage() implements with trie and inverted index to achieve O(depth of Json structure) look up time (if no collision, for single query).
 
## Example:  
    Storage s = new GraphStorage(); // or new ListStorage();

    // add documents
    s.add("{"id":1,"last":"Doe","first":"John","location":{"city":"Oakland","state":"CA","postalCode":"94607"},"active":true}")
    s.add("{"id":2,"last":"Doe","first":"Jane","location":{"city":"San Francisco","state":"CA","postalCode":"94105"},"active":true}")

    // find documents
    string result = s.get("{"location":{"city":"Oakland"}}");

    // delete documents
    s.delete("{"id":2}")
    
    // show all documents in the storage
    s.get("{}");
    
    
./main provides a simple command line interface for "add" "get" "delete" documents 

    # example command line input
    add {"id":1,"last":"Doe","first":"John","location":{"city":"Oakland","state":"CA","postalCode":"94607"},"active":true}
    add {"id":2,"last":"Doe","first":"Jane","location":{"city":"San Francisco","state":"CA","postalCode":"94105"},"active":true}
    add {"id":3,"last":"Black","first":"Jim","location":{"city":"Spokane","state":"WA","postalCode":"99207"},"active":true}
    add {"id":4,"last":"Frost","first":"Jack","location":{"city":"Seattle","state":"WA","postalCode":"98204"},"active":false}
    get {"location":{"state":"WA"},"active":true}
    get {"id":1}
    get {"active":true}
    delete {"active":true}
    get {}
    
    # expected output 
    {"id":3,"last":"Black","first":"Jim","location":{"city":"Spokane","state":"WA","postalCode":"99207"},"active":true}
    {"id":1,"last":"Doe","first":"John","location":{"city":"Oakland","state":"CA","postalCode":"94607"},"active":true}
    {"id":1,"last":"Doe","first":"John","location":{"city":"Oakland","state":"CA","postalCode":"94607"},"active":true}
    {"id":2,"last":"Doe","first":"Jane","location":{"city":"San Francisco","state":"CA","postalCode":"94105"},"active":true}
    {"id":3,"last":"Black","first":"Jim","location":{"city":"Spokane","state":"WA","postalCode":"99207"},"active":true}
    {"id":4,"last":"Frost","first":"Jack","location":{"city":"Seattle","state":"WA","postalCode":"98204"},"active":false}


