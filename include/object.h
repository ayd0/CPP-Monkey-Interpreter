#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <memory>

namespace object {
    typedef std::string ObjectType;

    const ObjectType INTEGER_OBJ = "INTEGER";
    const ObjectType BOOLEAN     = "BOOLEAN";
    const ObjectType NULL_OBJ    = "NULL";

    class Object {
        public:
            virtual ~Object() = default;
            virtual ObjectType Type() const = 0;
            virtual std::string Inspect() const = 0;
    };

    struct Integer : public Object {
        int64_t Value; 

        Integer(int64_t value) : Value(value) {}

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

    // these serve as predefined singleton instances
    // treat as const, DO NOT reassign them
    extern std::shared_ptr<Boolean> TRUE;
    extern std::shared_ptr<Boolean> FALSE;
    extern std::shared_ptr<Null>    NULL_T;
}

#endif // OBJECT_H
