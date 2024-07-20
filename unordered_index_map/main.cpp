#include <iostream>
#include <string>
#include "unordered_index_map.h"

struct foo {
    int i;
    double d;
    char c;
    std::string s;
};

int main() {
    unordered_index_map<foo, &foo::i, &foo::d, &foo::c> foo_u_index_map;

    foo record1 = { 1, 3.14, 'a', "Hello" };
    foo record2 = { 2, 2.71, 'b', "World" };
    foo record3 = { 1, 2.71, 'c', "Foo" };

    foo_u_index_map.add(record1);
    foo_u_index_map.add(record2);
    foo_u_index_map.add(record3);

    // Retrieve by index on 'i'
    auto results = foo_u_index_map.get<&foo::i>(1);
    std::cout << "Found by i: " << results.size() << '\n';
    for (const auto* result : results) {
        std::cout << result->i << ", " << result->d << ", " << result->c << ", " << result->s << '\n';
    }

    // Retrieve by predicate on 'd'
    auto pred_results = foo_u_index_map.get<&foo::d>([](double d) { return d > 2.7; });
    std::cout << "Found by predicate on d: " << pred_results.size() << '\n';
    for (const auto* result : pred_results) {
        std::cout << result->i << ", " << result->d << ", " << result->c << ", " << result->s << '\n';
    }

    // Retrieve by index on 'c'
    auto results_c = foo_u_index_map.get<&foo::c>('c');
    std::cout << "Found by c: " << results_c.size() << '\n';
    for (const auto* result : results_c) {
        std::cout << result->i << ", " << result->d << ", " << result->c << ", " << result->s << '\n';
    }

    return 0;
}
