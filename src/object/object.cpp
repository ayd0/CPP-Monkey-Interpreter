#include "../../include/object.h"

namespace object {
    std::shared_ptr<Boolean> TRUE  =  std::make_shared<Boolean>(true);
    std::shared_ptr<Boolean> FALSE =  std::make_shared<Boolean>(false);
    std::shared_ptr<Null>    NULL_T = std::make_shared<Null>();
}
