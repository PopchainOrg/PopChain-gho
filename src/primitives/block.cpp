// Copyright (c) 2017-2018 The Popchain Core Developers

#include "primitives/block.h"

#include "hash.h"
#include "tinyformat.h"
#include "utilstrencodings.h"
//#include "crypto/common.h"

// popchain ghost calc blockheader hash
uint256 CBlockHeader::GetHash() const
{
//	return SerializeHash(*this);
	uint256 hash;
/*popchain ghost*/
	CryptoPop(this, (unsigned char *)&hash);
/*popchain ghost*/

//	view_data_u8("PoW 3", (unsigned char *)&hash, OUTPUT_LEN); 
//	std::cout<<"gethex() ="<<hash.GetHex()<<std::endl;
//	std::cout<<"tostring ="<<hash.ToString()<<std::endl; 
	return hash;	
}

std::string CBlockHeader::ToString() const                                                                                                                                                                                                                                   
{
/*popchain ghost*/
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=%d, hashPrevBlock=%s, hashUncles=%s, nCoinbase=%s, hashMerkleRoot=%s, hashClaimTrie=%s, nTime=%u, nDifficulty=%u, nBits=%08x, nNonce=%s)\n",
		GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashUncles.ToString(),
        nCoinbase.ToString(),/*need change by base58 ?*/     
        hashMerkleRoot.ToString(),
        hashClaimTrie.ToString(),
        nTime,
        nDifficulty,
        nBits, nNonce.ToString());
/*popchain ghost*/
    return s.str();
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=%d, hashPrevBlock=%s, hashUncles=%s, nCoinbase=%s, hashMerkleRoot=%s, hashClaimTrie=%s, nTime=%u, nDifficulty=%u, nBits=%08x, nNonce=%s, vtx=%u, vuh=%u)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashUncles.ToString(),/*popchain ghost*/
        nCoinbase.ToString(),/*popchain ghost*/
        hashMerkleRoot.ToString(),
        hashClaimTrie.ToString(),
        nTime,
        nDifficulty,/*popchain ghost*/
        nBits, nNonce.ToString(),
        vtx.size(),
        vuh.size()/*popchain ghost*/);
    for (unsigned int i = 0; i < vtx.size(); i++)
    {
        s << "  " << vtx[i].ToString() << "\n";
    }

	/*popchain ghost*/
	
	for (unsigned int i = 0; i < vuh.size(); i++)
    {
        s << "  " << vuh[i].ToString() << "\n";
    }
   
	/*popchain ghost*/
	
    return s.str();
}

/*popchain ghost*/

uint256 BlockUncleRoot(const CBlock& block)
{
	uint256 hashUncles = uint256();
    std::vector<uint256> uncles;
    uncles.resize(block.vuh.size());
    for (size_t s = 0; s < block.vuh.size(); s++) {
        uncles[s] = block.vuh[s].GetHash();
    }

	if(uncles.size() == 1){
		CHash256().Write(uncles[0].begin(), 32).Write(uncles[0].begin(), 32).Finalize(hashUncles.begin());
	}
	else if(uncles.size() == 2){
		CHash256().Write(uncles[0].begin(), 32).Write(uncles[1].begin(), 32).Finalize(hashUncles.begin());
	}
	
    return hashUncles;
}


/*popchain ghost*/

