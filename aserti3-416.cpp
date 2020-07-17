// g++ -std=c++11 aserti3-416.cpp
#include <cassert>
#include <cstdint>
#include <cstring>

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
                                  const int32_t nforkHeight) {
    // This cannot handle the genesis block and early blocks in general.
    assert(pindexPrev);

    // Special difficulty rule for testnet:
    // If the new block's timestamp is more than 2* 10 minutes then allow
    // mining of a min-difficulty block.
    if (params.fPowAllowMinDifficultyBlocks &&
        (pblock->GetBlockTime() >
         pindexPrev->GetBlockTime() + 2 * params.nPowTargetSpacing)) {
        return UintToArith256(params.powLimit).GetCompact();
    }

    // Diff halves/doubles for every 2 days behind/ahead of schedule we get
    const uint64_t tau = 2*24*60*60; 

    // This algorithm uses fixed-point math. The lowest rbits bits are after
    // the radix, and represent the "decimal" (or binary) portion of the value
    const uint8_t rbits = 16; 

    const CBlockIndex *pforkBlock = &pindexPrev[nforkHeight];

    assert(pforkBlock != nullptr);
    assert(pindexPrev->nHeight >= params.DifficultyAdjustmentInterval());

    int32_t nTimeDiff = pindexPrev->nTime - pforkBlock->GetBlockHeader().nTime;
    int32_t nHeightDiff = pindexPrev->nHeight - pforkBlock->nHeight;

    const arith_uint256 origTarget = arith_uint256().SetCompact(pforkBlock->nBits);
    arith_uint256 nextTarget = origTarget;

    // Ultimately, we want to approximate the following ASERT formula, using only integer (fixed-point) math:
    //     new_target = old_target * 2^((blocks_time - IDEAL_BLOCK_TIME*(height_diff+1)) / tau)

    // First, we'll calculate the exponent:
    int64_t exponent = ((nTimeDiff - params.nPowTargetSpacing * (nHeightDiff+1)) << rbits) / tau;

    // Next, we use the 2^x = 2 * 2(x-1) identity to shift our exponent into the [0, 1) interval.
    // The truncated exponent tells us how many shifts we need to do
    // Note: This needs to be a right shift. Right shift rounds downward, whereas division rounds towards zero.
    int8_t shifts = exponent >> rbits;
    if (shifts < 0) {
        nextTarget = nextTarget >> -shifts;
    } else {
        nextTarget = nextTarget << shifts;
    }
    exponent -= (shifts << rbits);

    // Now we compute an approximated target * 2^(exponent)
    // 2^x ~= (1 + 0.695502049*x + 0.2262698*x**2 + 0.0782318*x**3) for 0 <= x < 1
    // Error versus actual 2^x is less than 0.013%.
    uint64_t factor = (195766423245049*exponent + 
                       971821376*exponent*exponent + 
                       5127*exponent*exponent*exponent + (1ll<<47))>>(rbits*3);
    nextTarget += (nextTarget * factor) >> rbits;

    const arith_uint256 powLimit = UintToArith256(params.powLimit);
    if (nextTarget > powLimit) {
        return powLimit.GetCompact();
    }

    return nextTarget.GetCompact();
}


// int main() {
//     CBlockIndex const* pindexPrev;
//     CBlockHeader const* pblock;
//     Consensus::Params params;
//     // int32_t const nforkHeight = gArgs.GetArg("-asertactivationheight", INT_MAX);
//     int32_t const nforkHeight = 0;

//     auto res = GetNextASERTWorkRequired(pindexPrev, pblock, params, nforkHeight);

//     return 0;
// }