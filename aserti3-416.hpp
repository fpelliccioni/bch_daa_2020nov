#ifndef ASERTI3_416_HPP_
#define ASERTI3_416_HPP_

#include <cassert>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>

/** Template base class for fixed-sized opaque blobs. */
template <unsigned int BITS> class base_blob {
protected:
    static constexpr int WIDTH = BITS / 8;
    uint8_t data[WIDTH];

public:
    constexpr base_blob() noexcept : data{0} {}

    /// type tag + convenience member for uninitialized c'tor
    static constexpr struct Uninitialized_t {} Uninitialized{};

    /// Uninitialized data constructor -- to be used when we want to avoid a
    /// redundant zero-initialization in cases where we know we will fill-in
    /// the data immediately anyway (e.g. for random generators, etc).
    /// Select this c'tor with e.g.: uint256 foo{uint256::Uninitialized}
    explicit constexpr base_blob(Uninitialized_t /* type tag to select this c'tor */) noexcept {}

//     explicit base_blob(const std::vector<uint8_t> &vch) noexcept;

//     bool IsNull() const {
//         for (int i = 0; i < WIDTH; i++) {
//             if (data[i] != 0) {
//                 return false;
//             }
//         }
//         return true;
//     }

//     void SetNull() { memset(data, 0, sizeof(data)); }

//     inline int Compare(const base_blob &other) const {
//         for (size_t i = 0; i < sizeof(data); i++) {
//             uint8_t a = data[sizeof(data) - 1 - i];
//             uint8_t b = other.data[sizeof(data) - 1 - i];
//             if (a > b) {
//                 return 1;
//             }
//             if (a < b) {
//                 return -1;
//             }
//         }

//         return 0;
//     }

//     friend inline bool operator==(const base_blob &a, const base_blob &b) {
//         return a.Compare(b) == 0;
//     }
//     friend inline bool operator!=(const base_blob &a, const base_blob &b) {
//         return a.Compare(b) != 0;
//     }
//     friend inline bool operator<(const base_blob &a, const base_blob &b) {
//         return a.Compare(b) < 0;
//     }
//     friend inline bool operator<=(const base_blob &a, const base_blob &b) {
//         return a.Compare(b) <= 0;
//     }
//     friend inline bool operator>(const base_blob &a, const base_blob &b) {
//         return a.Compare(b) > 0;
//     }
//     friend inline bool operator>=(const base_blob &a, const base_blob &b) {
//         return a.Compare(b) >= 0;
//     }

//     std::string GetHex() const;
    void SetHex(const char *psz);
    void SetHex(const std::string &str);
//     std::string ToString() const { return GetHex(); }

    uint8_t *begin() { return &data[0]; }

//     uint8_t *end() { return &data[WIDTH]; }

    const uint8_t *begin() const { return &data[0]; }

//     const uint8_t *end() const { return &data[WIDTH]; }

//     static constexpr unsigned int size() { return sizeof(data); }

//     uint64_t GetUint64(int pos) const {
//         const uint8_t *ptr = data + pos * 8;
//         return uint64_t(ptr[0]) | (uint64_t(ptr[1]) << 8) |
//                (uint64_t(ptr[2]) << 16) | (uint64_t(ptr[3]) << 24) |
//                (uint64_t(ptr[4]) << 32) | (uint64_t(ptr[5]) << 40) |
//                (uint64_t(ptr[6]) << 48) | (uint64_t(ptr[7]) << 56);
//     }

//     template <typename Stream> void Serialize(Stream &s) const {
//         s.write((char *)data, sizeof(data));
//     }

//     template <typename Stream> void Unserialize(Stream &s) {
//         s.read((char *)data, sizeof(data));
//     }
};

const signed char p_util_hexdigit[256] = {
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    0,  1,   2,   3,   4,   5,   6,   7,  8,  9,  -1, -1, -1, -1, -1, -1,
    -1, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

inline
signed char HexDigit(char c) {
    return p_util_hexdigit[(uint8_t)c];
}

/**
 * Converts the given character to its lowercase equivalent.
 * This function is locale independent. It only converts uppercase
 * characters in the standard 7-bit ASCII range.
 * @param[in] c     the character to convert to lowercase.
 * @return          the lowercase equivalent of c; or the argument
 *                  if no conversion is possible.
 */
constexpr uint8_t ToLower(uint8_t c) {
    return (c >= 'A' && c <= 'Z' ? (c - 'A') + 'a' : c);
}

/**
 * Tests if the given character is a whitespace character. The whitespace
 * characters are: space, form-feed ('\f'), newline ('\n'), carriage return
 * ('\r'), horizontal tab ('\t'), and vertical tab ('\v').
 *
 * This function is locale independent. Under the C locale this function gives
 * the same result as std::isspace.
 *
 * @param[in] c     character to test
 * @return          true if the argument is a whitespace character; otherwise
 * false
 */
constexpr inline bool IsSpace(char c) noexcept {
    return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' ||
           c == '\v';
}

template <unsigned int BITS> void base_blob<BITS>::SetHex(const char *psz) {
    memset(data, 0, sizeof(data));

    // skip leading spaces
    while (IsSpace(*psz)) {
        psz++;
    }

    // skip 0x
    if (psz[0] == '0' && ToLower(uint8_t(psz[1])) == 'x') {
        psz += 2;
    }

    // hex string to uint
    const char *pbegin = psz;
    while (::HexDigit(*psz) != -1) {
        psz++;
    }

    psz--;
    uint8_t *p1 = (uint8_t *)data;
    uint8_t *pend = p1 + WIDTH;
    while (psz >= pbegin && p1 < pend) {
        *p1 = ::HexDigit(*psz--);
        if (psz >= pbegin) {
            *p1 |= uint8_t(::HexDigit(*psz--) << 4);
            p1++;
        }
    }
}

template <unsigned int BITS>
void base_blob<BITS>::SetHex(const std::string &str) {
    SetHex(str.c_str());
}


/**
 * 256-bit opaque blob.
 * @note This type is called uint256 for historical reasons only. It is an
 * opaque blob of 256 bits and has no integer operations. Use arith_uint256 if
 * those are required.
 */
class uint256 : public base_blob<256> {
public:
    using base_blob<256>::base_blob; ///< inherit constructors
};

/**
 * A BlockHash is a unqiue identifier for a block.
 */
struct BlockHash : public uint256 {
    explicit BlockHash() : uint256() {}
    explicit BlockHash(const uint256 &b) : uint256(b) {}

    static BlockHash fromHex(const std::string &str) {
        BlockHash r;
        r.SetHex(str);
        return r;
    }
};

/**
 * uint256 from const char *.
 * This is a separate function because the constructor uint256(const char*) can
 * result in dangerously catching uint256(0).
 */
inline uint256 uint256S(const char *str) {
    uint256 rv{uint256::Uninitialized};
    rv.SetHex(str);
    return rv;
}

/**
 * uint256 from std::string.
 * This is a separate function because the constructor uint256(const std::string
 * &str) can result in dangerously catching uint256(0) via std::string(const
 * char*).
 */
inline uint256 uint256S(const std::string &str) {
    uint256 rv{uint256::Uninitialized};
    rv.SetHex(str);
    return rv;
}


// ---------------------------------------------------------------------------------------------------

/** Template base class for unsigned big integers. */
template <unsigned int BITS> class base_uint {
protected:
    static constexpr int WIDTH = BITS / 32;
    uint32_t pn[WIDTH];

public:
    base_uint() {
        static_assert(
            BITS / 32 > 0 && BITS % 32 == 0,
            "Template parameter BITS must be a positive multiple of 32.");

        for (int i = 0; i < WIDTH; i++) {
            pn[i] = 0;
        }
    }

    base_uint(const base_uint &b) {
        static_assert(
            BITS / 32 > 0 && BITS % 32 == 0,
            "Template parameter BITS must be a positive multiple of 32.");

        for (int i = 0; i < WIDTH; i++) {
            pn[i] = b.pn[i];
        }
    }

    base_uint &operator=(const base_uint &b) {
        for (int i = 0; i < WIDTH; i++) {
            pn[i] = b.pn[i];
        }
        return *this;
    }

    base_uint(uint64_t b) {
        static_assert(
            BITS / 32 > 0 && BITS % 32 == 0,
            "Template parameter BITS must be a positive multiple of 32.");

        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++) {
            pn[i] = 0;
        }
    }

//     explicit base_uint(const std::string &str);

    const base_uint operator~() const {
        base_uint ret;
        for (int i = 0; i < WIDTH; i++) {
            ret.pn[i] = ~pn[i];
        }
        return ret;
    }

    const base_uint operator-() const {
        base_uint ret;
        for (int i = 0; i < WIDTH; i++) {
            ret.pn[i] = ~pn[i];
        }
        ++ret;
        return ret;
    }

//     double getdouble() const;

    base_uint &operator=(uint64_t b) {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++) {
            pn[i] = 0;
        }
        return *this;
    }

//     base_uint &operator^=(const base_uint &b) {
//         for (int i = 0; i < WIDTH; i++) {
//             pn[i] ^= b.pn[i];
//         }
//         return *this;
//     }

//     base_uint &operator&=(const base_uint &b) {
//         for (int i = 0; i < WIDTH; i++) {
//             pn[i] &= b.pn[i];
//         }
//         return *this;
//     }

//     base_uint &operator|=(const base_uint &b) {
//         for (int i = 0; i < WIDTH; i++) {
//             pn[i] |= b.pn[i];
//         }
//         return *this;
//     }

//     base_uint &operator^=(uint64_t b) {
//         pn[0] ^= (unsigned int)b;
//         pn[1] ^= (unsigned int)(b >> 32);
//         return *this;
//     }

//     base_uint &operator|=(uint64_t b) {
//         pn[0] |= (unsigned int)b;
//         pn[1] |= (unsigned int)(b >> 32);
//         return *this;
//     }

    base_uint &operator<<=(unsigned int shift);
    base_uint &operator>>=(unsigned int shift);

    base_uint &operator+=(const base_uint &b) {
        uint64_t carry = 0;
        for (int i = 0; i < WIDTH; i++) {
            uint64_t n = carry + pn[i] + b.pn[i];
            pn[i] = n & 0xffffffff;
            carry = n >> 32;
        }
        return *this;
    }

    base_uint &operator-=(const base_uint &b) {
        *this += -b;
        return *this;
    }

//     base_uint &operator+=(uint64_t b64) {
//         base_uint b;
//         b = b64;
//         *this += b;
//         return *this;
//     }

//     base_uint &operator-=(uint64_t b64) {
//         base_uint b;
//         b = b64;
//         *this += -b;
//         return *this;
//     }

    base_uint &operator*=(uint32_t b32);
//     base_uint &operator*=(const base_uint &b);
    base_uint &operator/=(const base_uint &b);

    base_uint &operator++() {
        // prefix operator
        int i = 0;
        while (i < WIDTH && ++pn[i] == 0) {
            i++;
        }
        return *this;
    }

//     const base_uint operator++(int) {
//         // postfix operator
//         const base_uint ret = *this;
//         ++(*this);
//         return ret;
//     }

//     base_uint &operator--() {
//         // prefix operator
//         int i = 0;
//         while (i < WIDTH && --pn[i] == std::numeric_limits<uint32_t>::max()) {
//             i++;
//         }
//         return *this;
//     }

//     const base_uint operator--(int) {
//         // postfix operator
//         const base_uint ret = *this;
//         --(*this);
//         return ret;
//     }

    int CompareTo(const base_uint &b) const;
//     bool EqualTo(uint64_t b) const;

    friend inline const base_uint operator+(const base_uint &a,
                                            const base_uint &b) {
        return base_uint(a) += b;
    }
//     friend inline const base_uint operator-(const base_uint &a,
//                                             const base_uint &b) {
//         return base_uint(a) -= b;
//     }
//     friend inline const base_uint operator*(const base_uint &a,
//                                             const base_uint &b) {
//         return base_uint(a) *= b;
//     }
    friend inline const base_uint operator/(const base_uint &a,
                                            const base_uint &b) {
        return base_uint(a) /= b;
    }
//     friend inline const base_uint operator|(const base_uint &a,
//                                             const base_uint &b) {
//         return base_uint(a) |= b;
//     }
//     friend inline const base_uint operator&(const base_uint &a,
//                                             const base_uint &b) {
//         return base_uint(a) &= b;
//     }
//     friend inline const base_uint operator^(const base_uint &a,
//                                             const base_uint &b) {
//         return base_uint(a) ^= b;
//     }
    friend inline const base_uint operator>>(const base_uint &a, int shift) {
        return base_uint(a) >>= shift;
    }
    friend inline const base_uint operator<<(const base_uint &a, int shift) {
        return base_uint(a) <<= shift;
    }
    friend inline const base_uint operator*(const base_uint &a, uint32_t b) {
        return base_uint(a) *= b;
    }
    friend inline bool operator==(const base_uint &a, const base_uint &b) {
        return memcmp(a.pn, b.pn, sizeof(a.pn)) == 0;
    }
    friend inline bool operator!=(const base_uint &a, const base_uint &b) {
        return memcmp(a.pn, b.pn, sizeof(a.pn)) != 0;
    }
    friend inline bool operator>(const base_uint &a, const base_uint &b) {
        return a.CompareTo(b) > 0;
    }
//     friend inline bool operator<(const base_uint &a, const base_uint &b) {
//         return a.CompareTo(b) < 0;
//     }
    friend inline bool operator>=(const base_uint &a, const base_uint &b) {
        return a.CompareTo(b) >= 0;
    }
//     friend inline bool operator<=(const base_uint &a, const base_uint &b) {
//         return a.CompareTo(b) <= 0;
//     }
//     friend inline bool operator==(const base_uint &a, uint64_t b) {
//         return a.EqualTo(b);
//     }
//     friend inline bool operator!=(const base_uint &a, uint64_t b) {
//         return !a.EqualTo(b);
//     }

//     std::string GetHex() const;
    // void SetHex(const char *psz);
    // void SetHex(const std::string &str);
//     std::string ToString() const;

//     unsigned int size() const { return sizeof(pn); }

    /**
     * Returns the position of the highest bit set plus one, or zero if the
     * value is zero.
     */
    unsigned int bits() const;

    uint64_t GetLow64() const {
        static_assert(WIDTH >= 2, "Assertion WIDTH >= 2 failed (WIDTH = BITS / "
                                  "32). BITS is a template parameter.");
        return pn[0] | uint64_t(pn[1]) << 32;
    }
};

template <unsigned int BITS>
int base_uint<BITS>::CompareTo(const base_uint<BITS> &b) const {
    for (int i = WIDTH - 1; i >= 0; i--) {
        if (pn[i] < b.pn[i]) {
            return -1;
        }
        if (pn[i] > b.pn[i]) {
            return 1;
        }
    }
    return 0;
}

template <unsigned int BITS> unsigned int base_uint<BITS>::bits() const {
    for (int pos = WIDTH - 1; pos >= 0; pos--) {
        if (pn[pos]) {
            for (int nbits = 31; nbits > 0; nbits--) {
                if (pn[pos] & 1U << nbits) {
                    return 32 * pos + nbits + 1;
                }
            }
            return 32 * pos + 1;
        }
    }
    return 0;
}

template <unsigned int BITS>
base_uint<BITS> &base_uint<BITS>::operator<<=(unsigned int shift) {
    base_uint<BITS> a(*this);
    for (int i = 0; i < WIDTH; i++) {
        pn[i] = 0;
    }
    int k = shift / 32;
    shift = shift % 32;
    for (int i = 0; i < WIDTH; i++) {
        if (i + k + 1 < WIDTH && shift != 0) {
            pn[i + k + 1] |= (a.pn[i] >> (32 - shift));
        }
        if (i + k < WIDTH) {
            pn[i + k] |= (a.pn[i] << shift);
        }
    }
    return *this;
}

template <unsigned int BITS>
base_uint<BITS> &base_uint<BITS>::operator>>=(unsigned int shift) {
    base_uint<BITS> a(*this);
    for (int i = 0; i < WIDTH; i++) {
        pn[i] = 0;
    }
    int k = shift / 32;
    shift = shift % 32;
    for (int i = 0; i < WIDTH; i++) {
        if (i - k - 1 >= 0 && shift != 0) {
            pn[i - k - 1] |= (a.pn[i] << (32 - shift));
        }
        if (i - k >= 0) {
            pn[i - k] |= (a.pn[i] >> shift);
        }
    }
    return *this;
}

template <unsigned int BITS>
base_uint<BITS> &base_uint<BITS>::operator*=(uint32_t b32) {
    uint64_t carry = 0;
    for (int i = 0; i < WIDTH; i++) {
        uint64_t n = carry + (uint64_t)b32 * pn[i];
        pn[i] = n & 0xffffffff;
        carry = n >> 32;
    }
    return *this;
}

template <unsigned int BITS>
base_uint<BITS> &base_uint<BITS>::operator/=(const base_uint &b) {
    // make a copy, so we can shift.
    base_uint<BITS> div = b;
    // make a copy, so we can subtract.
    base_uint<BITS> num = *this;
    // the quotient.
    *this = 0;
    int num_bits = num.bits();
    int div_bits = div.bits();
    if (div_bits == 0) {
        // throw uint_error("Division by zero");
        throw std::runtime_error("Division by zero");
    }
    // the result is certainly 0.
    if (div_bits > num_bits) {
        return *this;
    }
    int shift = num_bits - div_bits;
    // shift so that div and num align.
    div <<= shift;
    while (shift >= 0) {
        if (num >= div) {
            num -= div;
            // set a bit of the result.
            pn[shift / 32] |= (1 << (shift & 31));
        }
        // shift back.
        div >>= 1;
        shift--;
    }
    // num now contains the remainder of the division.
    return *this;
}

// template <unsigned int BITS> void base_uint<BITS>::SetHex(const char *psz) {
//     *this = UintToArith256(uint256S(psz));
// }

// template <unsigned int BITS>
// void base_uint<BITS>::SetHex(const std::string &str) {
//     SetHex(str.c_str());
// }

// ---------------------------------------------------------------------------------------------------


/** 256-bit unsigned big integer. */
class arith_uint256 : public base_uint<256> {
public:
    arith_uint256() {}
    arith_uint256(const base_uint<256> &b) : base_uint<256>(b) {}
    arith_uint256(uint64_t b) : base_uint<256>(b) {}
    // explicit arith_uint256(const std::string &str) : base_uint<256>(str) {}

    /**
     * The "compact" format is a representation of a whole number N using an
     * unsigned 32bit number similar to a floating point format.
     * The most significant 8 bits are the unsigned exponent of base 256.
     * This exponent can be thought of as "number of bytes of N".
     * The lower 23 bits are the mantissa.
     * Bit number 24 (0x800000) represents the sign of N.
     * N = (-1^sign) * mantissa * 256^(exponent-3)
     *
     * Satoshi's original implementation used BN_bn2mpi() and BN_mpi2bn().
     * MPI uses the most significant bit of the first byte as sign.
     * Thus 0x1234560000 is compact (0x05123456)
     * and  0xc0de000000 is compact (0x0600c0de)
     *
     * Bitcoin only uses this "compact" format for encoding difficulty targets,
     * which are unsigned 256bit quantities. Thus, all the complexities of the
     * sign bit and using base 256 are probably an implementation accident.
     */
    arith_uint256 &SetCompact(uint32_t nCompact, bool *pfNegative = nullptr,
                              bool *pfOverflow = nullptr);
    uint32_t GetCompact(bool fNegative = false) const;

    friend uint256 ArithToUint256(const arith_uint256 &);
    friend arith_uint256 UintToArith256(const uint256 &);
};


// ---------------------------------------------------------------------------------------------------
// Params
// ---------------------------------------------------------------------------------------------------


namespace Consensus {

/**
 * Parameters that influence chain consensus.
 */
struct Params {
    // BlockHash hashGenesisBlock;
    // int nSubsidyHalvingInterval;
    // /** Block height at which BIP16 becomes active */
    // int BIP16Height;
    // /** Block height and hash at which BIP34 becomes active */
    // int BIP34Height;
    // BlockHash BIP34Hash;
    // /** Block height at which BIP65 becomes active */
    // int BIP65Height;
    // /** Block height at which BIP66 becomes active */
    // int BIP66Height;
    // /** Block height at which CSV (BIP68, BIP112 and BIP113) becomes active */
    // int CSVHeight;
    // /** Block height at which UAHF kicks in */
    // int uahfHeight;
    // /** Block height at which the new DAA becomes active */
    // int daaHeight;
    // /** Block height at which the magnetic anomaly activation becomes active */
    // int magneticAnomalyHeight;
    // /** Block height at which the graviton activation becomes active */
    // int gravitonHeight;
    // /** Unix time used for MTP activation of 15 May 2020 12:00:00 UTC upgrade */
    // int phononActivationTime;
    // /** Unix time used for MTP activation of 15 Nov 2020 12:00:00 UTC upgrade */
    // int axionActivationTime;

    // /** Proof of work parameters */
    uint256 powLimit;
    bool fPowAllowMinDifficultyBlocks;
    // bool fPowNoRetargeting;
    int64_t nPowTargetSpacing;
    int64_t nPowTargetTimespan;
    int64_t DifficultyAdjustmentInterval() const {
        return nPowTargetTimespan / nPowTargetSpacing;
    }
    // uint256 nMinimumChainWork;
    // BlockHash defaultAssumeValid;
};
} // namespace Consensus

/**
 * CChainParams defines various tweakable parameters of a given instance of the
 * Bitcoin system. There are three: the main network on which people trade goods
 * and services, the public test network which gets reset from time to time and
 * a regression test mode which is intended for private networks only. It has
 * minimal difficulty to ensure that blocks can be found instantly.
 */
class CChainParams {
public:
    // enum Base58Type {
    //     PUBKEY_ADDRESS,
    //     SCRIPT_ADDRESS,
    //     SECRET_KEY,
    //     EXT_PUBLIC_KEY,
    //     EXT_SECRET_KEY,

    //     MAX_BASE58_TYPES
    // };

    const Consensus::Params &GetConsensus() const { return consensus; }
    // const CMessageHeader::MessageMagic &DiskMagic() const { return diskMagic; }
    // const CMessageHeader::MessageMagic &NetMagic() const { return netMagic; }
    // int GetDefaultPort() const { return nDefaultPort; }

    // const CBlock &GenesisBlock() const { return genesis; }
    // /** Default value for -checkmempool and -checkblockindex argument */
    // bool DefaultConsistencyChecks() const { return fDefaultConsistencyChecks; }
    // /** Policy: Filter transactions that do not match well-defined patterns */
    // bool RequireStandard() const { return fRequireStandard; }
    // /** If this is a test chain */
    // bool IsTestChain() const { return m_is_test_chain; }
    // uint64_t PruneAfterHeight() const { return nPruneAfterHeight; }
    // /** Minimum free space (in GB) needed for data directory */
    // uint64_t AssumedBlockchainSize() const { return m_assumed_blockchain_size; }
    // /**
    //  * Minimum free space (in GB) needed for data directory when pruned; Does
    //  * not include prune target
    //  */
    // uint64_t AssumedChainStateSize() const {
    //     return m_assumed_chain_state_size;
    // }
    // /** Whether it is possible to mine blocks on demand (no retargeting) */
    // bool MineBlocksOnDemand() const { return consensus.fPowNoRetargeting; }
    // /** Return the BIP70 network string (main, test or regtest) */
    // std::string NetworkIDString() const { return strNetworkID; }
    // /** Return the list of hostnames to look up for DNS seeds */
    // const std::vector<std::string> &DNSSeeds() const { return vSeeds; }
    // const std::vector<uint8_t> &Base58Prefix(Base58Type type) const {
    //     return base58Prefixes[type];
    // }
    // const std::string &CashAddrPrefix() const { return cashaddrPrefix; }
    // const std::vector<SeedSpec6> &FixedSeeds() const { return vFixedSeeds; }
    // const CCheckpointData &Checkpoints() const { return checkpointData; }
    // const ChainTxData &TxData() const { return chainTxData; }

protected:
    CChainParams() {}
    Consensus::Params consensus;
};


namespace ChainParamsConstants {
    const BlockHash MAINNET_DEFAULT_ASSUME_VALID = BlockHash::fromHex("000000000000000000c0fd281da21c51356771c89b34cb55ea1413d9c1ca4e66");
    const uint256 MAINNET_MINIMUM_CHAIN_WORK = uint256S("00000000000000000000000000000000000000000131410982cd0adc8ef33d88");

    // const BlockHash TESTNET_DEFAULT_ASSUME_VALID = BlockHash::fromHex("0000000000002b1dcad9b546e9de3eda59dea9547f48cd8803c75fb0cf3a18a0");
    // const uint256 TESTNET_MINIMUM_CHAIN_WORK = uint256S("0000000000000000000000000000000000000000000000594d3c47e367215741");
} // namespace ChainParamsConstants


/**
 * Main network
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        // consensus.nSubsidyHalvingInterval = 210000;
        // // 00000000000000ce80a7e057163a4db1d5ad7b20fb6f598c9597b9665c8fb0d4 -
        // // April 1, 2012
        // consensus.BIP16Height = 173805;
        // consensus.BIP34Height = 227931;
        // consensus.BIP34Hash = BlockHash::fromHex(
        //     "000000000000024b89b42a942fe0d9fea3bb44ab7bd1b19115dd6a759c0808b8");
        // // 000000000000000004c2b624ed5d7756c508d90fd0da2c7c679febfa6c4735f0
        // consensus.BIP65Height = 388381;
        // // 00000000000000000379eaa19dce8c9b722d46ae6a57c2f1a988119488b50931
        // consensus.BIP66Height = 363725;
        // // 000000000000000004a1b34462cb8aeebd5799177f7a29cf28f2d1961716b5b5
        // consensus.CSVHeight = 419328;
        consensus.powLimit = uint256S(
            "00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        // two weeks
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60;
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        // consensus.fPowNoRetargeting = false;

        // // The best chain should have at least this much work.
        // consensus.nMinimumChainWork =
        //     ChainParamsConstants::MAINNET_MINIMUM_CHAIN_WORK;

        // // By default assume that the signatures in ancestors of this block are
        // // valid.
        // consensus.defaultAssumeValid =
        //     ChainParamsConstants::MAINNET_DEFAULT_ASSUME_VALID;

        // // August 1, 2017 hard fork
        // consensus.uahfHeight = 478558;

        // // November 13, 2017 hard fork
        // consensus.daaHeight = 504031;

        // // November 15, 2018 hard fork
        // consensus.magneticAnomalyHeight = 556766;

        // // November 15, 2019 protocol upgrade
        // consensus.gravitonHeight = 609135;

        // // May 15, 2020 12:00:00 UTC protocol upgrade
        // consensus.phononActivationTime = 1589544000;

        // // Nov 15, 2020 12:00:00 UTC protocol upgrade
        // consensus.axionActivationTime = 1605441600;


        // genesis = CreateGenesisBlock(1231006505, 2083236893, 0x1d00ffff, 1,
        //                              50 * COIN);
        // consensus.hashGenesisBlock = genesis.GetHash();
        // assert(consensus.hashGenesisBlock ==
        //        uint256S("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1"
        //                 "b60a8ce26f"));
        // assert(genesis.hashMerkleRoot ==
        //        uint256S("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b"
        //                 "7afdeda33b"));
    }
};

// inline
// Consensus::Params GetDefaultMainnetConsensusParams() {
//     Consensus::Params consensus;
//     consensus.powLimit = uint256S(
//         "00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
//     // two weeks
//     consensus.nPowTargetTimespan = 14 * 24 * 60 * 60;
//     consensus.nPowTargetSpacing = 10 * 60;
//     consensus.fPowAllowMinDifficultyBlocks = false;
//     return consensus; 
// }

// ---------------------------------------------------------------------------------------------------



class CBlockHeader {
public:
    // // header
    int32_t nVersion;
    BlockHash hashPrevBlock;
    uint256 hashMerkleRoot;
    uint32_t nTime;
    uint32_t nBits;
    uint32_t nNonce;

    // CBlockHeader() { SetNull(); }

    // ADD_SERIALIZE_METHODS;

    // template <typename Stream, typename Operation>
    // inline void SerializationOp(Stream &s, Operation ser_action) {
    //     READWRITE(this->nVersion);
    //     READWRITE(hashPrevBlock);
    //     READWRITE(hashMerkleRoot);
    //     READWRITE(nTime);
    //     READWRITE(nBits);
    //     READWRITE(nNonce);
    // }

    // void SetNull() {
    //     nVersion = 0;
    //     hashPrevBlock = BlockHash();
    //     hashMerkleRoot.SetNull();
    //     nTime = 0;
    //     nBits = 0;
    //     nNonce = 0;
    // }

    // bool IsNull() const { return (nBits == 0); }

    // BlockHash GetHash() const;

    int64_t GetBlockTime() const { return (int64_t)nTime; }
};

class CBlockIndex {
public:
    //! pointer to the hash of the block, if any. Memory is owned by this
    //! CBlockIndex
    const BlockHash *phashBlock = nullptr;

    //! pointer to the index of the predecessor of this block
    CBlockIndex *pprev = nullptr;

    //! pointer to the index of some further predecessor of this block
    CBlockIndex *pskip = nullptr;

    //! height of the entry in the chain. The genesis block has height 0
    int nHeight = 0;

    // //! Which # file this block is stored in (blk?????.dat)
    // int nFile = 0;

    // //! Byte offset within blk?????.dat where this block's data is stored
    // unsigned int nDataPos = 0;

    // //! Byte offset within rev?????.dat where this block's undo data is stored
    // unsigned int nUndoPos = 0;

    //! (memory only) Total amount of work (expected number of hashes) in the
    //! chain up to and including this block
    arith_uint256 nChainWork = arith_uint256();

    // //! Number of transactions in this block.
    // //! Note: in a potential headers-first mode, this number cannot be relied
    // //! upon
    // unsigned int nTx = 0;

    // //! (memory only) Number of transactions in the chain up to and including
    // //! this block.
    // //! This value will be non-zero only if and only if transactions for this
    // //! block and all its parents are available. Change to 64-bit type when
    // //! necessary; won't happen before 2030
    // unsigned int nChainTx = 0;

    // //! Verification status of this block. See enum BlockStatus
    // BlockStatus nStatus = BlockStatus();

    //! block header
    int32_t nVersion = 0;
    uint256 hashMerkleRoot = uint256();

    uint32_t nTime = 0;
    uint32_t nBits = 0;
    uint32_t nNonce = 0;

    // //! (memory only) Sequential id assigned to distinguish order in which
    // //! blocks are received.
    // int32_t nSequenceId = 0;

    // //! (memory only) block header metadata
    // uint64_t nTimeReceived = 0;

    // //! (memory only) Maximum nTime in the chain up to and including this block.
    // unsigned int nTimeMax = 0;

    // explicit CBlockIndex() = default;

    // explicit CBlockIndex(const CBlockHeader &block) : CBlockIndex() {
    //     nVersion = block.nVersion;
    //     hashMerkleRoot = block.hashMerkleRoot;
    //     nTime = block.nTime;
    //     nTimeReceived = 0;
    //     nBits = block.nBits;
    //     nNonce = block.nNonce;
    // }

    // FlatFilePos GetBlockPos() const {
    //     FlatFilePos ret;
    //     if (nStatus.hasData()) {
    //         ret.nFile = nFile;
    //         ret.nPos = nDataPos;
    //     }
    //     return ret;
    // }

    // FlatFilePos GetUndoPos() const {
    //     FlatFilePos ret;
    //     if (nStatus.hasUndo()) {
    //         ret.nFile = nFile;
    //         ret.nPos = nUndoPos;
    //     }
    //     return ret;
    // }

    CBlockHeader GetBlockHeader() const {
        CBlockHeader block;
        block.nVersion = nVersion;
        if (pprev) {
            block.hashPrevBlock = pprev->GetBlockHash();
        }
        block.hashMerkleRoot = hashMerkleRoot;
        block.nTime = nTime;
        block.nBits = nBits;
        block.nNonce = nNonce;
        return block;
    }

    BlockHash GetBlockHash() const { return *phashBlock; }

    // /**
    //  * Get the number of transaction in the chain so far.
    //  */
    // int64_t GetChainTxCount() const { return nChainTx; }

    // /**
    //  * Check whether this block's and all previous blocks' transactions have
    //  * been downloaded (and stored to disk) at some point.
    //  *
    //  * Does not imply the transactions are consensus-valid (ConnectTip might
    //  * fail) Does not imply the transactions are still stored on disk.
    //  * (IsBlockPruned might return true)
    //  */
    // bool HaveTxsDownloaded() const { return GetChainTxCount() != 0; }

    int64_t GetBlockTime() const { return int64_t(nTime); }

    // int64_t GetBlockTimeMax() const { return int64_t(nTimeMax); }

    // int64_t GetHeaderReceivedTime() const { return nTimeReceived; }

    // int64_t GetReceivedTimeDiff() const {
    //     return GetHeaderReceivedTime() - GetBlockTime();
    // }

    // static constexpr int nMedianTimeSpan = 11;

    // int64_t GetMedianTimePast() const {
    //     int64_t pmedian[nMedianTimeSpan];
    //     int64_t *pbegin = &pmedian[nMedianTimeSpan];
    //     int64_t *pend = &pmedian[nMedianTimeSpan];

    //     const CBlockIndex *pindex = this;
    //     for (int i = 0; i < nMedianTimeSpan && pindex;
    //          i++, pindex = pindex->pprev) {
    //         *(--pbegin) = pindex->GetBlockTime();
    //     }

    //     std::sort(pbegin, pend);
    //     return pbegin[(pend - pbegin) / 2];
    // }

    // std::string ToString() const {
    //     return strprintf(
    //         "CBlockIndex(pprev=%p, nHeight=%d, merkle=%s, hashBlock=%s)", pprev,
    //         nHeight, hashMerkleRoot.ToString(), GetBlockHash().ToString());
    // }

    // //! Check whether this block index entry is valid up to the passed validity
    // //! level.
    // bool IsValid(enum BlockValidity nUpTo = BlockValidity::TRANSACTIONS) const {
    //     return nStatus.isValid(nUpTo);
    // }

    // //! Raise the validity level of this block index entry.
    // //! Returns true if the validity was changed.
    // bool RaiseValidity(enum BlockValidity nUpTo) {
    //     // Only validity flags allowed.
    //     if (nStatus.isInvalid()) {
    //         return false;
    //     }

    //     if (nStatus.getValidity() >= nUpTo) {
    //         return false;
    //     }

    //     nStatus = nStatus.withValidity(nUpTo);
    //     return true;
    // }

    // //! Build the skiplist pointer for this entry.
    // void BuildSkip();

    // //! Efficiently find an ancestor of this block.
    // CBlockIndex *GetAncestor(int height);
    const CBlockIndex *GetAncestor(int height) const;
};

/** Turn the lowest '1' bit in the binary representation of a number into a '0'.
 */
static inline int InvertLowestOne(int n) {
    return n & (n - 1);
}

/** Compute what height to jump back to with the CBlockIndex::pskip pointer. */
static inline int GetSkipHeight(int height) {
    if (height < 2) {
        return 0;
    }

    // Determine which height to jump back to. Any number strictly lower than
    // height is acceptable, but the following expression seems to perform well
    // in simulations (max 110 steps to go back up to 2**18 blocks).
    return (height & 1) ? InvertLowestOne(InvertLowestOne(height - 1)) + 1
                        : InvertLowestOne(height);
}

inline
const CBlockIndex *CBlockIndex::GetAncestor(int height) const {
    if (height > nHeight || height < 0) {
        return nullptr;
    }

    const CBlockIndex *pindexWalk = this;
    int heightWalk = nHeight;
    while (heightWalk > height) {
        int heightSkip = GetSkipHeight(heightWalk);
        int heightSkipPrev = GetSkipHeight(heightWalk - 1);
        if (pindexWalk->pskip != nullptr &&
            (heightSkip == height ||
             (heightSkip > height && !(heightSkipPrev < heightSkip - 2 &&
                                       heightSkipPrev >= height)))) {
            // Only follow pskip if pprev->pskip isn't better than pskip->pprev.
            pindexWalk = pindexWalk->pskip;
            heightWalk = heightSkip;
        } else {
            assert(pindexWalk->pprev);
            pindexWalk = pindexWalk->pprev;
            heightWalk--;
        }
    }
    return pindexWalk;
}


// ---------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------


// https://gitlab.com/jtoomim/bitcoin-cash-node/-/blob/fd92035c2e8d16360fb3e314b626bf52f2a2be67/src/pow.cpp#L299

/**
 * Compute the next required proof of work using an absolutely scheduled 
 * exponentially weighted target (ASERT).
 *
 * With ASERT, we define an ideal schedule for block issuance (e.g. 1 block every 600 seconds), and we calculate the
 * difficulty based on how far the most recent block's timestamp is ahead of or behind that schedule.
 * We set our targets (difficulty) exponentially. For every [tau] seconds ahead of or behind schedule we get, we
 * double or halve the difficulty. 
 */
uint32_t GetNextASERTWorkRequired(const CBlockIndex *pindexPrev,
                                  const CBlockHeader *pblock,
                                  const Consensus::Params &params,
                                  const int32_t nforkHeight) noexcept;

#endif // ASERTI3_416_HPP_