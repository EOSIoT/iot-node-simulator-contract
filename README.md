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
* `submit(account_name)` : action called by simulated IoT nodes to submit some simulated data

## Compiling

The `eosiocpp` tool is used to generate the .abi from the .hpp and the .wasm from the .cpp.  


## Deploying

The generated artifacts must be placed into a deployment directory of the same name, e.g. `iotnodesim`.  Then deployed using your preferred method.  Using cleos:
* `cleos set contract eosiotstress /work/iot-node-simulator-contract/deploy/iotnodesim`

## Using

Setup the [node simulator](https://github.com/EOSIoT/iot-node-simulator).  Ensure the simulated nodes send their transactions to the account hosting this contract, and that the action is the `submit` action, and that the argument is the accout name of the account assigned to host the simulated IoT node.
