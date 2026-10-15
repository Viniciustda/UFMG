# TP

**TP: Power grid failure monitoring**  
Course: [RC - Redes de Computadores](../README.md) (Computer Networks), 2025/1

Distributed system to monitor failures in a power grid. Sensors connect over TCP to two servers that also talk to each other: the location server (SL) stores where each sensor is, and the status server (SS) stores its risk state. The messages follow a protocol with numeric codes defined in the assignment.

## Files

- [`Documentacao.pdf`](Documentacao.pdf): Report (Portuguese).
- [`Makefile`](Makefile): Builds `server` and `sensor`.
- [`common.c`](common.c): Shared socket and message functions.
- [`common.h`](common.h): Shared definitions.
- [`sensor.c`](sensor.c): Sensor client.
- [`server.c`](server.c): Server (used for both SL and SS).

## How to run

```bash
make
./server 127.0.0.1 <peer_port> <client_port>   # start the SL and the SS, each pointing to the other
./sensor 127.0.0.1 <port_SL> <port_SS>
```

Comments in the code and reports are in Portuguese unless noted otherwise.
