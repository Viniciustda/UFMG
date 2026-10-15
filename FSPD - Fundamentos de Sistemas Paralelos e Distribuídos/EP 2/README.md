# EP 2

**EP 2: RPC with portmapper and registry**  
Course: [FSPD - Fundamentos de Sistemas Paralelos e Distribuídos](../README.md) (Fundamentals of Parallel and Distributed Systems), 2025/1

Two-level name service with gRPC. On each machine, a portmapper registers services and tells on which port each one runs. A central registry records which portmapper (machine) announced each service, so a client can find any service by name.

## Files

- [`Makefile`](Makefile): Targets to generate code and run servers and clients.
- [`portmapper/`](portmapper): Portmapper server, client and `.proto` file.
- [`registry/`](registry): Registry server, client and `.proto` file.

## How to run

```bash
pip install grpcio grpcio-tools
make generate                      # generates the gRPC code from the .proto files
make run_registry arg=<port>
make run_portmapper_1 arg=<port>
make run_cli_registry arg=<server_address>
```

Comments in the code and reports are in Portuguese unless noted otherwise.
