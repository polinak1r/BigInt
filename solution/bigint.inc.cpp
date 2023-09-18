#include <algorithm>
#include <iostream>
#include <vector>

const int BASE = 1000;
const int FIB_10 = []() {
    int i = 0;
    int j = BASE;
    while (j > 1) {
        i++;
        j /= 10;
    }
    return i;
}();

struct bigint {
private:
    std::vector<unsigned int> digits;

    void remove_leading_zeros() {
        while (digits.size() > 1 && digits.back() == 0) {
            digits.pop_back();
        }
    }

public:
    explicit bigint(const std::string &str) {
        unsigned int value = 0;
        unsigned int multiplier = 1;

        for (auto it = str.rbegin(); it != str.rend(); ++it) {
            if (isdigit(*it) != 0) {
                value += (*it - '0') * multiplier;
                multiplier *= 10;
                if (multiplier == BASE) {
                    digits.push_back(value);
                    value = 0;
                    multiplier = 1;
                }
            }
        }

        if (value != 0 || digits.empty()) {
            digits.push_back(value);
        }

        remove_leading_zeros();
    }

    bigint(unsigned int num = 0) {
        do {
            digits.push_back(num % BASE);
            num /= BASE;
        } while (num != 0);
    }

    bool operator==(const bigint &other) const {
        return digits == other.digits;
    }

    bool operator!=(const bigint &other) const {
        return !(*this == other);
    }

    bool operator<(const bigint &other) const {
        if (digits.size() != other.digits.size()) {
            return digits.size() < other.digits.size();
        }
        for (std::size_t i = digits.size() - 1; i > 0; --i) {
            if (digits[i] != other.digits[i]) {
                return digits[i] < other.digits[i];
            }
        }

        if (digits[0] != other.digits[0]) {
            return digits[0] < other.digits[0];
        }

        return false;
    }

    bool operator>(const bigint &other) const {
        return other < *this;
    }

    bool operator<=(const bigint &other) const {
        return !(*this > other);
    }

    bool operator>=(const bigint &other) const {
        return !(*this < other);
    }

    [[nodiscard]] std::string to_string() const {
        const std::string zeros(FIB_10, '0');
        std::string result;
        if (digits.empty()) {
            return "0";
        }
        for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
            result += zeros.substr(0, FIB_10 - (std::to_string(*it)).size()) +
                      std::to_string(*it);
        }
        auto it_zero = std::find_if_not(
            result.begin(), std::prev(result.end()),
            [](char c) { return (c == '0'); }
        );

        result.erase(result.begin(), it_zero);
        return result;
    }

    explicit operator unsigned int() const {
        unsigned int result = 0;
        unsigned int multiplier = 1;
        for (unsigned int digit : digits) {
            result += digit * multiplier;
            multiplier *= BASE;
        }
        return result;
    }

    bigint &operator+=(const bigint &rhs) {
        unsigned int carry = 0;

        bigint rhs_copy = rhs;

        if (rhs_copy.digits.size() > digits.size()) {
            digits.resize(rhs_copy.digits.size(), 0);
        }

        auto it_rhs = rhs_copy.digits.begin();
        for (auto it = digits.begin();
             it != digits.end() || it_rhs != rhs_copy.digits.end();) {
            *it += carry;
            carry = 0;
            if (it_rhs != rhs_copy.digits.end()) {
                *it += *it_rhs;
            }
            if (*it >= BASE) {
                *it -= BASE;
                carry = 1;
            }
            if (it != digits.end()) {
                it++;
            }
            if (it_rhs != rhs_copy.digits.end()) {
                it_rhs++;
            }
        }
        if (carry == 1) {
            digits.push_back(1);
        }
        return *this;
    }

    friend bigint operator+(bigint lhs, const bigint &rhs) {
        lhs += rhs;
        return lhs;
    }

    bigint &operator-=(const bigint &rhs) {
        unsigned int carry = 0;
        bigint rhs_copy = rhs;
        auto it_rhs = rhs_copy.digits.begin();
        for (auto it = digits.begin();
             it != digits.end() || it_rhs != rhs_copy.digits.end();) {
            if (it_rhs == rhs_copy.digits.end() && carry == 1 && *it != 0) {
                *it -= carry;
                carry = 0;
            }
            if (it_rhs == rhs_copy.digits.end() && carry == 1 && *it == 0) {
                *it = BASE - 1;
            }
            if (it_rhs != rhs_copy.digits.end()) {
                if (*it >= (carry + *it_rhs)) {
                    *it -= (carry + *it_rhs);
                    carry = 0;
                } else {
                    *it = *it + BASE - carry - *it_rhs;
                    carry = 1;
                }
            }

            if (it != digits.end()) {
                it++;
            }
            if (it_rhs != rhs_copy.digits.end()) {
                it_rhs++;
            }
        }
        remove_leading_zeros();
        return *this;
    }

    friend bigint operator-(bigint lhs, const bigint &rhs) {
        lhs -= rhs;
        return lhs;
    }

    bigint &operator++() {
        *this += 1;
        return *this;
    }

    bigint operator++(int) {
        bigint temp(*this);
        operator++();
        return temp;
    }

    bigint &operator--() {
        *this -= 1;
        return *this;
    }

    bigint operator--(int) {
        bigint temp(*this);
        operator--();
        return temp;
    }
};

bool operator==(unsigned int lhs, const bigint &rhs) {
    return rhs == lhs;
}

bool operator!=(unsigned int lhs, const bigint &rhs) {
    return rhs != lhs;
}

bool operator<(unsigned int lhs, const bigint &rhs) {
    return rhs > lhs;
}

bool operator>(unsigned int lhs, const bigint &rhs) {
    return rhs < lhs;
}

bool operator<=(unsigned int lhs, const bigint &rhs) {
    return !(lhs > rhs);
}

bool operator>=(unsigned int lhs, const bigint &rhs) {
    return !(lhs < rhs);
}
