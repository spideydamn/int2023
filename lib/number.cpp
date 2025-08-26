#include "number.h"
#include "secondary_functions.cpp"


int2023_t from_int(int32_t i) {
    bool is_negative = false;
    if (i < 0) {
        is_negative = true;
    }

    int2023_t res;
    uint8_t digit_number = 252;
    while (i != 0) {
        res.digits[digit_number] = abs(i) % 256;
        i /= 256;
        --digit_number;
    }

    if (is_negative) {
        res.digits[0] += 128; // the first bit stores zero
    }

    return res;
}

int2023_t from_string(const char *buff) {
    bool is_negative = false;
    if (buff[0] == '-') {
        ++buff;
        is_negative = true;
    }

    int2023_t res;
    uint8_t digit_number = 252;
    char string[strlen(buff)];
    strcpy(string, buff);
    while (!IsAllZeros(string)) { // conversion from 256-digit number system to binary
        res.digits[digit_number] = DivString256(string);
        --digit_number;
    }
    if (is_negative) {
        res.digits[0] += 128; // the first bit stores zero
    }
    return res;
}

int2023_t operator+(const int2023_t &lhs, const int2023_t &rhs) {
    int2023_t res;

    if (IsNegative(lhs) && IsNegative(rhs) ||
        IsPositive(lhs) && IsPositive(rhs)) { // if numbers of identical signs are simply added by digits
        for (int i = 252; i >= 1; --i) {
            /* if the resulting next number is greater than the base of the number system 256,
             * then we transfer the integer from division to the next digit */
            res.digits[i - 1] += (res.digits[i] + lhs.digits[i] + rhs.digits[i]) / 256;
            res.digits[i] += (lhs.digits[i] + rhs.digits[i]);
        }

        // checking the first byte taking into account the sign in the first bit
        if ((lhs.digits[0] % 128) + (rhs.digits[0] % 128) + (res.digits[0]) < 128) {
            res.digits[0] +=
                    (lhs.digits[0] / 128) * 128 + (lhs.digits[0] % 128) + (rhs.digits[0] % 128) + (res.digits[0]);
        } else {
            std::cerr << "Undefined Behavior";
        }
    } else if (IsPositive(lhs) && IsNegative(rhs)) { // if the signs are different
        if (WhichAbsGreater(lhs, rhs) == 1) { // if abs(lhs) > abs(rhs)
            res = LPositiveSNegative(lhs, rhs); // subtract the smaller from the larger
        } else if (WhichAbsGreater(lhs, rhs) == -1) { // if abs(lhs) < abs(rhs)
            res = SPositiveLNegative(lhs, rhs); // subtract the larger from the smaller
        }
    } else if (IsNegative(lhs) && IsPositive(rhs)) { // if the signs are different
        if (WhichAbsGreater(lhs, rhs) == 1) { // if abs(lhs) > abs(rhs)
            res = SPositiveLNegative(rhs, lhs); // subtract the larger from the smaller
        } else if (WhichAbsGreater(lhs, rhs) == -1) { // if abs(lhs) < abs(rhs)
            res = LPositiveSNegative(rhs, lhs); // subtract the smaller from the larger
        }
    }
    return res;
}

int2023_t operator-(const int2023_t &lhs, const int2023_t &rhs) {
    return lhs + ChangingSign(rhs); // addition with the opposite sign of the second number
}

int2023_t operator*(const int2023_t &lhs, const int2023_t &rhs) {
    int2023_t res = Karatsuba(lhs, rhs); // multiplication by karatsuba algorithm
    if (IsNegative(lhs) && IsPositive(rhs) || IsNegative(rhs) && IsPositive(lhs)) {
        res = ChangingSign(res); // if the numbers are of different signs, change the sign
    }
    return res;
}

int2023_t operator/(const int2023_t &lhs, const int2023_t &rhs) {
    int2023_t res = lhs;
    res.digits[0] %= 128;
    short int res_it = 0;

    int2023_t dividend;

    for (uint8_t digit: res.digits) { // linear division algorithm
        dividend = ShiftLeft(dividend, 1);
        dividend.digits[252] = digit;
        if (WhichAbsGreater(dividend, rhs) >= 0) {

            res.digits[res_it] = BinSearchDivider(dividend, rhs); // finding divisor "dividend" to get "rhs"
            dividend = dividend - (rhs * from_int(res.digits[res_it])); // modulo of the division

        } else {
            res.digits[res_it] = 0; // take the next digit
        }
        ++res_it;
    }
    ShiftRight(res, res_it); // move the resulting res from scratch, pressing it to the left

    if ((IsNegative(lhs) && IsPositive(rhs) || IsNegative(rhs) && IsPositive(lhs)) && !IsAllZeros(res)) {
        res = ChangingSign(res); // if the numbers are of different signs and res != 0, change the sign
    }

    return res;
}

bool operator==(const int2023_t &lhs, const int2023_t &rhs) {
    for (int i = 0; i < 253; ++i) { // we go through the array until the first inequality
        if (lhs.digits[i] != rhs.digits[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const int2023_t &lhs, const int2023_t &rhs) {
    for (int i = 0; i < 253; ++i) { // we go through the array until the first inequality
        if (lhs.digits[i] != rhs.digits[i]) {
            return true;
        }
    }
    return false;
}

std::ostream &operator<<(std::ostream &stream, const int2023_t &value) {
    if (IsNegative(value)) {
        stream << "-"; // print minus if negative
        if (value.digits[0] > 128) {
            /* if the same byte contains the beginning of a number in addition to a signed bit,
             * then print the beginning of the number as well */
            stream << value.digits[0] - 128;
        }
    }

    char *bit_digit;
    uint8_t index_in_bit_digit;
    bool was_bit_end_of_zeros = false;


    for (uint8_t i = 253 - Len(value); i < 253; ++i) {
        bit_digit = ToBit(value.digits[i]);
        for (index_in_bit_digit = 0; index_in_bit_digit < 8; ++index_in_bit_digit) {
            if (!was_bit_end_of_zeros && bit_digit[index_in_bit_digit] == '1') {
                // check if the number has started so as not to print leading zeros
                was_bit_end_of_zeros = true;
            }
            if (was_bit_end_of_zeros) {
                stream << bit_digit[index_in_bit_digit];
            }
        }
    }
    if (!was_bit_end_of_zeros) { // if the number is zero, print zero
        stream << 0;
    }

    return stream;
}
