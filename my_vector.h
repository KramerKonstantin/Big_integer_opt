#ifndef BIGINTOPT_MY_VECTOR_H
#define BIGINTOPT_MY_VECTOR_H


#include <cassert>
#include <vector>
#include <memory>
#include <cstdint>

const size_t SIZE = 32;

struct my_vector {
    bool is_vector_created = false;
    size_t vector_size;
    union any_data {
        uint32_t small_data[SIZE];
        std::shared_ptr<std::vector<uint32_t>> big_data;
        any_data() {};
        ~any_data() {};
    } _data;
    void to_big();
public:
    my_vector();
    my_vector(my_vector const &vector);
    my_vector &operator=(my_vector const &vector);
    explicit my_vector(size_t size);
    my_vector(size_t size, uint32_t value);
    ~my_vector();
    size_t size() const;
    bool empty() const;
    void ensure_unique();
    void resize(size_t size);
    void push_back(uint32_t value);
    void push_front(uint32_t value);
    uint32_t pop_back();
    uint32_t &back();
    uint32_t  &operator[](size_t index);
    const uint32_t &operator[](size_t index) const;
    friend bool operator==(my_vector const &a, my_vector const &b);
    static void swap(my_vector &a, my_vector &b);
    static void swap_big_small(my_vector::any_data &a, my_vector::any_data &b);

};


#endif //BIGINTOPT_MY_VECTOR_H
