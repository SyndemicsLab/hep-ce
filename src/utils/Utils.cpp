#include "Utils.hpp"

namespace Utils {
    double probabilityToRate(double probability){
        if(probability < 0 || probability >= 1){
            throw std::domain_error("Out of probability value range");
        }
        return -log(1 - probability);
    }

    double rateToProbability(double rate){
        if(rate < 0){
            throw std::domain_error("Out of rate value range");
        }
        return 1 - exp(-rate);
    }

    double discount(double valueToDiscount, double discountRate, double timestep){
        if(discountRate < 0 || timestep < 0){
            throw std::domain_error("Out of DiscountRate or Timestep Range");
        }
        return valueToDiscount / pow(1 + discountRate, timestep);
    }
}