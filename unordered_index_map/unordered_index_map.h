#ifndef UNORDERED_INDEX_MAP_H
#define UNORDERED_INDEX_MAP_H

#include <unordered_map>
#include <vector>
#include <functional>
#include <type_traits>
#include <tuple>

// Helper to compare member pointers
template <typename T, typename MemberType, MemberType T::* Member>
struct member_pointer_comparator {
    static constexpr bool compare(MemberType T::* other) {
        return Member == other;
    }
};

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
        return std::get<index_of<Member>()>(indexes);
    }

    // Helper to get the index of a member in the tuple
    template<auto Member, size_t Index = 0, auto First, auto... Rest>
    constexpr static size_t index_of_impl() {
        using MemberType = decltype(Member);
        if constexpr (member_pointer_comparator<T, MemberType, Member>::compare(First)) {
            return Index;
        }
        else {
            return index_of_impl<Member, Index + 1, Rest...>();
        }
    }

    template<auto Member>
    constexpr static size_t index_of() {
        return index_of_impl<Member, 0, Members...>();
    }

    // Tuple of unordered maps for each member
    std::tuple<std::unordered_map<std::decay_t<decltype(std::declval<T>().*Members)>, std::vector<size_t>>...> indexes;
};

#endif // UNORDERED_INDEX_MAP_H
