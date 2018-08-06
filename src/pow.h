// Copyright (c) 2017-2018 The Popchain Core Developers

#ifndef BITCOIN_POW_H
#define BITCOIN_POW_H

#include "consensus/params.h"

#include <stdint.h>

class CBlockHeader;
class CBlockIndex;
class uint256;
class arith_uint256;
/*popchain ghost*/
unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params&);
/*popchain ghost*/
unsigned int CalculateNextWorkRequired(arith_uint256 bnAvg,
                                       int64_t nLastBlockTime, int64_t nFirstBlockTime,
                                       const Consensus::Params&);

/** Check whether a block hash satisfies the proof-of-work requirement specified by nBits */
/*popchain ghost*/
bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params&);
//bool CheckProofOfWork(uint256 hash, uint256 nDifficulty, const Consensus::Params&);
/*popchain ghost*/
arith_uint256 GetBlockProof(const CBlockIndex& block);

/** Return the time it would take to redo the work difference between from and to, assuming the current hashrate corresponds to the difficulty at tip, in seconds. */
int64_t GetBlockProofEquivalentTime(const CBlockIndex& to, const CBlockIndex& from, const CBlockIndex& tip, const Consensus::Params&);

#endif // BITCOIN_POW_H
