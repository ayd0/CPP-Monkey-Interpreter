#ifndef OBJECT_H
#define OBJECT_H

#include "ast.h"

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>

namespace object {
    typedef std::string ObjectType;

    const ObjectType INTEGER_OBJ      = "INTEGER";
    const ObjectType STRING_OBJ       = "STRING";
    const ObjectType BOOLEAN          = "BOOLEAN";
    const ObjectType NULL_OBJ         = "NULL";
    const ObjectType RETURN_VALUE_OBJ = "RETURN_VALUE";
    const ObjectType FUNCTION_OBJ     = "FUNCTION";
    const ObjectType ERROR_OBJ        = "ERROR";


    class Object {
        public:
            bool isAnonymous = true;
            virtual ~Object() = default;
            virtual ObjectType Type() const = 0;
            virtual std::string Inspect() const = 0;
    };

    static std::vector<Object*> memhold;
    std::vector<Object*>& getMemhold();

    static void deleteAnonymousObjects() {
        auto& memhold = getMemhold();
        for (auto mem : memhold) {
            if (mem->isAnonymous) {
                delete mem;
            }
        }
        memhold.clear();
    }

    struct Integer : public Object {
        int64_t Value; 
        bool isAnonymous = true;

        Integer(int64_t value) : Value(value) {
            getMemhold().push_back(this);
        }
        ~Integer() {}

        ObjectType Type() const override { return INTEGER_OBJ; }
        std::string Inspect() const override { return std::to_string(Value); }
    };

    struct String : public Object {
        std::string Value;

        String(std::string value) : Value(value) {
            getMemhold().push_back(this);
        }
        ~String() {}

        ObjectType Type() const override { return STRING_OBJ; }
        std::string Inspect() const override { return Value; }
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

        ReturnValue(Object* val) : Value(val) { memhold.push_back(this); }

        ObjectType Type() const override { return RETURN_VALUE_OBJ; }
        std::string Inspect() const override { return Value->Inspect(); }
    };

    struct Error : public Object {
        std::string Message;

        Error(std::string msg) : Message(msg) {
            getMemhold().push_back(this);
        }

        ObjectType Type() const override { return ERROR_OBJ; }
        std::string Inspect() const override { return "ERROR: " + Message; }
    };

    struct Environment {
        std::map<std::string, Object*> store;
        Environment* outer = nullptr;
            
        Environment() {}
        ~Environment() {
            std::vector<Object*> deletedObjs;
            for (auto& item : store) {
                if (std::find(deletedObjs.begin(), deletedObjs.end(), item.second) == deletedObjs.end()) {
                    delete item.second;
                    deletedObjs.push_back(item.second);
                }
            }
            store.clear();
        }

        std::pair<Object*, bool> Get(std::string name) {
            std::pair<Object*, bool> objOk = {nullptr, false};
            auto it = store.find(name);
            if (it != store.end()) {
                objOk = {store[name], true};
            } else if (outer != nullptr) {
                objOk = outer->Get(name);     
            }
            return objOk;
        }
        
        Object* Set(std::string name, Object* val) {
            store[name] = val;
            return val;
        }

        Environment* NewEnclosedEnvironment() {
            Environment* env = new Environment();
            env->outer = this;
            return env;
        }
    };

    struct Function : public Object {
        std::vector<ast::Identifier*> Parameters;
        ast::BlockStatement* Body;
        Environment* Env;

        Function(std::vector<ast::Identifier*> &params, 
                 ast::BlockStatement* body, 
                 object::Environment* env) 
            : Parameters(params), Body(body), Env(env) {}
        ~Function() {
            for (ast::Identifier* param : Parameters) {
                delete param;
            }
            delete Body;
            delete Env;
        }
        
        ObjectType Type() const override { return FUNCTION_OBJ; }
        std::string Inspect() const override { 
            std::stringstream out;

            out << "fn(";
            for (unsigned int i = 0; i < Parameters.size(); ++i) {
                out << Parameters[i]->String();
                if (i < Parameters.size() - 1) {
                    out << ", ";
                }
            }
            out << ") {\n" << Body->String() << "\n}";

            return out.str();
        }
    };

    // these serve as predefined singleton instances
    extern std::shared_ptr<Boolean> TRUE;
    extern std::shared_ptr<Boolean> FALSE;
    extern std::shared_ptr<Null>    NULL_T;
}

#endif // OBJECT_H
