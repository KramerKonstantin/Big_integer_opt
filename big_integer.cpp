#include <iostream>
#include "big_integer.h"
#include <climits>

// Default constructor
big_integer::big_integer() : is_negative(false) {}

// Copy constructor
big_integer::big_integer(big_integer const &other) : is_negative(other.is_negative), data(other.data) {}

// Constructor from int
big_integer::big_integer(int a) : is_negative(a < 0) {
    if (a) {
        data.resize(1);
        data[0] = static_cast<uint32_t>(std::abs(a));
    }
}

// Explicit constructor from std::string.
big_integer::big_integer(std::string const &str) {
    data.resize(1);
    data[0] = 0;
    is_negative = false;
    size_t begin = 0;
    if (str[begin] == '-') {
        begin = 1;
        is_negative = true;
    }
    while (str[begin] == '0') {
        begin++;
    }
    for (size_t i = begin; i < str.size(); i++) {
        uint64_t t = 0;
        for (size_t j = 0; j < data.size(); j++) {
            t += (uint64_t) data[j] * (uint64_t) 10;
            data[j] = (uint32_t) (t & UINT32_MAX);
            t >>= 32;
        }
        while (t) {
            data.push_back((uint32_t) (t & UINT32_MAX));
            t >>= 32;
        }
        t = (uint64_t) (str[i] - '0');
        for (size_t j = 0; j < data.size(); j++) {
            t += (uint64_t) data[j];
            data[j] = (uint32_t) (t & UINT32_MAX);
            t >>= 32;
            if (!t) {
                break;
            }
        }
        if (t) {
            data.push_back((uint32_t) t);
        }
    }
    if (is_negative && !data.back()) {
        is_negative = false;
    }
}

// Assignment operator
big_integer &big_integer::operator=(big_integer const &bi) {
    data = bi.data;
    is_negative = bi.is_negative;
    return *this;
}

// Comparision operators
bool operator==(big_integer const &bi_left, big_integer const &bi_right) {
    return ((bi_left.is_negative == bi_right.is_negative) && (bi_left.data == bi_right.data)) || (bi_left.data.empty() && bi_right.data.empty());
}

bool operator!=(big_integer const &bi_left, big_integer const &bi_right) {
    return !(bi_left == bi_right);
}

bool operator>(big_integer const& bi_left, big_integer const& bi_right) {
    if (bi_left == bi_right) return false;
    if (bi_left.is_negative) {
        if (!bi_right.is_negative || bi_left.data.size() > bi_right.data.size()) {
            return false;
        }
        else if (bi_left.data.size() < bi_right.data.size()) {
            return true;
        }
    }
    else {
        if (bi_left.data.size() < bi_right.data.size()) {
            return false;
        }
        else if (bi_right.is_negative || bi_left.data.size() > bi_right.data.size()) {
            return true;
        }
    }
    for (size_t i = bi_left.data.size(); i > 0; i--) {
        if (bi_left.data[i - 1] > bi_right.data[i - 1]) {
            return !bi_left.is_negative;
        }
        else if (bi_left.data[i - 1] < bi_right.data[i - 1]) {
            return bi_left.is_negative;
        }
    }
}

bool operator<(big_integer const& bi_left, big_integer const& bi_right) {
    return (bi_left != bi_right) && !(bi_left > bi_right);
}

bool operator<=(big_integer const& bi_left, big_integer const& bi_right) {
    return !(bi_left > bi_right);
}

bool operator>=(big_integer const& bi_left, big_integer const& bi_right) {
    return !(bi_left < bi_right);
}

// Arithmetic operations
big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    big_integer rv = *this;
    rv.is_negative = !is_negative;
    return rv;
}

big_integer &big_integer::operator+=(big_integer const &bi) {
    if (is_negative != bi.is_negative) {
        *this -= -bi;
        return *this;
    }
    size_t n = std::max(data.size(), bi.data.size());
    my_vector rv(n);
    uint64_t carry = 0, sum = 0;
    for (size_t i = 0; i < n; i++) {
        sum = (carry + digit(i)) + bi.digit(i);
        rv[i] = static_cast<uint32_t>(sum & UINT32_MAX);
        carry = sum >> 32;
    }
    if (carry) {
        rv.push_back(static_cast<uint32_t>(carry));
    }
    my_vector::swap(rv, data);
    make_fit();
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &bi) {
    if (is_negative != bi.is_negative) {
        *this += -bi;
        return *this;
    }
    size_t n = std::max(data.size(), bi.data.size());
    my_vector rv(n, 0);
    const big_integer *min;
    const big_integer *max;
    if (abs(*this) <= abs(bi)) {
        min = this;
        max = &bi;
    } else {
        min = &bi;
        max = this;
    }
    bool borrow = false;
    for (size_t i = 0; i < rv.size(); i++) {
        int64_t cur = max->data[i];
        if (i >= min->data.size()) {
            cur -= static_cast<int64_t>(borrow);
        } else {
            cur -= static_cast<int64_t>(borrow) + min->data[i];
        }
        if (cur < 0) {
            cur += static_cast<uint64_t>(1) << 32;
            borrow = true;
        } else {
            borrow = false;
        }
        rv[i] = static_cast<uint32_t>(cur);
    }
    is_negative = *this < bi;
    my_vector::swap(rv, data);
    make_fit();
    return *this;
}

big_integer operator+(big_integer bi_left, big_integer const &bi_right) {
    return bi_left += bi_right;
}

big_integer operator-(big_integer bi_left, big_integer const &bi_right) {
    return bi_left -= bi_right;
}

// Multiplication
big_integer &big_integer::operator*=(big_integer const &bi) {
    is_negative = is_negative ^ bi.is_negative;
    size_t n = data.size() + bi.data.size();
    my_vector rv(n, 0);
    for (size_t i = 0; i < data.size(); i++) {
        uint32_t carry = 0;
        for (size_t j = 0; j < bi.data.size(); j++) {
            uint64_t cur = static_cast<uint64_t>(data[i]) * bi.data[j] + rv[i + j] + carry;
            rv[i + j] = static_cast<uint32_t>(cur & UINT32_MAX);
            carry = static_cast<uint32_t>(cur >> 32);
        }
        rv[i + bi.data.size()] += carry;
    }
    my_vector::swap(rv, data);
    make_fit();
    return *this;
}

big_integer operator*(big_integer bi_left, big_integer const &bi_right) {
    return bi_left *= bi_right;
}

// Division and modulo
big_integer &big_integer::operator/=(big_integer const &bi) {
    if (bi == 0) {
        throw std::runtime_error("Division by zero");
    }
    bool sign = is_negative ^ bi.is_negative;
    big_integer abs_a = abs(*this);
    big_integer abs_b = abs(bi);
    if (*this == 0 || abs_b > abs_a) {
        *this = 0;
    } else if (bi.data.size() == 1) {
        uint64_t carry = 0, tmp = 0;
        for (size_t i = data.size(); i > 0; i--) {
            tmp = (carry << 32) + data[i - 1];
            data[i - 1] = static_cast<uint32_t>(tmp / bi.data[0]);
            carry = tmp % bi.data[0];
        }
        make_fit();
    } else {
        *this = unsigned_div_bi_bi(*this, bi);
    }
    is_negative = sign;
    return *this;
}

big_integer operator/(big_integer bi_left, big_integer const &bi_right) {
    return bi_left /= bi_right;
}

big_integer &big_integer::operator%=(big_integer const &bi) {
    return *this = *this - (*this / bi) * bi;
}

big_integer operator%(big_integer bi_left, big_integer const &bi_right) {
    return bi_left %= bi_right;
}

// Prefix/postfix increment/decrement
big_integer& big_integer::operator++() {
    // prefix
    return *this += 1;
}

big_integer big_integer::operator++(int) {
    // postfix
    big_integer rv(*this);
    *this += 1;
    return rv;
}

big_integer& big_integer::operator--() {
    // prefix
    return *this -= 1;
}

big_integer big_integer::operator--(int) {
    // postfix
    big_integer rv(*this);
    *this -= 1;
    return rv;
}

// Bit operations
big_integer &big_integer::operator&=(big_integer const &bi) {
    size_t n = std::max(data.size(), bi.data.size());
    my_vector temp(n);
    transfer_to_twos_complement();
    big_integer b(bi);
    b.transfer_to_twos_complement();
    for (size_t i = 0; i < n; i++) {
        temp[i] = digit_for_bit(i) & b.digit_for_bit(i);
    }
    my_vector::swap(temp, data);
    transfer_to_signed_form();
    make_fit();
    return *this;
}

big_integer &big_integer::operator|=(big_integer const &bi) {
    size_t n = std::min(data.size(), bi.data.size());
    my_vector temp(n);
    transfer_to_twos_complement();
    big_integer b(bi);
    b.transfer_to_twos_complement();
    for (size_t i = 0; i < n; i++) {
        temp[i] = digit(i) | b.digit(i);
    }
    my_vector::swap(temp, data);
    transfer_to_signed_form();
    make_fit();
    return *this;
}

big_integer &big_integer::operator^=(big_integer const &bi) {
    size_t n = std::max(data.size(), bi.data.size());
    my_vector temp(n);
    transfer_to_twos_complement();
    big_integer b(bi);
    b.transfer_to_twos_complement();
    for (size_t i = 0; i < n; i++) {
        temp[i] = digit_for_bit(i) ^ b.digit_for_bit(i);
    }
    my_vector::swap(temp, data);
    transfer_to_signed_form();
    make_fit();
    return *this;
}

big_integer operator&(big_integer bi_left, big_integer const &bi_right) {
    return bi_left &= bi_right;
}

big_integer operator|(big_integer bi_left, big_integer const &bi_right) {
    return bi_left |= bi_right;
}

big_integer operator^(big_integer bi_left, big_integer const &bi_right) {
    return bi_left ^= bi_right;
}

big_integer big_integer::operator~() const {
    return -(*this) - 1;
}

// Bit shifts
big_integer &big_integer::operator<<=(uint32_t c) {
    if (c == 0) {
        return *this;
    }
    size_t div = c / 32;
    size_t mod = c % 32;
    size_t new_size = data.size() + div + 1;
    my_vector temp(new_size);
    temp[div] = digit(0) << mod;
    for (size_t i = div + 1; i < new_size; i++) {
        temp[i] = (digit(i - div) << mod) | static_cast<uint32_t>(digit(i - div - 1)) >> (32 - mod);
    }
    *this = big_integer(is_negative, temp);
    make_fit();
    return *this;
}

big_integer &big_integer::operator>>=(uint32_t c) {
    if (c == 0) {
        return *this;
    }
    size_t div = c / 32;
    size_t mod = c % 32;
    size_t new_size = div < data.size() ? data.size() - div : 0;
    my_vector tmp(new_size);
    for (size_t i = 0; i < new_size; i++) {
        tmp[i] = (static_cast<uint32_t>(digit(i + div)) >> mod)
                 | (static_cast<uint32_t>(digit(i + div + 1)) << (32 - mod));
    }
    *this = big_integer(is_negative, tmp);
    make_fit();
    if (is_negative) {
        *this -= 1;
    }
    return *this;
}

big_integer operator<<(big_integer bi, uint32_t ui) {
    return bi <<= ui;
}

big_integer operator>>(big_integer bi, uint32_t ui) {
    return bi >>= ui;
}

// Additional
big_integer::~big_integer() = default;

big_integer::big_integer(uint32_t a) : is_negative(false) {
    if (a) {
        data.resize(1);
        data[0] = a;
    }
}

void big_integer::make_fit() {
    while (!data.empty() && data.back() == 0) {
        data.pop_back();
    }
    if (data.empty()) {
        is_negative = false;
    }
}

big_integer::big_integer(bool new_sign, my_vector const &new_data) : is_negative(new_sign), data(new_data) {
    make_fit();
}

uint32_t big_integer::digit(size_t index) const {
    if (index < data.size()) {
        return data[index];
    } else {
        return 0;
    }
}

uint32_t big_integer::digit_for_bit(size_t index) const {
    if (index < data.size()) {
        return data[index];
    } else {
        return UINT32_MAX;
    }
}

big_integer big_integer::abs(big_integer const &a) {
    big_integer res(a);
    res.is_negative = false;
    return res;
}

big_integer unsigned_div_bi_bi(big_integer const &bi_left, big_integer const &bi_right) {
    uint64_t scaling = (static_cast<uint64_t>(UINT32_MAX) + 1)
            / (static_cast<uint64_t>(bi_right.data[bi_right.data.size() - 1]) + 1);
    big_integer a = unsigned_mul_bi_uint(bi_left, static_cast<uint32_t>(scaling));
    big_integer b = unsigned_mul_bi_uint(bi_right, static_cast<uint32_t>(scaling));
    my_vector div(a.data.size() - b.data.size() + 1);
    big_integer cur;
    cur.data.resize(b.data.size() - 1);
    for (size_t i = a.data.size() - b.data.size() + 1, j = 0; i < a.data.size(); i++, j++) {
        cur.data[j] = a.data[i];
    }
    uint64_t b0 = b.data[b.data.size() - 1];
    for (size_t i = div.size(); i > 0; i--) {
        cur.data.push_front(a.data[i - 1]);
        uint64_t a1 = cur.data[cur.data.size() - 1];
        if (cur.data.size() > b.data.size()) {
            a1 = (a1 << 32) + cur.data[cur.data.size() - 2];
        }
        uint64_t coefficient = a1 / b0;
        if (coefficient >= UINT32_MAX) {
            coefficient = UINT32_MAX;
        }
        big_integer mul_dq = b * static_cast<uint32_t>(coefficient);
        while (cur < mul_dq) {
            coefficient--;
            mul_dq -= b;
        }
        div[i - 1] = static_cast<uint32_t>(coefficient & UINT32_MAX);
        cur -=mul_dq;
    }
    my_vector::swap(a.data, div);
    a.make_fit();
    return a;
}

big_integer unsigned_mul_bi_uint(big_integer const &bi, uint32_t const &ui) {
        big_integer mul;
        mul.data.resize(bi.data.size());
        uint32_t  carry = 0;
        for (size_t i = 0; i < bi.data.size(); i++) {
            uint64_t tmp = static_cast<uint64_t>(bi.data[i]) * ui + carry;
            mul.data[i] = static_cast<uint32_t>(tmp & UINT32_MAX);
            carry = static_cast<uint32_t>(tmp >> 32);
        }
        mul.data.push_back(carry);
        mul.make_fit();
        return mul;
}

void big_integer::transfer_to_twos_complement() {
    if (is_negative) {
        is_negative = false;
        for (size_t i = 0; i < data.size(); i++) {
            data[i] = ~data[i];
        }
        *this += 1;
    }

}

void big_integer::transfer_to_signed_form() {
    bool sign = (data[data.size() - 1] >> 31 == 1);
    if (sign) {
        for (size_t i = 0; i < data.size(); i++) {
            data[i] = ~data[i];
        }
        is_negative = false;
        *this += 1;
        is_negative = true;
    }
}

std::string to_string(big_integer const &bi) {
    if (bi.data.empty()) {
        return "0";
    }
    big_integer a(bi);
    std::string rv;
    while (a != 0) {
        big_integer b = a % 10;
        if (b == 0) {
            std::string str = '0' + rv;
            rv = str;
        } else {
            std::string str = std::to_string(b.data[0]) + rv;
            rv = str;
        }
        a /= 10;
    }
    if (bi.is_negative) {
        rv = '-' + rv;
    }
    return rv;
}


