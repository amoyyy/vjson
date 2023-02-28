# vjson
JSON parser and encoder in C/C++, EASY to modify, LIGHTWEIGHT in small project

**Support Datatypes:**
```
null, bool, integer, double, string, array, object
```

**APIs:**
```
string get_string() 
int64_t get_int() 
double get_double();
bool get_bool() 
ValueArray &get_array() 
ValueObject &get_object() 
```

**Usage:**
```
// case1: to parser json data from file "in.json";
Value& p = Parser("in.json").decode();

// case2: to get json data
Value& v_ref = p["key"];

// case3: to operate json data
v["key1"].set_value(3.0); 
or v["key1"] = 3.0;

v["key3"].set_value("CIAE JSON");
or  v["key3"] = "CIAE JSON";

// case4: to add member to the array or object, 
v.push_back(value)
v.push_back(value,"key")

// case5: to delete member of a object
v.delete_item("key")

// case6: to replace member of a object
v.replace_item(Value* v, "key")

// case6: others function : judge whether "key" is a item of object
v_ref: v_ref["key1"].has_member("key")；
```
