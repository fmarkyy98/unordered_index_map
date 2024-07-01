#ifndef UNORDERED_INDEX_MAP_H
#define UNORDERED_INDEX_MAP_H

#include <unordered_map>
#include <vector>
#include <functional>
#include <type_traits>

template<typename T, auto... Members>
class unordered_index_map {
public:
    // Add a record
    void add(const T& record) {
        data.push_back(record);
        (add_to_index<Members>(record, data.size() - 1), ...);
    }

    // Retrieve records by exact key value
    template<auto Member>
    std::vector<const T*> get(typename std::decay_t<decltype(std::declval<T>().*Member)> key) const {
        return get<Member>([key](const auto& value) { return value == key; });
    }

    // Retrieve records by a predicate on the indexed member
    template<auto Member>
    std::vector<const T*> get(std::function<bool(std::decay_t<decltype(std::declval<T>().*Member)>)> predicate) const {
        const auto& index = get_index<Member>();
        std::vector<const T*> result;
        for (const auto& [key, indices] : index) {
            if (predicate(key)) {
                for (size_t idx : indices) {
                    result.push_back(&data[idx]);
                }
            }
        }
        return result;
    }

    // Function to get all records
    const std::vector<T>& get_all() const {
        return data;
    }

private:
    std::vector<T> data;

    // Helper to add to an index
    template<auto Member>
    void add_to_index(const T& record, size_t index) {
        auto& idx = get_index<Member>();
        idx[record.*Member].push_back(index);
    }

    // Helper to get the index for a specific member
    template<auto Member>
    auto& get_index() const {
        using MemberType = std::decay_t<decltype(std::declval<T>().*Member)>;
        static std::unordered_map<MemberType, std::vector<size_t>> index;
        return index;
    }
};

#endif // UNORDERED_INDEX_MAP_H
