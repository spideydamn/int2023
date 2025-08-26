#include "number.h"
#include<cstring>


char *ToBit(uint8_t digit) {
    // converts a number to binary
    char *bit_digit = new char[8]{};
    uint8_t index_in_bit_digit;

    for (index_in_bit_digit = 0; index_in_bit_digit < 8; ++index_in_bit_digit) { // reset to zeros the byte
        bit_digit[index_in_bit_digit] = '0';
    }

    index_in_bit_digit = 7; // bit index to define it
    while (digit != 0) { // standard algorithm for converting from decimal to binary number system
        if (digit % 2 == 1) {
            bit_digit[index_in_bit_digit] = '1';
        } else {
            bit_digit[index_in_bit_digit] = '0';
        }
        --index_in_bit_digit;
        digit /= 2;
    }

    return bit_digit;
}

char DigitToChar(const unsigned short int digit) {
    // converting numbers to char
    switch (digit) {
        case 0:
            return '0';
        case 1:
            return '1';
        case 2:
            return '2';
        case 3:
            return '3';
        case 4:
            return '4';
        case 5:
            return '5';
        case 6:
            return '6';
        case 7:
            return '7';
        case 8:
            return '8';
        case 9:
            return '9';
        default:
            return '\0';
    }
}

bool IsAllZeros(const char *string) {
    // checking if the string is all zeros
    for (unsigned short int i = 0; i < static_cast<unsigned short int>(strlen(string)); ++i) {
        if (string[i] != '0') {
            return false;
        }
    }

    return true;
}

bool IsAllZeros(int2023_t num) {
    // checking if int2023_t is all zeros
    if (num.digits[0] % 128 != 0) { // we check separately the first byte with the first bit under the sign
        return false;
    }
    for (uint8_t i = 1; i < 253; ++i) {
        if (num.digits[i] != 0) {
            return false;
        }
    }

    return true;
}


unsigned short int DivString256(char *string) {
    // dividing a string by 256 returns the modulo remainder
    short int string_it = 0;
    unsigned short int dividend = 0;

    for (int i = 0; i < strlen(string); ++i) {
        dividend = dividend * 10 + (string[i] - '0');
        if (dividend >= 256) {
            string[string_it] = DigitToChar(dividend / 256);
            dividend %= 256;
        } else {
            string[string_it] = '0';
        }
        ++string_it;
    }
    string[string_it] = '\0';

    return dividend;
}

bool IsPositive(const int2023_t num) {
    // checks if a number is positive
    return num.digits[0] < 128;
}

bool IsNegative(const int2023_t num) {
    // checks if a number is negative
    return num.digits[0] >= 128;
}

int2023_t ChangingSign(int2023_t num) {
    //reverses the sign of the number int2023_t
    if (IsNegative(num)) {
        num.digits[0] -= 128;
    } else {
        num.digits[0] += 128;
    }

    return num;
}

int8_t WhichAbsGreater(const int2023_t &num1, const int2023_t &num2) {
    /*
     * returns 1 if abs(num1) > abs(num2)
     * returns 0 if abs(num1) == abs(num2)
     * returns -1 if abs(num1) < abs(num2)
     */

    // we check separately the first byte with the first bit for the sign
    if (num1.digits[0] % 128 > num2.digits[0] % 128) {
        return 1;
    } else if (num1.digits[0] % 128 < num2.digits[0] % 128) {
        return -1;
    }

    for (int i = 1; i < 253; ++i) { // we go until the first mismatch
        if (num1.digits[i] > num2.digits[i]) {
            return 1;
        } else if (num1.digits[i] < num2.digits[i]) {
            return -1;
        }
    }

    // return zero if the numbers are equal
    return 0;
}

int2023_t LPositiveSNegative(int2023_t positive, const int2023_t negative) {
    // Subtracting a small "negative" from a large positive "positive"

    int2023_t res;
    uint8_t loan_ind = 252;

    for (int i = 252; i >= 1; --i) {
        if (positive.digits[i] >= negative.digits[i]) {
            res.digits[i] = positive.digits[i] - negative.digits[i];
        } else {
            if (loan_ind >= i) {
                /* if the position is pointed to by the loan iterator,
                 * then move it to the next one after the current one */
                loan_ind = i - 1;
            }
            while (positive.digits[loan_ind] == 0) {
                /* We are looking for the first digit
                 * where we can place one in the lower digits */
                positive.digits[loan_ind] = 255;
                --loan_ind;
            }

            positive.digits[loan_ind] -= 1;
            res.digits[i] = 256 + positive.digits[i] - negative.digits[i];
        }
    }

    // we check separately the first byte with the first bit for the sign
    res.digits[0] = positive.digits[0] - (negative.digits[0] - 128);

    return res;
}

int2023_t SPositiveLNegative(const int2023_t positive, int2023_t negative) {
    // Subtracting a large "negative" from a small positive "positive"
    int2023_t res;
    uint8_t loan_ind = 252;

    for (int i = 252; i >= 0; --i) {
        if (negative.digits[i] >= positive.digits[i]) {
            res.digits[i] = negative.digits[i] - positive.digits[i];
        } else {
            if (loan_ind >= i) {
                /* if the position is pointed to by the loan iterator,
                 * then move it to the next one after the current one */
                loan_ind = i - 1;
            }
            while (negative.digits[loan_ind] == 0) {
                /* We are looking for the first digit
                 * where we can place one in the lower digits */
                negative.digits[loan_ind] = 255;
                --loan_ind;
            }

            negative.digits[loan_ind] -= 1;
            res.digits[i] = 256 + negative.digits[i] - positive.digits[i];
        }
    }

    return res;
}

uint8_t Len(const int2023_t &num) {
    // Calculating the number of bytes in a number without taking leading zeros into account

    // we check separately the first byte with the first bit for the sign
    if (num.digits[0] != 128 && num.digits[0] != 0) {
        return 253;
    }

    for (int i = 1; i < 253; ++i) { // looking for the first non-zero
        if (num.digits[i] != 0) {
            return 253 - i;
        }
    }

    return 1;
}

int2023_t ShiftLeft(const int2023_t &num, const uint8_t qt) {
    // shift by qt bytes to the left
    int2023_t res;

    if (qt > 1 && num.digits[qt - 1] != 0) { // if the offset is outside the array
        std::cerr << "Undefined Behavior" << std::endl;
    }

    for (int i = 252; i >= 0; --i) {
        if (i + qt <= 252) {
            res.digits[i] = num.digits[i + qt];
        }
    }

    return res;
}

int2023_t ShiftRight(const int2023_t &num, const uint8_t qt) {
    //shift by qt bytes to the right
    int2023_t res;

    if (qt > 1 && num.digits[253 - qt] != 0) { // if the offset is outside the array
        std::cerr << "Undefined Behavior" << std::endl;
    }

    for (int i = 252; i >= 0; --i) {
        if (i - qt >= 0) {
            res.digits[i] = num.digits[i - qt];
        }
    }

    return res;
}

int2023_t Karatsuba(const int2023_t &lhs, const int2023_t &rhs) {
    // multiplying numbers using the recursive cratsuba algorithm
    uint8_t len_lhs = Len(lhs);
    uint8_t len_rhs = Len(rhs);

    int2023_t res;

    if (len_lhs < 50) { // Multiplying small numbers is faster using a linear algorithm
        for (uint8_t i = 252; i >= 1; --i) {
            int2023_t next_num;
            for (uint8_t j = 252; j >= 1; --j) {
                next_num.digits[j - 1] += (next_num.digits[j] + lhs.digits[i] * rhs.digits[j]) / 256;
                next_num.digits[j] += lhs.digits[i] * rhs.digits[j];
            }
            res = res + ShiftLeft(next_num, 252 - i);
        }
    } else { // recursive Karatsuba algorithm
        uint8_t m = (len_lhs > len_rhs ? len_lhs : len_rhs + 1) / 2;

        int2023_t right_lhs, left_lhs; // senior and junior ranks of lhs
        int2023_t right_rhs, left_rhs; // senior and junior ranks of rhs

        // fill them with values
        for (int i = 252; i >= 253 - m; --i) {
            right_lhs.digits[i] = lhs.digits[i];
        }
        for (int i = 252 - m; i >= 253 - len_lhs; --i) {
            left_lhs.digits[m + i] = lhs.digits[i];
        }

        for (int i = 252; i >= 253 - m; --i) {
            right_rhs.digits[i] = rhs.digits[i];
        }
        for (int i = 252 - m; i >= 253 - len_rhs; --i) {
            left_rhs.digits[m + i] = rhs.digits[i];
        }

        int2023_t sum_lhs_parts = left_lhs + right_lhs;
        int2023_t sum_rhs_parts = left_rhs + right_rhs;

        int2023_t multi_parts = Karatsuba(sum_lhs_parts, sum_rhs_parts);

        int2023_t multi_left_parts = Karatsuba(left_lhs, left_rhs);
        int2023_t multi_right_parts = Karatsuba(right_lhs, right_rhs);

        int2023_t sum_mid_terms = (multi_parts - multi_left_parts) - multi_right_parts;

        res = multi_right_parts + ShiftLeft(sum_mid_terms, m) +
              ShiftLeft(multi_left_parts, 2 * m);

    }

    return res;
}

uint8_t BinSearchDivider(int2023_t &dividend, const int2023_t &rhs) {
    // binary search for the greatest divisor of a divinded number to get rhs
    int l = 0;
    int r = 256;
    int m;

    while (r - l > 1) {
        m = (l + r) / 2;
        if (WhichAbsGreater(rhs * from_int(m), dividend) <= 0) {
            l = m;
        } else {
            r = m;
        }
    }

    return l;
}