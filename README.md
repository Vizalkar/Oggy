# Oggy
Handy utilities

## Opaque List
  // Manage a collection of objects with opaque unique ids 
  
  int main()
  {
      Oggy::OpaqueList<int, int*> olist;

      for (int i(0); i<100; ++i){
          olist.addVal(new int(i));
      }

      for (const auto& i : olist){
          std::cout << *i << " ";
      }

      std::cout << std::endl << olist.size() << std::endl;

      auto ids(olist.getIDVector());
      for (auto it(ids.begin() + 10); it < ids.end() -10 ; ++it){
          delete olist.removeVal(*it);
      }

      for (const auto& i : olist){
          std::cout << *i << " ";
      }

      std::cout << std::endl << olist.size() << std::endl;
  }
