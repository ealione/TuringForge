#include <string>
#include <unordered_map>

struct RecordType {};

struct Options {
    bool recorder;
};

#define recorder(ex) \
    if (options.recorder) { \
        ex; \
    }

int find_iteration_from_record(const std::string& key, const RecordType& record) {
    int iteration = 0;
    while (record[key].count("iteration" + std::to_string(iteration))) {
        iteration++;
    }
    return iteration - 1;
}
