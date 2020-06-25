## Smart Contract (dApp) for IoT Node Simulation Stress Test of EOSIO Blockchains

The goal of this simulation is to understand the actual performance of an EOSIO blockchain when ingesting and distributing data
from potentially thousands or millions of IoT devices, and specifically for the **Measurement Earth Global Open Data and Trusted Sensor Platform**
https://measurement.earth.  This contract is designed to work with the [IoT node simulator software](https://github.com/EOSIoT/iot-node-simulator).

This contract is loaded onto an account designated to receive the data transactions submitted from the simulated nodes.  
The contract generates statistics about two key metrics we're interested in for considering the feasibility of building a massive IoT network on a blockchain:

* transaction latency
* transaction throughput

The min, max, mean is recorded for each of the metrics specified above.

When the test is complete, the data is available by simple inspection of the [state table](https://telos.bloks.io/account/eosiotstress?loadContract=true&tab=Tables&account=eosiotstress&scope=eosiotstress&limit=100&table=statetable).

This contract has been deployed to the [Telos](https://www.telos.net/) blockchain running EOSIO software.  
The account hosting the contract is [eosiotstress](https://telos.bloks.io/account/eosiotstress)

### API

The simple API consists of these actions:

* `start()` :  prepares for the stress test
* `restart()` : reset and restart the stress test
* `submit(account_name user, string unique_id, uint32_t time, string memo)` : action called by simulated IoT nodes to submit some simulated data.  Unique_id is a required number to avoid duplicate transactions.

### Compiling

This contract has been updated to work the CDT 1.7 (latest at time of writing).

The `eosio-cpp` tool is used to generate the .abi from the .hpp and the .wasm from the .cpp.  


### Deploying

The generated artifacts must be placed into a deployment directory of the same name, e.g. `iotnodesim`.  Then deployed using your preferred method.  Using cleos:
* `cleos set contract eosiotstress /work/iot-node-simulator-contract/deploy/iotnodesim`

### Using

Setup the [node simulator](https://github.com/EOSIoT/iot-node-simulator). It is designed to work out-of-the-box with this pre-installed contract. 

Ensure the simulated nodes send their transactions to the account hosting this contract, and that the action is the `submit` action, and that the argument is the accout name of the account assigned to host the simulated IoT node.

The data stored in EOS smart contract persistent-state tables called "multi-index" tables is available for public inspection.  It is currently permissionless to access open data published on-chain to EOS.  This can be used to our advantage - as a global collective.  
To see how, it is most convenient to use a block explorer tool like [bloks.io](https://telos.bloks.io/) to inspect a chain.

For programatic access we can access this data with a simple HTTP POST as shown (this example using the Telos mainnet).

* > curl --data '{"json":true,"code":"eosiotstress","scope":"eosiotstress","table":"statetable"}' 'http://telos.eoscafeblock.com/v1/chain/get_table_rows'
* Postman: POST: http://telos.eoscafeblock.com/v1/chain/get_table_rows  Body: {"json":true,"code":"eosiotstress","scope":"eosiotstress","table":"statetable"}

On the Telos blockain, a simulation for 1000 nodes at 600 second reporting interval yeilded the following.  The results are bang-on with expectations: the tps mean tended toward the expected value of 1.7 :

```
[
  {
    host: 'eosiotstress',
    lifetime_resets: 16,
    latency_stats: {
      min: '0.00000000000000000',
      max: '36.00000000000000000',
      var: '0.00000000000000000',
      mean: '5.09214412285883089'
    },
    tps_stats: {
      min: '1.06250000000000000',
      max: '5.40000000000000036',
      var: '0.00000000000000000',
      mean: '1.77898204588052233'
    },
    num_transactions: 1693,
    time_first_tx_s: 1593053437,
    time_last_tx_s: 1593054429
  }
]
```


