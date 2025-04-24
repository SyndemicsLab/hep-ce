////////////////////////////////////////////////////////////////////////////////
// File: aging_internals.hpp                                                  //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BASE_AGINGINTERNALS_HPP_
#define HEPCE_EVENT_BASE_AGINGINTERNALS_HPP_

#include <hepce/event/base/aging.hpp>

#include <DataManagement/DataManagerBase.hpp>

#include "internals/event_internals.hpp"
#include <hepce/model/cost.hpp>
#include <hepce/utils/config.hpp>
#include <hepce/utils/pair_hashing.hpp>

namespace hepce {
namespace event {
namespace base {
class AgingImpl : public virtual Aging, public EventBase {
public:
    using agemap_t =
        std::unordered_map<utils::tuple_3i, data::CostUtil, utils::key_hash_3i,
                           utils::key_equal_3i>;
    AgingImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
              const std::string &log_name = "console") {
        SetDiscount(utils::GetDoubleFromConfig("cost.discounting_rate", dm));
        SetCostCategory(model::CostCategory::kBackground);
        _age_data.clear();
        LoadData(dm);
    }
    ~AgingImpl() = default;

    int Execute(model::Person &person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                model::Sampler &sampler) override;

private:
    agemap_t _age_data;

    static int Callback(void *storage, int count, char **data, char **columns) {
        utils::tuple_3i tup = std::make_tuple(
            std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
        data::CostUtil cu = {utils::SToDPositive(data[3]),
                             utils::SToDPositive(data[4])};
        (*((agemap_t *)storage))[tup] = cu;
        return 0;
    }

    inline const std::string BuildSQL() const {
        return "SELECT age_years, gender, drug_behavior, cost, utility "
               "FROM background_impacts;";
    }

    int LoadData(std::shared_ptr<datamanagement::DataManagerBase> dm);

    /// @brief Adds person's background cost
    /// @param person The person to whom cost will be added
    void AddBackgroundCostAndUtility(
        model::Person &person,
        std::shared_ptr<datamanagement::DataManagerBase> dm);
};
} // namespace base
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_BASE_AGINGINTERNALS_HPP_