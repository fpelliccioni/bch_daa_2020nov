#ifndef ASERTI3_416_HPP_
#define ASERTI3_416_HPP_

#include <cassert>
#include <cstdint>
#include <cstring>

#include <string>

/** Template base class for fixed-sized opaque blobs. */
template <unsigned int BITS> class base_blob {
protected:
    static constexpr int WIDTH = BITS / 8;
    uint8_t data[WIDTH];

public:
//     constexpr base_blob() noexcept : data{0} {}

//     /// type tag + convenience member for uninitialized c'tor
//     static constexpr struct Uninitialized_t {} Uninitialized{};

//     /// Uninitialized data constructor -- to be used when we want to avoid a
//     /// redundant zero-initialization in cases where we know we will fill-in
//     /// the data immediately anyway (e.g. for random generators, etc).
//     /// Select this c'tor with e.g.: uint256 foo{uint256::Uninitialized}
//     explicit constexpr base_blob(Uninitialized_t /* type tag to select this c'tor */) noexcept {}

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
//     void SetHex(const char *psz);
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

//     const base_uint operator~() const {
//         base_uint ret;
//         for (int i = 0; i < WIDTH; i++) {
//             ret.pn[i] = ~pn[i];
//         }
//         return ret;
//     }

//     const base_uint operator-() const {
//         base_uint ret;
//         for (int i = 0; i < WIDTH; i++) {
//             ret.pn[i] = ~pn[i];
//         }
//         ++ret;
//         return ret;
//     }

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

//     base_uint &operator-=(const base_uint &b) {
//         *this += -b;
//         return *this;
//     }

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
//     base_uint &operator/=(const base_uint &b);

//     base_uint &operator++() {
//         // prefix operator
//         int i = 0;
//         while (i < WIDTH && ++pn[i] == 0) {
//             i++;
//         }
//         return *this;
//     }

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

//     friend inline const base_uint operator+(const base_uint &a,
//                                             const base_uint &b) {
//         return base_uint(a) += b;
//     }
//     friend inline const base_uint operator-(const base_uint &a,
//                                             const base_uint &b) {
//         return base_uint(a) -= b;
//     }
//     friend inline const base_uint operator*(const base_uint &a,
//                                             const base_uint &b) {
//         return base_uint(a) *= b;
//     }
//     friend inline const base_uint operator/(const base_uint &a,
//                                             const base_uint &b) {
//         return base_uint(a) /= b;
//     }
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
//     friend inline bool operator==(const base_uint &a, const base_uint &b) {
//         return memcmp(a.pn, b.pn, sizeof(a.pn)) == 0;
//     }
//     friend inline bool operator!=(const base_uint &a, const base_uint &b) {
//         return memcmp(a.pn, b.pn, sizeof(a.pn)) != 0;
//     }
    friend inline bool operator>(const base_uint &a, const base_uint &b) {
        return a.CompareTo(b) > 0;
    }
//     friend inline bool operator<(const base_uint &a, const base_uint &b) {
//         return a.CompareTo(b) < 0;
//     }
//     friend inline bool operator>=(const base_uint &a, const base_uint &b) {
//         return a.CompareTo(b) >= 0;
//     }
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
//     void SetHex(const char *psz);
//     void SetHex(const std::string &str);
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

    // //! pointer to the index of some further predecessor of this block
    // CBlockIndex *pskip = nullptr;

    //! height of the entry in the chain. The genesis block has height 0
    int nHeight = 0;

    // //! Which # file this block is stored in (blk?????.dat)
    // int nFile = 0;

    // //! Byte offset within blk?????.dat where this block's data is stored
    // unsigned int nDataPos = 0;

    // //! Byte offset within rev?????.dat where this block's undo data is stored
    // unsigned int nUndoPos = 0;

    // //! (memory only) Total amount of work (expected number of hashes) in the
    // //! chain up to and including this block
    // arith_uint256 nChainWork = arith_uint256();

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
    // const CBlockIndex *GetAncestor(int height) const;
};


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
                                  const int32_t nforkHeight);



#endif // ASERTI3_416_HPP_