#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>

namespace person {
    class MOCKPerson : public Person {
    public:
        MOCK_METHOD(int, AddCSVTable, (std::string const &filepath),
                    (const, override));
    };
} // namespace person