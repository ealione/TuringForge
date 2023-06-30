#pragma once

#include <iostream>
#include <vector>

#include "Options.h"
#include "Constants.h"

using namespace std;

struct RunningSearchStatistics {
    vector<double> frequencies;
    double window_size{};
    vector<double> normalized_frequencies;
};

RunningSearchStatistics runningSearchStatistics(Options options, int window_size = 100000) {
    int maxsize = options.maxsize;
    int actualMaxsize = maxsize + MAX_DEGREE;
    vector<double> init_frequencies(actualMaxsize, 1.0);

    RunningSearchStatistics running_search_statistics;
    running_search_statistics.window_size = window_size;
    running_search_statistics.frequencies = init_frequencies;
    running_search_statistics.normalized_frequencies = init_frequencies;

    return running_search_statistics;
}

void move_window(RunningSearchStatistics& running_search_statistics) {
    double smallest_frequency_allowed = 1;
    int max_loops = 1000;

    vector<double>& frequencies = running_search_statistics.frequencies;
    double window_size = running_search_statistics.window_size;

    double cur_size_frequency_complexities = 0;
    for (double frequency : frequencies) {
        cur_size_frequency_complexities += frequency;
    }

    if (cur_size_frequency_complexities > window_size) {
        double difference_in_size = cur_size_frequency_complexities - window_size;

        int num_loops = 0;

        while (difference_in_size > 0) {
            vector<int> indices_to_subtract;

            for (int i = 0; i < frequencies.size(); ++i) {
                if (frequencies[i] > smallest_frequency_allowed) {
                    indices_to_subtract.push_back(i);
                }
            }

            int num_remaining = indices_to_subtract.size();

            double amount_to_subtract =
                    min(difference_in_size / num_remaining,
                        *min_element(frequencies.begin(), frequencies.end()) - smallest_frequency_allowed);

            for (int index : indices_to_subtract) {
                frequencies[index] -= amount_to_subtract;
            }

            double total_amount_to_subtract = amount_to_subtract * num_remaining;

            difference_in_size -= total_amount_to_subtract;

            ++num_loops;

            if (num_loops > max_loops || total_amount_to_subtract < 1e-6) {
                break;
            }
        }
    }
}

void normalize_frequencies(RunningSearchStatistics& running_search_statistics) {
    vector<double>& normalized_frequencies = running_search_statistics.normalized_frequencies;
    vector<double>& frequencies = running_search_statistics.frequencies;

    double sum_of_frequencies = 0;
    for (double frequency : frequencies) {
        sum_of_frequencies += frequency;
    }

    for (double& frequency : frequencies) {
        frequency /= sum_of_frequencies;
    }
}

void update_frequencies(RunningSearchStatistics& running_search_statistics, int size=-1) {
    if (size >= 0 && size < running_search_statistics.frequencies.size()) {
        running_search_statistics.frequencies[size] += 1;
    }
}