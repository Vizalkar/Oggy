# Oggy
Handy utilities

## Opaque List
```c++
// Manage a collection of objects through opaque unique ids 
int main()
{
    Oggy::OpaqueList<int, int*> olist;

    // Add objects without getting their ID 
    for (int i(0); i<50; ++i){
        olist.addVal(new int(i));
    }
    
    // Add objects and get their ID so you can refer to that object from afar
    int id = olist.addVal(new int(50));
    std::cout << olist.find(id)->second << std::endl;
    
    for (int i(51); i<100; ++i){
        olist.addVal(new int(i));
    }
    
    // Iterate through the list
    for (const auto& i : olist){
        std::cout << *i << " ";
    }

    std::cout << std::endl << olist.size() << std::endl;

    // Get all IDs in one vector
    auto ids(olist.getIDVector());
    
    // Remove objects with given ID
    // Objects are returned so you can delete them if it was pointers
    for (auto it(ids.begin() + 10); it < ids.end() -10 ; ++it){
        delete olist.removeVal(*it);
    }

    for (const auto& i : olist){
        std::cout << *i << " ";
    }

    std::cout << std::endl << olist.size() << std::endl;
}
```
