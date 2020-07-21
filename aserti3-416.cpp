// g++ -std=c++11 aserti3-416.cpp
#include <cassert>
#include <cstdint>
#include <cstring>

#include <iostream>
#include <string>

#include "aserti3-416.hpp"

uint32_t arith_uint256::GetCompact(bool fNegative) const {
    int nSize = (bits() + 7) / 8;
    uint32_t nCompact = 0;
    if (nSize <= 3) {
        nCompact = GetLow64() << 8 * (3 - nSize);
    } else {
        arith_uint256 bn = *this >> 8 * (nSize - 3);
        nCompact = bn.GetLow64();
    }
    // The 0x00800000 bit denotes the sign.
    // Thus, if it is already set, divide the mantissa by 256 and increase the
    // exponent.
    if (nCompact & 0x00800000) {
        nCompact >>= 8;
        nSize++;
    }
    assert((nCompact & ~0x007fffff) == 0);
    assert(nSize < 256);
    nCompact |= nSize << 24;
    nCompact |= (fNegative && (nCompact & 0x007fffff) ? 0x00800000 : 0);
    return nCompact;
}

// This implementation directly uses shifts instead of going through an
// intermediate MPI representation.
arith_uint256 &arith_uint256::SetCompact(uint32_t nCompact, bool *pfNegative,
                                         bool *pfOverflow) {
    int nSize = nCompact >> 24;
    uint32_t nWord = nCompact & 0x007fffff;
    if (nSize <= 3) {
        nWord >>= 8 * (3 - nSize);
        *this = nWord;
    } else {
        *this = nWord;
        *this <<= 8 * (nSize - 3);
    }
    if (pfNegative) {
        *pfNegative = nWord != 0 && (nCompact & 0x00800000) != 0;
    }
    if (pfOverflow) {
        *pfOverflow =
            nWord != 0 && ((nSize > 34) || (nWord > 0xff && nSize > 33) ||
                           (nWord > 0xffff && nSize > 32));
    }
    return *this;
}


// //Little
// inline uint32_t le32toh(uint32_t little_endian_32bits) {
//     return little_endian_32bits;
// }

static inline uint32_t ReadLE32(const uint8_t *ptr) {
    uint32_t x;
    memcpy((char *)&x, ptr, 4);
    return le32toh(x);
}

arith_uint256 UintToArith256(const uint256 &a) {
    arith_uint256 b;
    for (int x = 0; x < b.WIDTH; ++x) {
        b.pn[x] = ReadLE32(a.begin() + x * 4);
    }
    return b;
}



// https://gitlab.com/freetrader/bitcoin-cash-node/-/blob/affe4657dc85f25b6782648960579bb2a8fedd6a/src/pow.cpp#L106
// ASERT calculation function.
// Clamps to powLimit.
arith_uint256 CalculateASERT(const arith_uint256 refTarget,
                             const int64_t nPowTargetSpacing,
                             const int64_t nTimeDiff,
                             const int64_t nHeightDiff,
                             const arith_uint256 powLimit,
                             const int64_t nHalfLife,
                             bool debugASERT) noexcept {

    // std::cout << "CalculateASERT - 1\n";


    // Input target must never be zero nor exceed powLimit.
    assert(refTarget > 0 && refTarget <= powLimit);

    // std::cout << "CalculateASERT - 2\n";

    // Height diff should NOT be negative.
    assert(nHeightDiff >= 0);
    // std::cout << "CalculateASERT - 3\n";


    // This algorithm uses fixed-point math. The lowest rbits bits are after
    // the radix, and represent the "decimal" (or binary) portion of the value
    constexpr uint8_t rbits = 16;
    static_assert(rbits > 0);

    arith_uint256 nextTarget = refTarget;

    // std::cout << "refTarget.GetCompact():         " << refTarget.GetCompact() << "\n";
    // std::cout << "nextTarget.GetCompact():        " << nextTarget.GetCompact() << "\n";

    // It will be helpful when reading what follows, to remember that
    // nextTarget is adapted from reference block target value.

    // Ultimately, we want to approximate the following ASERT formula, using only integer (fixed-point) math:
    //     new_target = old_target * 2^((blocks_time - IDEAL_BLOCK_TIME*(height_diff+1)) / nHalfLife)

    // First, we'll calculate the exponent:
    assert( abs(nTimeDiff - nPowTargetSpacing * nHeightDiff) < (1ull<<(63-rbits)) );

    // std::cout << "nTimeDiff:         " << nTimeDiff << "\n";
    // std::cout << "nPowTargetSpacing: " << nPowTargetSpacing << "\n";
    // std::cout << "nHeightDiff:       " << nHeightDiff << "\n";
    // std::cout << "rbits:             " << (int)rbits << "\n";
    // std::cout << "nHalfLife:         " << nHalfLife << "\n";

    int64_t exponent = ((nTimeDiff - nPowTargetSpacing * nHeightDiff) << rbits) / nHalfLife;
    // std::cout << "exponent:         " << exponent << "\n";

    // Next, we use the 2^x = 2 * 2^(x-1) identity to shift our exponent into the [0, 1) interval.
    // The truncated exponent tells us how many shifts we need to do
    // Note1: This needs to be a right shift. Right shift rounds downward (floored division),
    //        whereas integer division in C++ rounds towards zero (truncated division).
    // Note2: This algorithm uses arithmetic shifts of negative numbers. This
    //        is unpecified but very common behavior for C++ compilers before
    //        C++20, and standard with C++20. We must check this behavior e.g.
    //        using static_assert.
    static_assert(int64_t(-1) >> 1 == int64_t(-1),
                  "ASERT algorithm needs arithmetic shift support");

    const int64_t shifts = exponent >> rbits;
    // std::cout << "shifts:         " << shifts << "\n";

    if (shifts < 0) {
        // std::cout << "CalculateASERT - 4\n";
        nextTarget = nextTarget >> -shifts;
    } else {
        // std::cout << "CalculateASERT - 5\n";
        nextTarget = nextTarget << shifts;
    }
    // std::cout << "CalculateASERT - 6\n";
    // std::cout << "nextTarget.GetCompact():        " << nextTarget.GetCompact() << "\n";

    // Remove everything but the decimal part from the exponent since we've
    // accounted for that through shifting.
    exponent -= (shifts << rbits);
    // What is left then should now be in the fixed point range [0, 1).
    assert(exponent >= 0 && exponent < 65536);
    // std::cout << "exponent:         " << exponent << "\n";

    // Check for overflow and underflow from shifting nextTarget. Since it's a uint, both could result in a
    // value of 0, so we'll need to clamp it if so. We can figure out which happened by looking at shifts's sign.
    if (nextTarget == 0 || nextTarget > powLimit) {
        // std::cout << "CalculateASERT - 7\n";
        if (shifts < 0) {
            // std::cout << "CalculateASERT - 8\n";
            // std::cout << "returns:  1\n";
            return arith_uint256(1);
        } else {
            // std::cout << "CalculateASERT - 9\n";
            // std::cout << "returns powLimit.GetCompact():        " << powLimit.GetCompact() << "\n";
            return powLimit;
        }
    }
    // std::cout << "CalculateASERT - 10\n";

    // Now we compute an approximated target * 2^(exponent)

    // 2^x ~= (1 + 0.695502049*x + 0.2262698*x**2 + 0.0782318*x**3) for 0 <= x < 1
    // Error versus actual 2^x is less than 0.013%.
    uint64_t factor = (195766423245049*exponent +
                       971821376*exponent*exponent +
                       5127*exponent*exponent*exponent + (1ull<<47))>>(rbits*3);

    // std::cout << "factor:         " << factor << "\n";

    nextTarget += (nextTarget * factor) >> rbits;
    // std::cout << "nextTarget.GetCompact():        " << nextTarget.GetCompact() << "\n";

    // The last operation was strictly increasing, so it could have exceeded powLimit. Check and clamp again.
    if (nextTarget > powLimit) {
        // std::cout << "CalculateASERT - 11\n";
        // std::cout << "returns powLimit.GetCompact():        " << powLimit.GetCompact() << "\n";
        return powLimit;
    }
    // std::cout << "CalculateASERT - 12\n";

    // std::cout << "returns nextTarget.GetCompact():        " << nextTarget.GetCompact() << "\n";
    return nextTarget;
}


// https://gitlab.com/freetrader/bitcoin-cash-node/-/blob/affe4657dc85f25b6782648960579bb2a8fedd6a/src/pow.cpp#L52
/**
 * Compute the next required proof of work using an absolutely scheduled
 * exponentially weighted target (ASERT).
 *
 * With ASERT, we define an ideal schedule for block issuance (e.g. 1 block every 600 seconds), and we calculate the
 * difficulty based on how far the most recent block's timestamp is ahead of or behind that schedule.
 * We set our targets (difficulty) exponentially. For every [nHalfLife] seconds ahead of or behind schedule we get, we
 * double or halve the difficulty.
 */
uint32_t GetNextASERTWorkRequired(const CBlockIndex *pindexPrev,
                                  const CBlockHeader *pblock,
                                  const Consensus::Params &params,
                                  const CBlockIndex *pindexReferenceBlock,
                                  bool debugASERT) noexcept {

    // std::cout << "GetNextASERTWorkRequired - 1\n";

    // This cannot handle the genesis block and early blocks in general.
    assert(pindexPrev != nullptr);

    // std::cout << "GetNextASERTWorkRequired - 2\n";

    // Reference block is the block on which all ASERT scheduling calculations are based.
    // It too must exist.
    assert(pindexReferenceBlock != nullptr);

    // std::cout << "GetNextASERTWorkRequired - 3\n";

    // We make no further assumptions other than the height of the prev block must be >= that of the activation block.
    assert(pindexPrev->nHeight >= pindexReferenceBlock->nHeight);

    // std::cout << "GetNextASERTWorkRequired - 4\n";

    // Special difficulty rule for testnet
    // If the new block's timestamp is more than 2* 10 minutes then allow
    // mining of a min-difficulty block.
    if (params.fPowAllowMinDifficultyBlocks &&
        (pblock->GetBlockTime() >
         pindexPrev->GetBlockTime() + 2 * params.nPowTargetSpacing)) {
        // std::cout << "GetNextASERTWorkRequired - 5\n";
        return UintToArith256(params.powLimit).GetCompact();
    }
    // std::cout << "GetNextASERTWorkRequired - 6\n";

    // Shortcut calculation in case the comparison block is the reference
    // Just return the original target in that case.
    // TODO: think about it - this code is executed only exactly once.
    if (pindexPrev->nHeight == pindexReferenceBlock->nHeight) {
        // std::cout << "GetNextASERTWorkRequired - 7\n";
       return pindexReferenceBlock->nBits;
    }

    // std::cout << "GetNextASERTWorkRequired - 8\n";

    const int64_t nTimeDiff = int64_t(pindexPrev->nTime) - int64_t(pindexReferenceBlock->GetBlockHeader().nTime);
    const int64_t nHeightDiff = pindexPrev->nHeight - pindexReferenceBlock->nHeight;


    // std::cout << "params.nDAAHalfLife: " << params.nDAAHalfLife << "\n";
    // std::cout << "pindexPrev->nTime:   " << int64_t(pindexPrev->nTime) << "\n";
    // std::cout << "pindexReferenceBlock->nTime:   " << int64_t(pindexReferenceBlock->nTime) << "\n";
    // std::cout << "pindexReferenceBlock->GetBlockHeader().nTime:   " << int64_t(pindexReferenceBlock->GetBlockHeader().nTime) << "\n";
    // std::cout << "pindexPrev->nHeight:   " << pindexPrev->nHeight << "\n";
    // std::cout << "pindexReferenceBlock->nHeight:   " << pindexReferenceBlock->nHeight << "\n";

    // std::cout << "nTimeDiff:   " << nTimeDiff << "\n";
    // std::cout << "nHeightDiff: " << nHeightDiff << "\n";

    const arith_uint256 refBlockTarget = arith_uint256().SetCompact(pindexReferenceBlock->nBits);
    // std::cout << "refBlockTarget.GetCompact(): " << refBlockTarget.GetCompact() << "\n";

    static const arith_uint256 powLimit = UintToArith256(params.powLimit);
    // std::cout << "powLimit.GetCompact(): " << powLimit.GetCompact() << "\n";

    // Refactored: do the actual target adaptation calculation in separate
    // CalculateCASERT() function
    arith_uint256 nextTarget = CalculateASERT(refBlockTarget,
                                              params.nPowTargetSpacing,
                                              nTimeDiff,
                                              nHeightDiff,
                                              powLimit,
                                              params.nDAAHalfLife,
                                              debugASERT);

    // std::cout << "nextTarget.GetCompact(): " << nextTarget.GetCompact() << "\n";

    // CalculateASERT() already clamps to powLimit.
    return nextTarget.GetCompact();
}



// // https://gitlab.com/jtoomim/bitcoin-cash-node/-/blob/wip-asert/src/pow.cpp#L299
// /**
//  * Compute the next required proof of work using an absolutely scheduled 
//  * exponentially weighted target (ASERT).
//  *
//  * With ASERT, we define an ideal schedule for block issuance (e.g. 1 block every 600 seconds), and we calculate the
//  * difficulty based on how far the most recent block's timestamp is ahead of or behind that schedule.
//  * We set our targets (difficulty) exponentially. For every [tau] seconds ahead of or behind schedule we get, we
//  * double or halve the difficulty. 
//  */
// uint32_t GetNextASERTWorkRequired(const CBlockIndex *pindexPrev,
//                                   const CBlockHeader *pblock,
//                                   const Consensus::Params &params,
//                                   const int32_t nRefHeight) noexcept {
//     // This cannot handle the genesis block and early blocks in general.
//     assert(pindexPrev);

//     std::cout << "GetNextASERTWorkRequired - 1\n";

//     // Special difficulty rule for testnet:
//     // If the new block's timestamp is more than 2* 10 minutes then allow
//     // mining of a min-difficulty block.
//     if (params.fPowAllowMinDifficultyBlocks &&
//         (pblock->GetBlockTime() >
//          pindexPrev->GetBlockTime() + 2 * params.nPowTargetSpacing)) {
//         std::cout << "GetNextASERTWorkRequired - 2\n";
//         return UintToArith256(params.powLimit).GetCompact();
//     }

//     std::cout << "GetNextASERTWorkRequired - 3\n";

//     // Diff halves/doubles for every 2 days behind/ahead of schedule we get
//     constexpr uint64_t tau = 2*24*60*60;

//     // This algorithm uses fixed-point math. The lowest rbits bits are after
//     // the radix, and represent the "decimal" (or binary) portion of the value
//     constexpr uint8_t rbits = 16;

//     std::cout << "nRefHeight: " << nRefHeight << "\n";

//     const CBlockIndex *prefBlock = pindexPrev->GetAncestor(nRefHeight);
//     assert(prefBlock != nullptr);

//     std::cout << "prefBlock->nHeight: " << prefBlock->nHeight << "\n";

//     const int64_t nTimeDiff = pindexPrev->nTime - prefBlock->GetBlockHeader().nTime;
//     const int32_t nHeightDiff = pindexPrev->nHeight - prefBlock->nHeight;
//     assert(nHeightDiff > 0);

//     std::cout << "nTimeDiff: " << nTimeDiff << "\n";
//     std::cout << "nHeightDiff: " << nHeightDiff << "\n";

//     const arith_uint256 origTarget = arith_uint256().SetCompact(prefBlock->nBits);
//     arith_uint256 nextTarget = origTarget;
//     std::cout << "prefBlock->nBits: " << prefBlock->nBits << "\n";
//     // std::cout << "nextTarget: " << nextTarget << "\n";
//     std::cout << "nextTarget.GetCompact(): " << nextTarget.GetCompact() << "\n";

//     // Ultimately, we want to approximate the following ASERT formula, using only integer (fixed-point) math:
//     //     new_target = old_target * 2^((blocks_time - IDEAL_BLOCK_TIME*(height_diff+1)) / tau)

//     std::cout << "nTimeDiff: " << nTimeDiff << "\n";
//     std::cout << "params.nPowTargetSpacing: " << params.nPowTargetSpacing << "\n";
//     std::cout << "nHeightDiff: " << nHeightDiff << "\n";
//     std::cout << "rbits: " << (int)rbits << "\n";
//     std::cout << "tau: " << tau << "\n";

//     // First, we'll calculate the exponent:
//     int64_t exponent = ((nTimeDiff - params.nPowTargetSpacing * (nHeightDiff+1)) << rbits) / tau;

//     std::cout << "exponent: " << exponent << "\n";

//     // Next, we use the 2^x = 2 * 2^(x-1) identity to shift our exponent into the [0, 1) interval.
//     // The truncated exponent tells us how many shifts we need to do
//     // Note1: This needs to be a right shift. Right shift rounds downward, whereas division rounds towards zero.
//     // Note2: This algorithm uses arithmetic shifts of negative numbers. This is unspecified but very common
//     // behavior for C++ compilers before C++20, and standard with C++20. We use unit tests to ensure our compiler
//     // behaves as expected.
//     int16_t shifts = exponent >> rbits;

//     std::cout << "shifts: " << shifts << "\n";

//     if (shifts < 0) {
//         std::cout << "GetNextASERTWorkRequired - 4\n";
//         nextTarget = nextTarget >> -shifts;
//     } else {
//         std::cout << "GetNextASERTWorkRequired - 5\n";
//         nextTarget = nextTarget << shifts;
//     }
//     exponent -= (shifts << rbits);

//     // std::cout << "nextTarget: " << nextTarget << "\n";
//     std::cout << "nextTarget.GetCompact(): " << nextTarget.GetCompact() << "\n";
//     std::cout << "exponent: " << exponent << "\n";


//     // Now we compute an approximated target * 2^(exponent)
//     // 2^x ~= (1 + 0.695502049*x + 0.2262698*x**2 + 0.0782318*x**3) for 0 <= x < 1
//     // Error versus actual 2^x is less than 0.013%.
//     uint64_t factor = (195766423245049*exponent + 
//                        971821376*exponent*exponent + 
//                        5127*exponent*exponent*exponent + (1ll<<47))>>(rbits*3);

//     std::cout << "factor: " << factor << "\n";

//     nextTarget += (nextTarget * factor) >> rbits;

//     // std::cout << "nextTarget: " << nextTarget << "\n";
//     std::cout << "nextTarget.GetCompact(): " << nextTarget.GetCompact() << "\n";

//     static const arith_uint256 powLimit = UintToArith256(params.powLimit);

//     // std::cout << "params.powLimit: " << params.powLimit << "\n";
//     // std::cout << "powLimit: " << powLimit << "\n";

//     if (nextTarget > powLimit) {
//         std::cout << "GetNextASERTWorkRequired - 6\n";
//         std::cout << "return powLimit.GetCompact(): " << powLimit.GetCompact() << "\n";
//         return powLimit.GetCompact();
//     }

//     std::cout << "GetNextASERTWorkRequired - 7\n";
//     std::cout << "return nextTarget.GetCompact(): " << nextTarget.GetCompact() << "\n";

//     return nextTarget.GetCompact();
// }


// // https://gitlab.com/jtoomim/bitcoin-cash-node/-/blob/fd92035c2e8d16360fb3e314b626bf52f2a2be67/src/pow.cpp#L299
// /**
//  * Compute the next required proof of work using an absolutely scheduled 
//  * exponentially weighted target (ASERT).
//  *
//  * With ASERT, we define an ideal schedule for block issuance (e.g. 1 block every 600 seconds), and we calculate the
//  * difficulty based on how far the most recent block's timestamp is ahead of or behind that schedule.
//  * We set our targets (difficulty) exponentially. For every [tau] seconds ahead of or behind schedule we get, we
//  * double or halve the difficulty. 
//  */
// uint32_t GetNextASERTWorkRequired(const CBlockIndex *pindexPrev,
//                                   const CBlockHeader *pblock,
//                                   const Consensus::Params &params,
//                                   const int32_t nforkHeight) {
//     // This cannot handle the genesis block and early blocks in general.
//     assert(pindexPrev);

//     // Special difficulty rule for testnet:
//     // If the new block's timestamp is more than 2* 10 minutes then allow
//     // mining of a min-difficulty block.
//     if (params.fPowAllowMinDifficultyBlocks &&
//         (pblock->GetBlockTime() >
//          pindexPrev->GetBlockTime() + 2 * params.nPowTargetSpacing)) {
//         return UintToArith256(params.powLimit).GetCompact();
//     }

//     // Diff halves/doubles for every 2 days behind/ahead of schedule we get
//     const uint64_t tau = 2*24*60*60; 

//     // This algorithm uses fixed-point math. The lowest rbits bits are after
//     // the radix, and represent the "decimal" (or binary) portion of the value
//     const uint8_t rbits = 16; 

//     const CBlockIndex *pforkBlock = &pindexPrev[nforkHeight];

//     assert(pforkBlock != nullptr);
//     assert(pindexPrev->nHeight >= params.DifficultyAdjustmentInterval());

//     int32_t nTimeDiff = pindexPrev->nTime - pforkBlock->GetBlockHeader().nTime;
//     int32_t nHeightDiff = pindexPrev->nHeight - pforkBlock->nHeight;

//     const arith_uint256 origTarget = arith_uint256().SetCompact(pforkBlock->nBits);
//     arith_uint256 nextTarget = origTarget;

//     // Ultimately, we want to approximate the following ASERT formula, using only integer (fixed-point) math:
//     //     new_target = old_target * 2^((blocks_time - IDEAL_BLOCK_TIME*(height_diff+1)) / tau)

//     // First, we'll calculate the exponent:
//     int64_t exponent = ((nTimeDiff - params.nPowTargetSpacing * (nHeightDiff+1)) << rbits) / tau;

//     // Next, we use the 2^x = 2 * 2(x-1) identity to shift our exponent into the [0, 1) interval.
//     // The truncated exponent tells us how many shifts we need to do
//     // Note: This needs to be a right shift. Right shift rounds downward, whereas division rounds towards zero.
//     int8_t shifts = exponent >> rbits;
//     if (shifts < 0) {
//         nextTarget = nextTarget >> -shifts;
//     } else {
//         nextTarget = nextTarget << shifts;
//     }
//     exponent -= (shifts << rbits);

//     // Now we compute an approximated target * 2^(exponent)
//     // 2^x ~= (1 + 0.695502049*x + 0.2262698*x**2 + 0.0782318*x**3) for 0 <= x < 1
//     // Error versus actual 2^x is less than 0.013%.
//     uint64_t factor = (195766423245049*exponent + 
//                        971821376*exponent*exponent + 
//                        5127*exponent*exponent*exponent + (1ll<<47))>>(rbits*3);
//     nextTarget += (nextTarget * factor) >> rbits;

//     const arith_uint256 powLimit = UintToArith256(params.powLimit);
//     if (nextTarget > powLimit) {
//         return powLimit.GetCompact();
//     }

//     return nextTarget.GetCompact();
// }


// int main() {
//     CBlockIndex const* pindexPrev;
//     CBlockHeader const* pblock;
//     Consensus::Params params;
//     // int32_t const nforkHeight = gArgs.GetArg("-asertactivationheight", INT_MAX);
//     int32_t const nforkHeight = 0;

//     auto res = GetNextASERTWorkRequired(pindexPrev, pblock, params, nforkHeight);

//     return 0;
// }