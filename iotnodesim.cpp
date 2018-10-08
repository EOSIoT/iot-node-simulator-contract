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
void iotnodesim::submit(account_name user, string unique_id, string memo)
{ 
    /* Require that submitter has signed this action */
    /* In the interest of simplicity anyone can "submit"
     * to the simulation.
     */
    require_auth(user);

    // Check if stats data already exists
    auto itr = state.find(_self);
    eosio_assert(itr != state.end(), "stats not yet started");

    print("Data submitted by: ", name{ user }, " id: ", unique_id, " memo: ", memo);

    /* Update the current statistics */

    state.modify(itr, itr->host, [](auto& s) {
        s.num_transactions++;
    });
    
}

void iotnodesim::version()
{
    print("EOSIOT IoT node simulation contract build 7");

};

EOSIO_ABI(iotnodesim, (start)(restart)(submit)(version))

