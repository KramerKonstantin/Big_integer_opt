#ifndef BIGINTOPT_BIG_INTEGER_H
#define BIGINTOPT_BIG_INTEGER_H


#include <string>
#include <vector>
#include "my_vector.h"

struct big_integer {
    big_integer();
    big_integer(big_integer const &bi);
    big_integer(int i);
    big_integer(uint32_t ui);
    explicit big_integer(std::string const &str);
    explicit big_integer(bool sign_, my_vector const &v);
    ~big_integer();

    big_integer &operator=(big_integer const &bi);

    friend bool operator==(big_integer const &a, big_integer const &b);
    friend bool operator!=(big_integer const &a, big_integer const &b);
    friend bool operator<(big_integer const &a, big_integer const &b);
    friend bool operator>(big_integer const &a, big_integer const &b);
    friend bool operator<=(big_integer const &a, big_integer const &b);
    friend bool operator>=(big_integer const &a, big_integer const &b);

    friend big_integer operator+(big_integer a, big_integer const &b);
    friend big_integer operator-(big_integer a, big_integer const &b);
    big_integer &operator+=(big_integer const &bi);
    big_integer &operator-=(big_integer const &bi);
    big_integer operator+() const;
    big_integer operator-() const;

    friend big_integer operator*(big_integer a, big_integer const &b);
    big_integer &operator*=(big_integer const &bi);

    friend big_integer operator/(big_integer a, big_integer const &b);
    friend big_integer operator%(big_integer a, big_integer const &b);
    big_integer &operator/=(big_integer const &bi);
    big_integer &operator%=(big_integer const &bi);

    big_integer &operator++();
    big_integer &operator--();
    big_integer operator++(int);
    big_integer operator--(int);

    friend big_integer operator&(big_integer a, big_integer const &b);
    friend big_integer operator|(big_integer a, big_integer const &b);
    friend big_integer operator^(big_integer a, big_integer const &b);
    big_integer &operator&=(big_integer const &bi);
    big_integer &operator|=(big_integer const &bi);
    big_integer &operator^=(big_integer const &bi);
    big_integer operator~() const;

    friend big_integer operator<<(big_integer bi, uint32_t i);
    friend big_integer operator>>(big_integer bi, uint32_t i);
    big_integer &operator<<=(uint32_t ui);
    big_integer &operator>>=(uint32_t ui);

    friend std::string to_string(big_integer const &bi);
    friend big_integer unsigned_div_bi_bi(big_integer const &bi_left, big_integer const &bi_right);
    friend big_integer unsigned_mul_bi_uint(big_integer const &bi, uint32_t const &ui);

private:
    bool is_negative;
    my_vector data;

    void transfer_to_twos_complement();
    void transfer_to_signed_form();
    uint32_t digit(size_t i) const;
    uint32_t digit_for_bit(size_t i) const;
    void make_fit();
    big_integer abs(big_integer const &bi);
};


#endif //BIGINTOPT_BIG_INTEGER_H
