#include "PersonFactory.hpp"

namespace person {
    std::shared_ptr<person::Person> PersonFactory::create() {
        return std::make_shared<person::Person>();
    }
} // namespace person