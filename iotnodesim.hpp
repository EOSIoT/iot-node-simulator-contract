/*
MIT License

Copyright(c) 2018 Evan Ross

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
* \file
*         This smart contract runs on the EOS blockchain and acts
*         as the "end point" accepting node data submissions while
*         calculating statistics on the data like transaction latency
*         and throughput (TPS).
*
* \author
*         Evan Ross <contact@firmwaremodules.com>
*/

#include <eosio/eosio.hpp>
#include <math.h>
#include <string>

using namespace eosio;

using std::string;

class [[eosio::contract("iotnodesim")]] iotnodesim : public contract {

public:

    using contract::contract;

    iotnodesim(name receiver, name code, datastream<const char*> ds) :contract(receiver, code, ds) {}

    struct stats {
        double min;
        double max;
        double var;
        double mean;

        void reset() {
            min = HUGE_VAL;
            max = 0.0;
            var = 0.0;
            mean = 0.0;
        }
    };


    
    struct [[eosio::table]] simstate {
        name  host;

        uint32_t lifetime_resets;

        stats latency_stats;
        stats tps_stats;

        uint32_t num_transactions;
        uint32_t time_first_tx_s;
        uint32_t time_last_tx_s;

        //simstate() {
        //    reset_state();
        //}

        void reset_state() {
            latency_stats.reset();
            tps_stats.reset();
            num_transactions = 0;
            time_first_tx_s = 0;
            time_last_tx_s = 0;
        }

        uint64_t primary_key() const { return host.value; }

        
    };

    /* Node simulation stress test state 
     * View this on the chain with:
     *    cleos get table eosiotstess <user> state
     */
    typedef eosio::multi_index<"statetable"_n, simstate> statetable;


    /// Start the simulation - only the contract host can call this
    /// Creates the state table. 
    [[eosio::action]]
    void start();

    
    /// Restart the simulation - anyone can call this
    [[eosio::action]]
    void restart(name node);

    
    /// Stop the simulation - only the contract host can call this
    /// Removes the state table.
    [[eosio::action]]
    void stop();

    
    /// Submit data to the platform
    /// account name ensures user signed transaction with existing account
    /// unique_id is required to ensure each transaction is unique
    /// node_time is node's current time at submission, in seconds UTC (same timebase as EOS now() API)
    [[eosio::action]]
    void submit(name user, string unique_id, uint32_t node_time, string memo);
//    void submit(account_name user);

    
    /// Do something
    [[eosio::action]]
    void version();


private:

 
};
