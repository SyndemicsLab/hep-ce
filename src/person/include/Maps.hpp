#ifndef PERSON_MAPS_HPP_
#define PERSON_MAPS_HPP_

#include "Person.hpp"
#include <map>
#include <string>

namespace person {
    std::map<std::string, HCV> person::hcvMap = {
        {"none", HCV::NONE}, {"acute", HCV::ACUTE}, {"chronic", HCV::CHRONIC}};

    std::map<std::string, Behavior> person::behaviorMap = {
        {"never", Behavior::NEVER},
        {"former_noninjection", Behavior::FORMER_NONINJECTION},
        {"former_injection", Behavior::FORMER_INJECTION},
        {"noninjection", Behavior::NONINJECTION},
        {"injection", Behavior::INJECTION}};

    std::map<std::string, LinkageType> person::linkageTypeMap = {
        {"background", LinkageType::BACKGROUND},
        {"intervention", LinkageType::INTERVENTION}};

    std::map<std::string, LinkageState> person::linkageStateMap = {
        {"never", LinkageState::NEVER},
        {"linked", LinkageState::LINKED},
        {"unlinked", LinkageState::UNLINKED}};

    std::map<std::string, FibrosisState> person::fibrosisStateMap = {
        {"none", FibrosisState::NONE},    {"f0", FibrosisState::F0},
        {"f1", FibrosisState::F1},        {"f2", FibrosisState::F2},
        {"f3", FibrosisState::F3},        {"f4", FibrosisState::F4},
        {"decomp", FibrosisState::DECOMP}};

    std::map<std::string, MeasuredFibrosisState>
        person::measuredFibrosisStateMap = {
            {"none", MeasuredFibrosisState::NONE},
            {"f01", MeasuredFibrosisState::F01},
            {"f23", MeasuredFibrosisState::F23},
            {"f4", MeasuredFibrosisState::F4},
            {"decomp", MeasuredFibrosisState::DECOMP}};

    std::map<std::string, HCCState> person::hccStateMap = {
        {"none", HCCState::NONE},
        {"early", HCCState::EARLY},
        {"late", HCCState::LATE}};

    std::map<std::string, MOUD> person::moudMap = {
        {"none", MOUD::NONE}, {"current", MOUD::CURRENT}, {"post", MOUD::POST}};

    std::map<std::string, Sex> person::sexMap = {{"male", Sex::MALE},
                                                 {"female", Sex::FEMALE}};

    std::map<std::string, PregnancyState> person::pregnancyStateMap = {
        {"none", PregnancyState::NONE},
        {"pregnant", PregnancyState::PREGNANT},
        {"postpartum", PregnancyState::POSTPARTUM}};

    std::map<HCV, std::string> person::hcvEnumToStringMap = {
        {HCV::NONE, "none"}, {HCV::ACUTE, "acute"}, {HCV::CHRONIC, "chronic"}};

    std::map<Behavior, std::string> person::behaviorEnumToStringMap = {
        {Behavior::NEVER, "never"},
        {Behavior::FORMER_NONINJECTION, "former_noninjection"},
        {Behavior::FORMER_INJECTION, "former_injection"},
        {Behavior::NONINJECTION, "noninjection"},
        {Behavior::INJECTION, "injection"}};

    std::map<LinkageType, std::string> person::linkageTypeEnumToStringMap = {
        {LinkageType::BACKGROUND, "background"},
        {LinkageType::INTERVENTION, "intervention"}};

    std::map<LinkageState, std::string> person::linkageStateEnumToStringMap = {
        {LinkageState::NEVER, "never"},
        {LinkageState::LINKED, "linked"},
        {LinkageState::UNLINKED, "unlinked"}};

    std::map<FibrosisState, std::string> person::fibrosisStateEnumToStringMap =
        {{FibrosisState::NONE, "none"},    {FibrosisState::F0, "f0"},
         {FibrosisState::F1, "f1"},        {FibrosisState::F2, "f2"},
         {FibrosisState::F3, "f3"},        {FibrosisState::F4, "f4"},
         {FibrosisState::DECOMP, "decomp"}};

    std::map<MeasuredFibrosisState, std::string>
        person::measuredFibrosisStateEnumToStringMap = {
            {MeasuredFibrosisState::NONE, "none"},
            {MeasuredFibrosisState::F01, "f01"},
            {MeasuredFibrosisState::F23, "f23"},
            {MeasuredFibrosisState::F4, "f4"},
            {MeasuredFibrosisState::DECOMP, "decomp"}};

    std::map<MOUD, std::string> person::moudEnumToStringMap = {
        {MOUD::NONE, "none"}, {MOUD::CURRENT, "current"}, {MOUD::POST, "post"}};

    std::map<Sex, std::string> person::sexEnumToStringMap = {
        {Sex::MALE, "male"}, {Sex::FEMALE, "female"}};

    std::map<PregnancyState, std::string>
        person::pregnancyStateEnumToStringMap = {
            {PregnancyState::NONE, "none"},
            {PregnancyState::PREGNANT, "pregnant"},
            {PregnancyState::POSTPARTUM, "postpartum"}};
} // namespace person
#endif
