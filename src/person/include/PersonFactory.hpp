#ifndef PERSONFACTORY_HPP_
#define PERSONFACTORY_HPP_

#include "Person.hpp"
#include <memory>

namespace person {
class PersonFactory {
public:
    std::shared_ptr<person::PersonBase> create();
};
} // namespace person

#endif