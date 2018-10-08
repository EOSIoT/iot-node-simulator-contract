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

#include <eosiolib/eosio.hpp>
#include <math.h>
#include <string>

using std::string;

class iotnodesim : public eosio::contract {

public:

    iotnodesim(account_name self) : contract(self), state(self, self) {}

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


    // @abi table statetable i64
    struct simstate {
        account_name  host;

        stats latency_stats;
        stats tps_stats;

        uint32_t num_transactions;
        uint32_t time_first_tx_s;

        simstate() {
            reset_state();
        }

        void reset_state() {
            latency_stats.reset();
            tps_stats.reset();
            num_transactions = 0;
            time_first_tx_s = 0;
        }

        auto primary_key() const { return host; }

        EOSLIB_SERIALIZE(simstate, (host)(latency_stats)(tps_stats)(num_transactions)(time_first_tx_s))
    };

    /* Node simulation stress test state 
     * View this on the chain with:
     *    cleos get table eosiotstess <user> state
     */
    typedef eosio::multi_index<N(statetable), simstate> statetable;

    /// @abi action
    /// Start the simulation - only the contract host can call this
    void start();

    /// @abi action
    /// Restart the simulation - only the contract host can call this
    void restart();

    /// @abi action
    /// Submit data to the platform
    /// account name ensures user signed transaction with existing account
    /// unique_id is required to ensure each transaction is unique
    /// node_time is node's current time at submission, in seconds UTC (same timebase as EOS now() API)
    void submit(account_name user, string unique_id, uint32_t node_time, string memo);
//    void submit(account_name user);

    /// @abi action
    /// Do something
    void version();


private:

    /* Instance of the simulation state table */
    statetable state;

};
