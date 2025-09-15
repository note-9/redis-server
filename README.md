# Redis Clone in C++

A lightweight key-value data store implemented in C++ that mimics core Redis functionality.

## Features
- In-memory storage of string key-value pairs
- Basic commands: `SET`, `GET`, `DEL`, `EXISTS`
- Persistent TCP server that handles multiple client connections
- Simple request-response protocol with error handling

## Getting Started
```bash
g++ -std=c++17 -O2 -o redis_clone main.cpp
./redis_clone
```
Connect using telnet or nc:
``` bash
nc localhost 6379
SET key value
GET key
```
