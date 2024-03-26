#include "../../include/object.h"

void TestStringHashKey();

/*
int main() {
    TestStringHashKey();
}
*/

void TestStringHashKey() {
    object::String* hello1 = new object::String("Hello World");
    object::String* hello2 = new object::String("Hello World");
    object::String* diff1  = new object::String("My name is Johnny");
    object::String* diff2  = new object::String("My name is Johnny");

    object::HashKey hello1HK = hello1->getHashKey();
    object::HashKey hello2HK = hello2->getHashKey();
    object::HashKey diff1HK = diff1->getHashKey();
    object::HashKey diff2HK = diff2->getHashKey();

    if (hello1HK.Type != hello2HK.Type && hello1HK.Value != hello2HK.Value) {
        std::cerr << "strings with the same content have different hash keys" << std::endl;
    }

    if (diff1HK.Type != diff2HK.Type && diff1HK.Value != diff2HK.Value) {
        std::cerr << "strings with the same content have different hash keys" << std::endl;
    }

    if (hello1HK.Type == diff1HK.Type && hello1HK.Value != hello2HK.Value) {
        std::cerr << "strings with the same content have different hash keys" << std::endl;
    }
}
