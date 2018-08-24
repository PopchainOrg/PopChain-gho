// Copyright (c) 2017-2018 The Popchain Core Developers

#include "base58.h"
#include "chainparams.h"
#include "consensus/merkle.h"
#include "pow.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>
#include "arith_uint256.h"
#include "chainparamsseeds.h"

//#define GENESIS_GENERATION

#ifdef GENESIS_GENERATION
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cstdio>
#include <string>
#include "utiltime.h"
#include <random>
#include <cmath>
#include <iomanip>
#include <util.h>
#include <new>
#include <chrono>
#include <thread>
#include <mutex>
#include "random.h"

#include <sys/time.h>

typedef uint32_t uint;
typedef CBlockHeader ch;
typedef long long ll;

static std::mutex mtx;

//test cnt 1000 times time
int64_t getCurrentTime()  
{      
   struct timeval tv;      
   gettimeofday(&tv,NULL);   
   return tv.tv_sec * 1000 + tv.tv_usec / 1000;      
}  

// find a genesis in about 10-20 mins
void _get(const ch * const pblock, const arith_uint256 hashTarget)
{
    uint256 hash;
    ch *pb = new ch(*pblock);
	
    for (int cnt = 0, tcnt=0; true; ++cnt,++tcnt)
    {
        uint256 hash = pb->GetHash();

		//std::cout<<"hex hash = "<<hash.GetHex()<<std::endl;
				
        if (UintToArith256(hash) <= hashTarget) break;
        pb->nNonce = ArithToUint256(UintToArith256(pb->nNonce) + 1);
        if (cnt > 1e3)
        {
            pb->nTime = GetTime();
            cnt = 0;
        }
		if (tcnt !=0 and tcnt % 1000 == 0)
        {
            std::cout<<"cryptopop tcnt = "<<tcnt<<" time = "<<getCurrentTime()<<" ms"<<std::endl;       
        }

    }
    
    std::lock_guard<std::mutex> guard(mtx);
    std::cout << "\n\t\t----------------------------------------\t" << std::endl;
    std::cout << "\t" << pb->ToString() << std::endl;
    std::cout << "\n\t\t----------------------------------------\t" << std::endl;
    delete pb;

    // stop while found one
    assert(0);
}

static void findGenesis(CBlockHeader *pb, const std::string &net)
{
	/*popchain ghost*/
    //arith_uint256 hashTarget = arith_uint256().SetCompact(pb->nBits);
    arith_uint256 hashTarget = maxUint256Div(pb->nDifficulty);

	/*popchain ghost*/
    std::cout << " finding genesis using target " << hashTarget.ToString()
        << ", " << net << std::endl;

    std::vector<std::thread> threads;

    for (int i = 0; i < std::min(GetNumCores(), 100); ++i)
    //for (int i = 0; i < 1; ++i)
    {
        if (i >= 0)
        {
		// Randomise nonce
        	arith_uint256 nonce = UintToArith256(GetRandHash());
        	// Clear the top and bottom 16 bits (for local use as thread flags and counters)
        	nonce <<= 32;
        	nonce >>= 16;
        	pb->nNonce = ArithToUint256(nonce);
		//std::cout<<"i = "<<i<<"    nNonce = "<<pb->nNonce.ToString()<<std::endl;	
        }
        threads.push_back(std::thread(_get, pb, hashTarget));
    }

    for (auto &t : threads)
    {
        t.join();
    }
}
#endif
/*popchain ghost*/
static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint256 nNonce, uint64_t nDifficulty, uint32_t nBits, int32_t nVersion, const CAmount &genesisReward)
//static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint256 nNonce, uint32_t nBits, int32_t nVersion, const CAmount &genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = CAmount(genesisReward);
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
	/*popchain ghost*/
    genesis.nDifficulty = nDifficulty;
    genesis.nBits    = nBits;
    /*popchain ghost*/
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
	/*popchain ghost*/
	genesis.hashUncles.SetNull();
	genesis.nCoinbase.SetNull();
	genesis.nNumber = 0;	
	genesis.vuh.clear();
	genesis.hashUncles = BlockUncleRoot(genesis);
	/*popchain ghost*/
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    genesis.hashClaimTrie = uint256S("0x1");
    return genesis;
}
/*popchain ghost*/
/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=00000ffd590b14, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=e0028e, nTime=1390095618, nBits=1e0ffff0, nNonce=28917698, vtx=1)
 *   CTransaction(hash=e0028e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d01044c5957697265642030392f4a616e2f3230313420546865204772616e64204578706572696d656e7420476f6573204c6976653a204f76657273746f636b2e636f6d204973204e6f7720416363657074696e6720426974636f696e73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0xA9037BAC7050C479B121CF)
 *   vMerkleTree: e0028e
 */
 /*popchain ghost*/

//static CBlock CreateGenesisBlock(uint32_t nTime, uint256 nNonce, uint256 nDifficulty, int32_t nVersion, const int64_t& genesisReward)
static CBlock CreateGenesisBlock(uint32_t nTime, uint256 nNonce, uint64_t nDifficulty, uint32_t nBits, int32_t nVersion, const int64_t& genesisReward)
{
    const char* pszTimestamp = "pop hold value testnet.";
    //const char* pszTimestamp = "hope u happiness in canada gg by armin.";
    const CScript genesisOutputScript = CScript() << ParseHex("034c73d75f59061a08032b68369e5034390abc5215b3df79be01fb4319173a88f8") << OP_CHECKSIG;
    //std::vector<unsigned char > printscript(genesisOutputScript.begin(),genesisOutputScript.end);
    //std::cout<< StrHex(printscript)<<endl ;
    //return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nDifficulty, nVersion, genesisReward);
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nDifficulty, nBits, nVersion, genesisReward);
}
/*popchain ghost*/

/*popchain ghost*/
//static CBlock CreateGenesisBlock1(uint32_t nTime, uint256 nNonce, uint256 nDifficulty, int32_t nVersion, const int64_t& genesisReward)                                                                                                                
static CBlock CreateGenesisBlock1(uint32_t nTime, uint256 nNonce, uint64_t nDifficulty, uint32_t nBits, int32_t nVersion, const int64_t& genesisReward)
{
    const char* pszTimestamp = "Change the World with Us. 22/May/2018, 00:00:00, GMT";
    //const char* pszTimestamp = "Shall I compare thee to a summer's day? 23/Jan/2011, Sy Forever";
    const CScript genesisOutputScript = CScript() << ParseHex("041c508f27e982c369486c0f1a42779208b3f5dc96c21a2af6004cb18d1529f42182425db1e1632dc6e73ff687592e148569022cee52b4b4eb10e8bb11bd927ec0") << OP_CHECKSIG;
    //return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nDifficulty, nVersion, genesisReward);
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nDifficulty, nBits, nVersion, genesisReward);
}
/*popchain ghost*/

/**
 * Main network
 * Popchain DevTeam
 */


class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";

        // reward setting
        consensus.premine = int64_t(1e8 * COIN);                            // premine
        consensus.genesisReward = int64_t(1 * COIN);                        // genesis
        consensus.minerReward4 = int64_t(112.966 * COIN);                   // miners
        consensus.minerReward5 = int64_t(133.775 * COIN);
        consensus.foundersReward = int64_t(4166666.667 * COIN);             // founders
        consensus.colleteral = int64_t(1e4 * COIN);                         // popnode colleteral

        consensus.nSubsidyHalvingInterval = 840960;                     // 4 years, 24 * 60 / 2.5 * 365 * 4 
        consensus.nInstantSendKeepLock = 24;
        consensus.nSuperblockStartBlock = 100;
        consensus.nSuperblockCycle = 576 * 30; 				            // ~(60*24*30)/2.6, actual number of blocks per month is 200700 / 12 = 16725
        consensus.nPopnodeMinimumConfirmations = 15;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = 0; 
        consensus.BIP34Hash = uint256S("0x0000083331b8aa57aaae020d79aabe4136ebea6ce29be3a50fcaa2a55777e79c");
		/*popchain ghost*/
        //consensus.powLimit = uint256S("0x000009b173000000000000000000000000000000000000000000000000000000");
        consensus.powLimit = uint256S("0x000009b173149ff8b3a49a388d7ebdd0e1eb76d294f9e5f648f254d81ad0938a");
        consensus.difficultyBoundDivisor = 2048;
        consensus.difficultyRapidFitDivisor = 200;
        //consensus.durationLimit = 13;
        consensus.minimumDifficulty = 1730830;                          // minidifficulty for target
        consensus.nYolandaTime = 172800;
		/*popchain ghost*/
        consensus.nPowAveragingWindow = 17;
        //consensus.nPowMaxAdjustDown = 32;                               // 32% adjustment down
        //consensus.nPowMaxAdjustUp = 48;                                 // 48% adjustment up
        //consensus.nPowTargetTimespan = 24 * 60 * 60;                    // Pop: 1 day
        consensus.nPowTargetSpacing = 2.5 * 60;                         // Pop: 2.5 minutes
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1916;                // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016;                      // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1524045652;              // 2018/4/18 18:0:52
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1555581652;                // 2019/4/18 18:0:52

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xb3;
        pchMessageStart[1] = 0x01;
        pchMessageStart[2] = 0x6f;
        pchMessageStart[3] = 0xb1;
		vAlertPubKey = ParseHex("028efd0f3c697689f8f1f6744edbbc1f85871b8c51218ddd89d90a3e435d1a8691");
        nDefaultPort = 2888;
        nMaxTipAge = 6 * 60 * 60; // ~144 blocks behind -> 2 x fork detection time, was 24 * 60 * 60 in bitcoin
        nPruneAfterHeight = 100000;
		/*popchain ghost*/
        arith_uint256 nTempBit =  UintToArith256( consensus.powLimit);
        genesis = CreateGenesisBlock1(1533916035, uint256S("0x0000bdefbb1cb97e50d2ac4c9cda0e61195d01d3441180448453fc484ce51ed5"), consensus.minimumDifficulty, nTempBit.GetCompact(), 1, consensus.genesisReward);
		/*popchain ghost*/
#ifdef GENESIS_GENERATION
        //arith_uint256 a("0x000009b173000000000000000000000000000000000000000000000000000000");
        arith_uint256 a("0x000009b173149ff8b3a49a388d7ebdd0e1eb76d294f9e5f648f254d81ad0938aL");
		/*popchain ghost*/
        std::cout << "\tpow:\t" << a.GetCompact()  << " "<< nTempBit.GetCompact() << std::endl;
		/*popchain ghost*/
        //findGenesis(&genesis, "main");
#endif
        consensus.hashGenesisBlock = genesis.GetHash();
        //assert(consensus.hashGenesisBlock == uint256S("0x000000747aad802a9081bd47533cf105a6e7203ca6313155adf41bd11bf0f01f"));
        assert(consensus.hashGenesisBlock == uint256S("0x000004cb6f548cb60b082d814aaf7172101f85b8fd5a0bf5a8677a358ce7a60d"));
        assert(genesis.hashMerkleRoot == uint256S("0x69de4474f3172f2366a11b9d5a2b9138fb5bbb0b77713d42fdfe69fc64a34162"));

        // Pop addresses start with 'P'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,56);
        // Pop script addresses start with 'S'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,63);
        // Pop private keys start with '5' or 'K' or 'L'(as in Bitcoin)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,0x80);
        // Pop BIP32 pubkeys start with 'xpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        // Pop BIP32 prvkeys start with 'xprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();
        // Pop BIP44 coin type is '247'
        base58Prefixes[EXT_COIN_TYPE]  = boost::assign::list_of(0x80)(0x00)(0x00)(0xf7).convert_to_container<std::vector<unsigned char> >();

        //vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));
        vFixedSeeds.clear();
        vSeeds.clear();		
        vSeeds.push_back(CDNSSeedData("pop.xxx", "seed1.pop.xxx"));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        nPoolMaxTransactions = 3;
        nFulfilledRequestExpireTime = 60*60; // fulfilled requests expire in 1 hour
        strSporkPubKey = "03dc480bc3e569ea1ceed5ab8ed5904abd3e938e0f7e594ebf82397e7830fa56a5";

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (0, uint256S("0x000004cb6f548cb60b082d814aaf7172101f85b8fd5a0bf5a8677a358ce7a60d")),
             0,                       // * UNIX timestamp of last checkpoint block
             0,                                // * total number of transactions between genesis and last checkpoint
                                               //   (the tx=... number in the SetBestChain debug.log lines)
             0                                 // * estimated number of transactions per day after checkpoint
        };

        // Founders reward script expects a vector of 2-of-3 multisig addresses
        vFoundersRewardAddress = {
            "PoWvpLL3iDxm2GJSZQRspzRawEFEs3BbG8", /* main-index: 0*/
            "PcjJdnb9Fp35TRTAz61vp6MPXTv3ApcMhj", /* main-index: 1*/
	    };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 * Popchain DevTeam
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";

        // reward setting
        consensus.premine = int64_t(1e7 * COIN);                            // premine
        consensus.genesisReward = int64_t(1 * COIN);                        // genesis                                                           
        consensus.minerReward4 = int64_t(300 * COIN);                   // miners
        consensus.minerReward5 = int64_t(535.103 * COIN);
        consensus.foundersReward = int64_t(200000 * COIN);             // founders

        consensus.colleteral = int64_t(1e4 * COIN);                         // popnode colleteral

        consensus.nSubsidyHalvingInterval = 840960;
        consensus.nInstantSendKeepLock = 6;
        consensus.nSuperblockStartBlock = 30;
        consensus.nSuperblockCycle = 50; 				 // Superblocks can be issued hourly on testnet
        consensus.nPopnodeMinimumConfirmations = 2;
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 100;
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256S("00065185c3ffa77ff797ea3141fba9b1ab76a0f336863dec1199042ca5560fc4");
        /* popchain ghost */
        //consensus.powLimit = uint256S("0x000fffffff000000000000000000000000000000000000000000000000000000");
        consensus.powLimit = uint256S("0x000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.difficultyBoundDivisor = 2048;
        consensus.difficultyRapidFitDivisor = 50;
        //consensus.durationLimit = 13;
        consensus.minimumDifficulty = 4096;
        consensus.nYolandaTime = 500;
      
        /* popchain ghost */
        consensus.nPowAveragingWindow = 17;
        //consensus.nPowMaxAdjustDown = 32;                               // 32% adjustment down
        //consensus.nPowMaxAdjustUp = 16;                                 // 16% adjustment up
        //consensus.nPowMaxAdjustUp = 48;                                 // 48% adjustment up
        //consensus.nPowTargetTimespan = 24 * 60 * 60;                    // Pop: 1 day
        //consensus.nPowTargetSpacing = 2.5 * 60;                         // Pop: 2.5 minutes
        consensus.nPowTargetSpacing = 15;
        consensus.fPowAllowMinDifficultyBlocks = true;
        //consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512;                // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; 			// nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601;                // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 123076799;                  // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1524057440;               // 2018/4/18 20:57:16
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout =   1555592236;               // 2019/4/18 20:57:16

        pchMessageStart[0] = 0xc2;
        pchMessageStart[1] = 0xe6;
        pchMessageStart[2] = 0xce;
        pchMessageStart[3] = 0xf3;
        vAlertPubKey = ParseHex("0244a0bb22e931bf59cc8a434d9d22bd2fa493f579bd2659bc9188361d78bdc45f");
        nDefaultPort = 12888;
        nMaxTipAge = 0x7fffffff; 		// allow mining on top of old blocks for testnet
        nPruneAfterHeight = 1000;
	    /*popchain ghost*/
   		arith_uint256 nTempBit =  UintToArith256( consensus.powLimit); 
        //genesis = CreateGenesisBlock(1529894661, uint256S("0000374f752799accf0ae43870b1764e17fc0e4a45ebd19adb80597bf0c30097"), nTempBit.GetCompact(), 1,  1 * COIN);
        genesis = CreateGenesisBlock(1529894661, uint256S("0x00006410bb8e706a7f96be15cb4217e7f9b1a4fe1000b0f2bab3f03cdbc60047"), consensus.minimumDifficulty, nTempBit.GetCompact(), 1,  1 * COIN);

		/*popchain ghost*/
#ifdef GENESIS_GENERATION
        arith_uint256 a("0x000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
		/*popchain ghost*/
        std::cout << "pow limit : " << a.GetCompact()<< " "<< nTempBit.GetCompact() << std::endl;
		/*popchain ghost*/
        //findGenesis(&genesis, "testnet");
#endif
        consensus.hashGenesisBlock = genesis.GetHash();
        //assert(consensus.hashGenesisBlock == uint256S("00077ade31e190b0dccd194c02c8e84bf77db7d037d8a8c3c2c82f89145e3e0a"));
        assert(consensus.hashGenesisBlock == uint256S("0x0008247432537c397b0144615ee72ea2aca440079b749d288f1c47c387b71511"));
        assert(genesis.hashMerkleRoot == uint256S("0x6f73646aa71aeec2163e047e0028e2c4313f3e88d4fb3e1ade176c56e1a148c4"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("uosrguosio.org","popchain.uosio.org"));

        // Testnet Pop addresses start with 'p'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,118);
        // Testnet Pop script addresses start with 's'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,125);
        // Testnet private keys start with '9' or 'c'(as in Bitcoin)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,0xef);
        // Testnet Pop BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        // Testnet Pop BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
        // Testnet Pop BIP44 coin type is '1' (All coin's testnet default)
        base58Prefixes[EXT_COIN_TYPE]  = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();

        //vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 3;
        nFulfilledRequestExpireTime = 5*60;                          // fulfilled requests expire in 5 minutes
        strSporkPubKey = "02284dd24544e031b1b575fc4bf720a57d57425157290a9882f4d0dd192b1a316c";

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (0, uint256S("0x0008247432537c397b0144615ee72ea2aca440079b749d288f1c47c387b71511")),
             0,     // * UNIX timestamp of last checkpoint block
             0,              // * total number of transactions between genesis and last checkpoint
                             //   (the tx=... number in the SetBestChain debug.log lines)
             0               // * estimated number of transactions per day after checkpoint
        };

        // Founders reward script expects a vector of 2-of-3 multisig addresses
        vFoundersRewardAddress = {
            "pbFyUjHfZB8BBivQS1LXh8EaJNH5jjGbzk",
            "pbvTm479A2XTp2nCa8Z9qwAhindNbKarrX"
        };
    }
};

static CTestNetParams testNetParams;

/**
 * Regression test
 * Popchain DevTeam
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        // reward setting
        consensus.premine = int64_t(1e8 * COIN);                            // premine    
        consensus.genesisReward = int64_t(1 * COIN);                        // genesis
        consensus.minerReward4 = int64_t(112.966 * COIN);                   // miners
        consensus.minerReward5 = int64_t(535.103 * COIN);
        consensus.foundersReward = int64_t(4166666.667 * COIN);             // founders
        consensus.colleteral = int64_t(1e4 * COIN);                         // popnode colleteral
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nInstantSendKeepLock = 6;
        consensus.nSuperblockStartBlock = 1500;
        consensus.nSuperblockCycle = 10;
        consensus.nPopnodeMinimumConfirmations = 1;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = -1;                                     // BIP34 has not necessarily activated on regtest
        consensus.BIP34Hash = uint256();
        // ghost
        consensus.powLimit = uint256S("0x0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f");
        consensus.nPowAveragingWindow = 17;
        //consensus.nPowMaxAdjustDown = 0;                                // Turn off adjustment down
        //consensus.nPowMaxAdjustUp = 0;                                  // Turn off adjustment up
        //consensus.nPowTargetTimespan = 24 * 60 * 60;                    // Pop: 1 day
        consensus.nPowTargetSpacing = 2.5 * 60;                         // Pop: 2.5 minutes
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        /*popchain ghost*/
        consensus.difficultyBoundDivisor = 2048;
        consensus.difficultyRapidFitDivisor = 20;
        //consensus.durationLimit = 13;
        consensus.minimumDifficulty = 17;
        consensus.nYolandaTime = 100;
        /*popchain ghost*/
        consensus.nRuleChangeActivationThreshold = 108;                 // 75% for testchains
        consensus.nMinerConfirmationWindow = 144;                       // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1000000000000ULL;

        pchMessageStart[0] = 0xf0;
        pchMessageStart[1] = 0xc5;
        pchMessageStart[2] = 0xbb;
        pchMessageStart[3] = 0xd0;
        nMaxTipAge = 6 * 60 * 60; // ~144 blocks behind -> 2 x fork detection time, was 24 * 60 * 60 in bitcoin
        nDefaultPort = 12888;
        nPruneAfterHeight = 1000;
		/*popchain ghost*/
		arith_uint256 nTempBit =  UintToArith256( consensus.powLimit);
        genesis = CreateGenesisBlock1(1529894661, uint256S("0x00009d7ffdffdfe3f6b2f57630e2ac492738972c1ae580e4faea92ff03e90001"), consensus.minimumDifficulty, nTempBit.GetCompact(), 1, 1 * COIN);
		/*popchain ghost*/
#ifdef GENESIS_GENERATION
        //findGenesis(&genesis, "regtest");
#endif
        consensus.hashGenesisBlock = genesis.GetHash();
        //assert(consensus.hashGenesisBlock == uint256S("01bb1c4d83e5cd73ad4fe568fa2b50469d33def5703dca7e90e06f32f273b95d"));
        assert(consensus.hashGenesisBlock == uint256S("0x00c7cf91fa452792f50bc2f15cb37e439174e1ee741888b8d12cd258d15f874c"));
        assert(genesis.hashMerkleRoot == uint256S("0x69de4474f3172f2366a11b9d5a2b9138fb5bbb0b77713d42fdfe69fc64a34162"));

        vFixedSeeds.clear();                                             //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();                                                  //! Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        nFulfilledRequestExpireTime = 5*60;                              // fulfilled requests expire in 5 minutes

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
            (0, uint256S("0x00c7cf91fa452792f50bc2f15cb37e439174e1ee741888b8d12cd258d15f874c")),
             0,
             0,
             0
        };
        // Regtest Pop addresses start with 'y'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,140);
        // Regtest Pop script addresses start with 'q'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,120);
        // Regtest private keys start with 'm'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,0xef);
        // Regtest Pop BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        // Regtest Pop BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
        // Regtest Pop BIP44 coin type is '1' (All coin's testnet default)
        base58Prefixes[EXT_COIN_TYPE]  = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();

        // Founders reward script expects a vector of 2-of-3 multisig addresses
        vFoundersRewardAddress = {"pbFyUjHfZB8BBivQS1LXh8EaJNH5jjGbzk"};
   }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
        return testNetParams;
    else if (chain == CBaseChainParams::REGTEST)
        return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

// Block height must be >1 and <last founders reward block height
// Index variable i ranges from 0 - (vFoundersRewardAddress.size()-1)
std::string CChainParams::GetFoundersRewardAddressAtHeight(int height) const
{
    assert(height > 1 && height < consensus.endOfFoundersReward());

    height /= consensus.nSuperblockCycle;
    size_t i = height % vFoundersRewardAddress.size();
    return vFoundersRewardAddress[i];
}

// Block height must be >1 and <last founders reward block height
// The founders reward address is expected to be a multisig (P2SH) address
CScript CChainParams::GetFoundersRewardScriptAtHeight(int height) const
{
    assert(height > 1 && height < consensus.endOfFoundersReward());

    CBitcoinAddress address(GetFoundersRewardAddressAtHeight(height).c_str());
    assert(address.IsValid());
    CScript scriptPubKey = GetScriptForDestination(address.Get());
    return scriptPubKey;
}

std::string CChainParams::GetFoundersRewardAddressAtIndex(int i) const
{
    assert(i >= 0 && i < int(vFoundersRewardAddress.size()));
    return vFoundersRewardAddress[i];
}

