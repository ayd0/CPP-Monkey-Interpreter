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
                }

                return new Error("argument to \"len\" not supported, got " + args[0]->Type());
            })
        },
    };
};
