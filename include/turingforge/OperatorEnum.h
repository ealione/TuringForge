#pragma once

#include <vector>
#include <functional>
#include <ostream>
#include <stdexcept>

struct AbstractOperatorEnum {
    std::string print(std::ostream& os, const std::string ops = ""){
        throw std::logic_error("Not implemented");
        //os << "something";

    }
    std::ostream& operator << (std::ostream& os) {
        print(os);
        return os;
    }
};

struct OperatorEnum : public AbstractOperatorEnum {
    std::vector<std::function<double(double, double)>> binops;
    std::vector<std::function<double(double)>> unaops;
    std::vector<std::function<double(double, double)>> diff_binops;
    std::vector<std::function<double(double)>> diff_unaops;
};

struct GenericOperatorEnum : public AbstractOperatorEnum {
    std::vector<std::function<double(double, double)>> binops;
    std::vector<std::function<double(double)>> unaops;
};