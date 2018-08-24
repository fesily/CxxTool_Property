# CxxTool_Property
c++17 property  tool library.header only!
## example:
```
PropertyName<int> key{"123"};
 
PropertyName<int> key1{"1234"};
 
PropertyComponent component;
 
component.AddValueReference(key,1);//init property "123" (int) = 1;
  
component.AddValueReference(key1,1);//init property "1234" (int) = 1;
  
component[key] = 3;
  
component[key1] = 2;
```
