#ifndef OBJECT_H
#define OBJECT_H

#include "ast.h"

#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>

namespace object {
    typedef std::string ObjectType;

    const ObjectType INTEGER_OBJ      = "INTEGER";
    const ObjectType STRING_OBJ       = "STRING";
    const ObjectType BOOLEAN          = "BOOLEAN";
    const ObjectType NULL_OBJ         = "NULL";
    const ObjectType RETURN_VALUE_OBJ = "RETURN_VALUE";
    const ObjectType FUNCTION_OBJ     = "FUNCTION";
    const ObjectType ARRAY_OBJ        = "ARRAY";
    const ObjectType BUILTIN_OBJ      = "BUILTIN";
    const ObjectType ERROR_OBJ        = "ERROR";

    class Object {
        public:
            std::int16_t refCount = 0;
            bool isAnon = true;
            virtual ~Object() = default;
            virtual ObjectType Type() const = 0;
            virtual std::string Inspect() const = 0;
            virtual Object* clone() const = 0;

            void incrRefCount() { refCount++; }
            void decRefCount()  { refCount--; }
    };

    struct Integer : public Object {
        int64_t Value; 

        Integer(int64_t value, bool incrRef=false) : Value(value) { 
            if (incrRef && refCount == 0) incrRefCount(); 
        }
        Integer(const Integer& other) : Value(other.Value) {
            incrRefCount();
        }
        ~Integer() {}

        ObjectType Type() const override { return INTEGER_OBJ; }
        std::string Inspect() const override { return std::to_string(Value); }
        Integer* clone() const override { return new Integer(*this); }
    };

    struct String : public Object {
        std::string Value;

        String(std::string value, bool incrRef=false) : Value(value) {
            if (incrRef) incrRefCount();
        }
        String(const String& other) : Value(other.Value) {}
        ~String() {}

        ObjectType Type() const override { return STRING_OBJ; }
        std::string Inspect() const override { return Value; }
        String* clone() const override { return new String(*this); }
    };

    struct Boolean : public Object {
        bool Value;

        Boolean(bool value, bool incrRef=false) : Value(value) {
            if (incrRef) incrRefCount();
        }
        Boolean(const Boolean& other) : Value(other.Value) {}

        ObjectType Type() const override { return BOOLEAN; }
        std::string Inspect() const override { return Value ? "true" : "false"; }
        Boolean* clone() const override { return new Boolean(*this); }
    };

    struct Null : public Object {
        ObjectType Type() const override { return NULL_OBJ; }
        std::string Inspect() const override { return "null"; }
        Null* clone() const override { return new Null(); }
    };

    struct ReturnValue : public Object {
        Object* Value;

        ReturnValue(Object* val, bool incrRef=false) : Value(val) {
            if (incrRef) incrRefCount();
        }
        ReturnValue(const ReturnValue& other) : Value(other.Value) {}

        ObjectType Type() const override { return RETURN_VALUE_OBJ; }
        std::string Inspect() const override { return Value->Inspect(); }
        ReturnValue* clone() const override { return new ReturnValue(*this); }
    };

    struct Error : public Object {
        std::string Message;

        Error(std::string msg, bool incrRef=false) : Message(msg) {}
        Error(const Error& other) : Message(other.Message) {}

        ObjectType Type() const override { return ERROR_OBJ; }
        std::string Inspect() const override { return "ERROR: " + Message; }
        Error* clone() const override { return new Error(*this); }
    };

    struct Array : public Object {
        std::vector<Object*> Elements;

        Array(std::vector<Object*> elements) : Elements(elements) {
            for (Object* el : Elements) {
                el->incrRefCount();
            }
        }
        Array(const Array& other) {
            for (unsigned int i = 0; i < other.Elements.size(); ++i) {
                Elements.push_back(other.Elements[i]->clone());
                Elements[i]->incrRefCount();
            }
        }
        ~Array() {
            for (Object* el : Elements) {
                el->decRefCount();
            }
        }

        ObjectType Type() const override { return ARRAY_OBJ; }
        std::string Inspect() const override { 
            std::stringstream out;
            out << "[";

            for (unsigned int i = 0; i < Elements.size(); ++i) {
                out << Elements[i]->Inspect();
                if (i < Elements.size() - 1) {
                    out << ", ";
                }
            }
            out << "]";

            return out.str();
        }
        Array* clone() const override { return new Array(*this); }
        void push(object::Object* obj) {
            obj->incrRefCount();
            Elements.push_back(obj);
        }
        void pop() {
            Elements.back()->decRefCount();
            Elements.pop_back();
        }
    };

    struct Environment {
        std::map<std::string, Object*> store;
        std::vector<Object*> heap;
        Environment* outer = nullptr;
            
        Environment() {}
        Environment(const Environment& other) : outer(other.outer) {
            for (const auto& pair : other.store) {
                store[pair.first] = pair.second->clone();
            }
            for (const auto& obj : other.heap) {
                heap.push_back(obj->clone());
            }
        }
        ~Environment() {
            std::cout << "TK_DEV::In ~Environment()" << std::endl;
            std::cout << "store.size() : " << store.size() << std::endl;
            for (auto& item : store) {
                store[item.first]->decRefCount();
            }
            std::cout << "TK_DEV::Leaving ~Environment()" << std::endl;
        }

        Environment* clone() { return new Environment(*this); }

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
            val->incrRefCount();
            if (val->isAnon)
                val->isAnon = false;
            store[name] = val;
            return val;
        }

        Environment* NewEnclosedEnvironment() {
            Environment* env = new Environment();
            env->outer = this;
            return env;
        }

        void clearHeap() {
            heap.erase(std::remove_if(heap.begin(), heap.end(),
                [this](Object* obj) {
                    if (obj->isAnon && obj->refCount <= 0) {
                        if (obj->Type() == ARRAY_OBJ) {
                            Array* objArr = dynamic_cast<Array*>(obj);
                            if (objArr->Elements.size() > 0) {
                                delete obj;
                                clearHeap();
                            }
                        } else {
                            delete obj;
                        }
                        return true;
                    }
                    return !obj->isAnon;
                }), heap.end());
        }

        void deleteAnonymousValues() {
            clearHeap();
            std::cout << "TK_DEV::Post clearHeap()" << std::endl;
            for(auto it = store.begin(); it != store.end();) {
                if (it->second->refCount <= 0) {
                    if (it->second->Type() == ARRAY_OBJ) {
                        delete it->second;
                        clearHeap();
                    } else {
                        delete it->second;
                    }
                    it = store.erase(it);
                } else {
                    ++it;
                }
            }
        }
    };

    struct Function : public Object {
        std::vector<ast::Identifier*> Parameters;
        ast::BlockStatement* Body;
        Environment* Env;

        Function(std::vector<ast::Identifier*> &params, 
                 ast::BlockStatement* body, 
                 object::Environment* env,
                 bool incrRef=false) 
            : Parameters(params), Body(body), Env(env)
        {
            if (incrRef) incrRefCount();
        }
        Function(const Function& other) : Env(other.Env->clone()), Body(other.Body->clone()) {
            for (unsigned int i = 0; i < other.Parameters.size(); ++i) {
                Parameters.push_back(other.Parameters[i]->clone());
            }
        }
        ~Function() {
            std::cout << "TK_DEV::In ~Function" << std::endl;
            for (ast::Identifier* param : Parameters) {
                delete param;
            }
            delete Body;
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
        Function* clone() const override { return new Function(*this); }
    };

    struct Builtin : public Object {
        std::function<Object*(std::vector<Object*> &args)> BuiltinFunction;

        Builtin(std::function<Object*(std::vector<Object*> &args)> fn) : BuiltinFunction(fn) {}
        Builtin(const Builtin& other) : BuiltinFunction(other.BuiltinFunction) {}

        ObjectType Type() const override { return BUILTIN_OBJ; }
        std::string Inspect() const override { return "builtin function"; }
        Builtin* clone() const override { return new Builtin(*this); }
    };

    extern std::map<std::string, object::Builtin*> builtins;

    // these serve as predefined singleton instances
    extern std::shared_ptr<Boolean> TRUE;
    extern std::shared_ptr<Boolean> FALSE;
    extern std::shared_ptr<Null>    NULL_T;
}

#endif // OBJECT_H
