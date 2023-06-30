#pragma once

#include <vector>
#include <string>
#include <cmath>
#include <iostream>

struct HallOfFameMember {
    PopMember member;
    bool exists;
};

template<typename T, typename L>
struct HallOfFame {
    std::vector<HallOfFameMember> members;

    explicit HallOfFame(const Options& options) {
        int actualMaxsize = options.maxsize + MAX_DEGREE;
        members.reserve(actualMaxsize);
        for (int i = 0; i < actualMaxsize; ++i) {
            Node<T>* node = new Node<T>(T(1));
            PopMember<T, L> popMember(node, L(0), L(INFINITY), options);
            HallOfFameMember hofMember{popMember, false};
            members.push_back(hofMember);
        }
    }

    HallOfFame<T, L> copy() const {
        HallOfFame<T, L> copy;
        copy.members.reserve(members.size());
        for (const auto& member : members) {
            Node<T>* nodeCopy = member.member.tree->copy();
            PopMember<T, L> popMember(nodeCopy, member.member.loss, member.member.score, member.member.options);
            HallOfFameMember hofMember{popMember, member.exists};
            copy.members.push_back(hofMember);
        }
        return copy;
    }

    std::vector<PopMember<T, L>> calculateParetoFrontier(const Dataset<T, L>& dataset, const Options& options) const {
        std::vector<PopMember<T, L>> dominating;
        int actualMaxsize = members.size();
        for (int size = 0; size < actualMaxsize; ++size) {
            if (!members[size].exists)
                continue;
            const PopMember<T, L>& member = members[size].member;
            bool betterThanAllSmaller = true;
            for (int i = 0; i < size; ++i) {
                if (!members[i].exists)
                    continue;
                const PopMember<T, L>& simplerMember = members[i].member;
                if (member.loss >= simplerMember.loss) {
                    betterThanAllSmaller = false;
                    break;
                }
            }
            if (betterThanAllSmaller)
                dominating.push_back(member.copy());
        }
        return dominating;
    }
};

std::vector<std::string> splitString(const std::string& s, int n) {
    std::vector<std::string> result;
    int len = s.length();
    if (len <= n) {
        result.push_back(s);
        return result;
    }
    int pos = 0;
    while (pos < len) {
        result.push_back(s.substr(pos, n));
        pos += n;
    }
    return result;
}

std::string stringDominatingParetoCurve(const HallOfFame<double, double>& hallOfFame, const Dataset<double, double>& dataset, const Options& options, int width = 100) {
    std::string output;
    double curMSE = dataset.baseline_loss;
    double lastMSE = curMSE;
    int lastComplexity = 0;
    output += "Hall of Fame:\n";
    output += std::string(width - 1, '-') + "\n";
    output += std::sprintf("%-10s  %-8s   %-8s  %-8s\n", "Complexity", "Loss", "Score", "Equation");

    std::vector<PopMember<double, double>> dominating = hallOfFame.calculateParetoFrontier(dataset, options);
    for (const auto& member : dominating) {
        int complexity = computeComplexity(member, options);
        if (member.loss < 0.0) {
            throw std::domain_error("Your loss function must be non-negative. To do this, consider wrapping your loss inside an exponential, which will not affect the search (unless you are using annealing).");
        }
        curMSE = member.loss;

        int delta_c = complexity - lastComplexity;
        double ZERO_POINT = 1e-10;
        double delta_l_mse = std::log(std::abs(curMSE / lastMSE) + ZERO_POINT);
        double score = -delta_l_mse / delta_c;
        std::string eqn_string = stringTree(member.tree, options.operators, dataset.varMap);
        int base_string_length = std::sprintf("%-10d  %-8.3e  %8.3e  ", 1, 1.0, 1.0).length();

        std::string dots = "...";
        int equation_width = (width - 1) - base_string_length - dots.length();

        output += std::sprintf("%-10d  %-8.3e  %-8.3e  ", complexity, curMSE, score);

        std::vector<std::string> split_eqn = splitString(eqn_string, equation_width);
        bool print_pad = false;
        while (split_eqn.size() > 1) {
            std::string cur_piece = split_eqn[0];
            split_eqn.erase(split_eqn.begin());
            output += std::string(print_pad * base_string_length, ' ') + cur_piece + dots + "\n";
            print_pad = true;
        }
        output += std::string(print_pad * base_string_length, ' ') + split_eqn[0] + "\n";

        lastMSE = curMSE;
        lastComplexity = complexity;
    }
    output += std::string(width - 1, '-');
    return output;
}