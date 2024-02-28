#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>

namespace object {
    typedef std::string ObjectType;

    const ObjectType INTEGER_OBJ      = "INTEGER";
    const ObjectType BOOLEAN          = "BOOLEAN";
    const ObjectType NULL_OBJ         = "NULL";
    const ObjectType RETURN_VALUE_OBJ = "RETURN_VALUE";
    const ObjectType ERROR_OBJ        = "ERROR";


    class Object {
        public:
            bool isAnonymous = true;
            virtual ~Object() = default;
            virtual ObjectType Type() const = 0;
            virtual std::string Inspect() const = 0;
    };

    static std::vector<Object*> memhold;

    static void deleteAnonymousObjects() {
        for (auto mem : memhold) {
            if (mem->isAnonymous) {
                delete mem;
            }
        }
        memhold.clear();
    }

    struct Integer : public Object {
        int64_t Value; 

        Integer(int64_t value) : Value(value) {
            memhold.push_back(this);
        }

        ObjectType Type() const override { return INTEGER_OBJ; }
        std::string Inspect() const override { return std::to_string(Value); }
    };

    struct Boolean : public Object {
        bool Value;

        Boolean(bool value) : Value(value) {}

        ObjectType Type() const override { return BOOLEAN; }
        std::string Inspect() const override { return Value ? "true" : "false"; }
    };

    struct Null : public Object {
        ObjectType Type() const override { return NULL_OBJ; }
        std::string Inspect() const override { return "null"; }
    };

    struct ReturnValue : public Object {
        Object* Value;

        ReturnValue(Object* val) : Value(val) {
            memhold.push_back(this);
        }

        ObjectType Type() const override { return RETURN_VALUE_OBJ; }
        std::string Inspect() const override { return Value->Inspect(); }
    };

    struct Error : public Object {
        std::string Message;

        Error(std::string msg) : Message(msg) {
            memhold.push_back(this);
        }

        ObjectType Type() const override { return ERROR_OBJ; }
        std::string Inspect() const override { return "ERROR: " + Message; }
    };

    struct Environment {
        std::map<std::string, Object*> store;
            
        Environment() {}

        std::pair<Object*, bool> Get(std::string name) {
            auto it = store.find(name);
            if (it != store.end()) {
                return {it->second, true};
            } else {
                return {nullptr, false};
            }
        }
        ~Environment() {
            // a hacky workaround but performance doesn't really matter here
            std::vector<Object*> deletedObjs;
            for (auto& item : store) {
                if (std::find(deletedObjs.begin(), deletedObjs.end(), item.second) == deletedObjs.end()) {
                    delete item.second;
                    deletedObjs.push_back(item.second);
                }
            }
            store.clear();
        }
        
        Object* Set(std::string name, Object* val) {
            store[name] = val;
            return val;
        }
    };

    // these serve as predefined singleton instances
    // treat as const, DO NOT reassign them
    extern std::shared_ptr<Boolean> TRUE;
    extern std::shared_ptr<Boolean> FALSE;
    extern std::shared_ptr<Null>    NULL_T;
}

#endif // OBJECT_H
