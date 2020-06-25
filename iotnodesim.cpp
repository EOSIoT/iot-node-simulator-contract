/*
MIT License

Copyright(c) 2018-2020 Evan Ross, Measurement Earth

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
*         This smart contract runs on an EOSIO software blockchain and acts
*         as the "end point" accepting node data submissions while
*         calculating statistics on the data like transaction latency
*         and throughput (TPS).
*
* \author
*         Evan Ross <contact@firmwaremodules.com>
*/

#include "iotnodesim.hpp"
#include <eosio/system.hpp>

using namespace eosio;

#define DEBUG 0
#if DEBUG
#define PRINT(...) print(__VA_ARGS__)
#else
#define PRINT(...)
#endif

// For table instantiation, please note:
//
// The first parameter "code", which specifies the owner of this table. As the 
// owner, the account will be charged for storage costs. Also, only that account can modify or delete the data in this
// table unless another payer is specified. Here we use the get_self() function which will pass the name of this
// contract.
// The second parameter "scope" which ensures the uniqueness of the table in the scope of this contract. In this case,
// since we only have one table we can use the value from get_first_receiver(). The value returned from the
// get_first_receiver function is the account name on which this contract is deployed to.


void iotnodesim::start()
{
    require_auth(get_self());
    PRINT("start!");

    statetable state(get_self(), get_first_receiver().value);

    // Check if stats data already exists
    auto itr = state.find(get_self().value);
    check(itr == state.end(), "stats already started");

    state.emplace(get_self(), [&](auto& s) {
        s.reset_state();
        s.host = get_self();
        s.lifetime_resets = 0;
    });

}

/// Restart the simulation
void iotnodesim::restart(name node)
{
    require_auth(node);
    PRINT("restart!");

    statetable state(get_self(), get_first_receiver().value);

    // Check if stats data already exists
    auto itr = state.find(get_self().value);
    check(itr != state.end(), "stats not yet started");

    state.modify(itr, itr->host, [](auto& s) {
        s.reset_state();
        s.lifetime_resets++;
    });
}

#if 1
/// Stop the simulation
void iotnodesim::stop()
{
    require_auth(get_self());
    PRINT("stop!");

    statetable state(get_self(), get_first_receiver().value);

    // Check if stats data already exists
    auto itr = state.find(get_self().value);
    check(itr != state.end(), "stats not yet started");

    state.erase(itr);
}
#endif

void compute_stats(
    uint32_t now_time,
    uint32_t node_time)
{

}


void iotnodesim::submit(name user, string unique_id, uint32_t node_time, string memo)
{ 
    /* Require that submitter has signed this action */
    /* In the interest of simplicity any account can "submit"
     * to the simulation.
     */
    require_auth(user);

    statetable state(get_self(), get_first_receiver().value);
    
    // Check if stats data already exists
    auto itr = state.find(get_self().value);
    check(itr != state.end(), "stats not yet started");
    check(node_time != 0, "invalid node time");


    const uint32_t now_time = current_time_point().sec_since_epoch();
#if DEBUG
    PRINT("Data submitted by: ", name{ user }, " id: ", unique_id, " memo: ", memo, "\n");
    PRINT(now_time, "\n");
    PRINT(node_time, "\n");
#endif

    /* 'itr' is a pointer to the table structure */

    /* Update the current statistics */
    const uint32_t latency = now_time - node_time;

    state.modify(itr, itr->host, [&](auto& s) {
        s.time_last_tx_s = now_time;
        if (s.num_transactions == 0) {
            s.time_first_tx_s = now_time;
        }
        s.num_transactions++;
        const uint32_t num_tx = s.num_transactions;

        if (latency < s.latency_stats.min) s.latency_stats.min = latency;
        if (latency > s.latency_stats.max) s.latency_stats.max = latency;
        
        /* for average we will weight by number of transactions */
        s.latency_stats.mean = (s.latency_stats.mean * (num_tx - 1.0) / num_tx) +
            (latency * (1.0) / num_tx);

        /* start computing tps stats after a few transactions have been accumulated. 
         * Note that TPS will ramp up during the period that simulated nodes
         * first come on-line.  It would be best to wait until this period
         * has elapsed before computing TPS stats.
         */
        if (num_tx > 10) {
            const double tps = (double)num_tx / (now_time - s.time_first_tx_s);
            if (tps < s.tps_stats.min) s.tps_stats.min = tps;
            if (tps > s.tps_stats.max) s.tps_stats.max = tps;

            /* for average we will weight by number of transactions */
            s.tps_stats.mean = (s.tps_stats.mean * (num_tx - 1.0) / num_tx) +
                (tps * (1.0) / num_tx);
        }


    });
}

void iotnodesim::version()
{
    /* Important: this guarantees uniqueness of each build of the contract. */
    PRINT("EOSIOT IoT node simulation contract build " __DATE__ " " __TIME__ "\n");

};


