#include "../../include/object.h"

namespace object {
    std::map<std::string, Builtin*> builtins {
        {
            "len",
            new Builtin([](std::vector<Object*> &args) -> Object* {
                if (args.size() != 1) {
                    std::stringstream out;
                    out << "wrong number of arguments. got=" << args.size() << ", want=1";
                    return new Error(out.str());
                }
                
                if (args[0]->Type() == STRING_OBJ) {
                    String* strObj = dynamic_cast<String*>(args[0]);
                    return new Integer(strObj->Value.length());
                } else if (args[0]->Type() == ARRAY_OBJ) {
                    Array* arrObj = dynamic_cast<Array*>(args[0]); 
                    return new Integer(arrObj->Elements.size());
                }

                return new Error("argument to `len` not supported, got " + args[0]->Type());
            })
        },
        {
            "last",
            new Builtin([](std::vector<Object*> &args) -> Object* {
                if (args.size() != 1) {
                    std::stringstream out;
                    out << "wrong number of arguments. got=" << args.size() << ", want=1";
                    return new Error(out.str());
                }
                          
                if (args[0]->Type() != ARRAY_OBJ) {
                    return new Error("argument to `last` must be ARRAY, got " + args[0]->Type());     
                }

                Array* arrObj = dynamic_cast<Array*>(args[0]);
                if (arrObj->Elements.size() == 0) {
                    return new Error("arrObj->Elements size is 0");
                }

                return arrObj->Elements.back();
            })
        },
        {
            "tail",
            new Builtin([](std::vector<Object*> &args) -> Object* {
                if (args.size() != 1) {
                    std::stringstream out;
                    out << "wrong number of arguments. got=" << args.size() << ", want=1";
                    return new Error(out.str());
                }
                          
                if (args[0]->Type() != ARRAY_OBJ) {
                    return new Error("argument to `rest` must be ARRAY, got " + args[0]->Type());     
                }

                Array* arrObj = dynamic_cast<Array*>(args[0]);
                if (arrObj->Elements.size() < 2) {
                    return new Error("arrObj->Elements size less than minimum required (2)");
                }


                std::vector<Object*> elements = arrObj->Elements;
                return new Array(std::vector<Object*>(elements.begin() + 1, elements.end()));
            })
        },
        {
            "push",
            new Builtin([](std::vector<Object*> &args) -> Object* {
                if (args.size() != 2) {
                    std::stringstream out;
                    out << "wrong number of arguments. got=" << args.size() << ", want=2";
                    return new Error(out.str());
                }
                          
                if (args[0]->Type() != ARRAY_OBJ) {
                    return new Error("argument to `push` must be ARRAY, got " + args[0]->Type());     
                }

                Array* arrObj = dynamic_cast<Array*>(args[0]);
                arrObj->push(args[1]);
                        
                return arrObj;
            })
        },
        {
            "pop",
            new Builtin([](std::vector<Object*> &args) -> Object* {
                if (args.size() != 1) {
                    std::stringstream out;
                    out << "wrong number of arguments. got=" << args.size() << ", want=1";
                    return new Error(out.str());
                }
                          
                if (args[0]->Type() != ARRAY_OBJ) {
                    return new Error("argument to `pop` must be ARRAY, got " + args[0]->Type());     
                }

                Array* arrObj = dynamic_cast<Array*>(args[0]);
                arrObj->pop();

                return new Integer(arrObj->Elements.size());
            })
        }
    };
};