#include "Utils.hpp"

/// @brief 
namespace Utils {

    /// @brief 
    /// @param probability 
    /// @return 
    double probabilityToRate(double probability){
        if(probability < 0 || probability >= 1){
            throw std::domain_error("Out of probability value range");
        }
        return -log(1 - probability);
    }

    /// @brief 
    /// @param rate 
    /// @return 
    double rateToProbability(double rate){
        if(rate < 0){
            throw std::domain_error("Out of rate value range");
        }
        return 1 - exp(-rate);
    }

    /// @brief 
    /// @param valueToDiscount 
    /// @param discountRate 
    /// @param timestep 
    /// @return 
    double discount(double valueToDiscount, double discountRate, double timestep){
        if(discountRate < 0 || timestep < 0){
            throw std::domain_error("Out of DiscountRate or Timestep Range");
        }
        return valueToDiscount / pow(1 + discountRate, timestep);
    }
}