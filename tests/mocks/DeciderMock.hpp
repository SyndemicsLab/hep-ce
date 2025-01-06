#ifndef DECIDERMOCK_HPP_
#define DECIDERMOCK_HPP_

#include "Decider.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace stats {
    class MOCKDecider : public DeciderBase {
    public:
        MOCK_METHOD(int, LoadGenerator, (std::mt19937_64 const generator),
                    (override));
        MOCK_METHOD(int, GetDecision, (std::vector<double> probs), (override));
    };
} // namespace stats

#endif