#ifndef OBJECT_H
#define OBJECT_H

#include <string>

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

        ObjectType Type() const override { return INTEGER_OBJ; }
        std::string Inspect() const override { return std::to_string(Value); }
    };

    struct Boolean : public Object {
        bool Value;

        ObjectType Type() const override { return BOOLEAN; }
        std::string Inspect() const override { return std::to_string(Value); }
    };

    struct Null : public Object {
        ObjectType Type() const override { return NULL_OBJ; }
        std::string Inspect() const override { return "null"; }
    };
}

#endif // OBJECT_H
