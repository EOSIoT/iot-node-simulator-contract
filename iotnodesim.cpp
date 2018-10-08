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

#include "iotnodesim.hpp"
#include <eosiolib/print.hpp>

using namespace eosio;

#define DEBUG 0

void iotnodesim::start()
{
    require_auth(_self);
    print("start!");

    // Check if stats data already exists
    auto itr = state.find(_self);
    eosio_assert(itr == state.end(), "stats already started");

    state.emplace(_self, [&](auto& s) {
        s.reset_state();
        s.host = _self;
    });

}

/// Restart the simulation
void iotnodesim::restart()
{
    require_auth(_self);
    print("restart!");

    // Check if stats data already exists
    auto itr = state.find(_self);
    eosio_assert(itr != state.end(), "stats not yet started");

    state.modify(itr, itr->host, [](auto& s) {
        s.reset_state();
    });
}

#if 1
/// Stop the simulation
void iotnodesim::stop()
{
    require_auth(_self);
    print("stop!");

    // Check if stats data already exists
    auto itr = state.find(_self);
    eosio_assert(itr != state.end(), "stats not yet started");

    state.erase(itr);
}
#endif

void compute_stats(
    uint32_t now_time,
    uint32_t node_time)
{

}


void iotnodesim::submit(account_name user, string unique_id, uint32_t node_time, string memo)
{ 
    /* Require that submitter has signed this action */
    /* In the interest of simplicity any account can "submit"
     * to the simulation.
     */
    require_auth(user);

    // Check if stats data already exists
    auto itr = state.find(_self);
    eosio_assert(itr != state.end(), "stats not yet started");
    eosio_assert(node_time != 0, "invalid node time");


    const uint32_t now_time = now();
#if DEBUG
    print("Data submitted by: ", name{ user }, " id: ", unique_id, " memo: ", memo, "\n");
    print(current_time(), "\n");
    print(now_time, "\n");
    print(node_time, "\n");
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
    print("EOSIOT IoT node simulation contract build 9");

};

EOSIO_ABI(iotnodesim, (start)(restart)(stop)(submit)(version))
