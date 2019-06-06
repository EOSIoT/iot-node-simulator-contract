# Smart Contract for IoT Node Simulation Stress Test of EOS Blockchain

This contract is loaded onto an account designated to receive the data submission
transactions from the simulated nodes.  The contract generates statistics about two
key metrics we're interested in for considering the feasibility of building a massive IoT network on a blockchain:
* transaction latency
* transaction throughput

The min, max, mean, variance is recorded for each of the metrics specified above.

When the test is complete, the data is available by simple inspection of the table.

## API

The simple API consists of these actions:

* `start()` :  prepares for the stress test
* `restart()` : reset and restart the stress test
* `submit(account_name user, string unique_id, uint32_t time)` : action called by simulated IoT nodes to submit some simulated data.  Unique_id is a required number to avoid duplicate transactions.

## Compiling

The `eosiocpp` tool is used to generate the .abi from the .hpp and the .wasm from the .cpp.  


## Deploying

The generated artifacts must be placed into a deployment directory of the same name, e.g. `iotnodesim`.  Then deployed using your preferred method.  Using cleos:
* `cleos set contract eosiotstress /work/iot-node-simulator-contract/deploy/iotnodesim`

## Using

Setup the [node simulator](https://github.com/EOSIoT/iot-node-simulator).  Ensure the simulated nodes send their transactions to the account hosting this contract, and that the action is the `submit` action, and that the argument is the accout name of the account assigned to host the simulated IoT node.

The data stored in EOS smart contract persistent-state tables called "multi-index" tables is available for public inspection.  It is currently permissionless to access open data published on-chain to EOS.  This can be used to our advantage - as a global collective.  To see how, we can access this data with a simple HTTP POST as shown (this example using the EOS Jungle Testnet).

* > curl --data '{"json":true,"code":"eosiotstress","scope":"eosiotstress","table":"statetable"}' 'http://jungle.cryptolions.io:18888/v1/chain/get_table_rows'
* Postman: POST: http://jungle.cryptolions.io:18888/v1/chain/get_table_rows  Body: {"json":true,"code":"eosiotstress","scope":"eosiotstress","table":"statetable"}

The simulation for 1000 nodes at 600 second reporting interval yeilded the following.  The results are bang-on with expectations: the tps mean tended toward the expected value of 1.7 :
```
{
  "rows": [{
      "host": "eosiotstress",
      "latency_stats": {
        "min": "1.00000000000000000",
        "max": "22.00000000000000000",
        "var": "0.00000000000000000",
        "mean": "1.84167019923696662"
      },
      "tps_stats": {
        "min": "1.38195664909197413",
        "max": "4.00000000000000000",
        "var": "0.00000000000000000",
        "mean": "1.71525493223053727"
      },
      "num_transactions": 4718,
      "time_first_tx_s": 1539364909,
      "time_last_tx_s": 1539368323
    }
  ],
  "more": false
}
```


