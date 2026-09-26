# hft-lab

A low-latency trading stack built from scratch in modern C++: a limit order
book and matching engine, exchange market-data (ITCH-style binary) and
order-entry (FIX-style) protocols, and the networking/concurrency plumbing
that connects them — with every stage benchmarked in nanoseconds, not
milliseconds.

This is a learning project focused on systems programming fundamentals that
general application-level C++ rarely exercises: cache-friendly data
structures, lock-free concurrency, zero-copy binary protocol parsing, and
rigorous latency measurement (percentiles, not averages).

## Why this project

Most side projects stop at "it works." This one is built around a stricter
bar: every component is measured, and every optimization is proven with a
before/after number rather than asserted. The goal is to go deep on memory
layout, the C++ memory model, kernel networking costs, and binary/text wire
protocols — the kind of low-level detail that's easy to wave hands at and
hard to actually get right.

## Architecture

```
                    FIX orders in (TCP)
   ┌─────────────┐ ───────────────────▶ ┌──────────────────┐
   │  Exchange    │                       │  Client /         │
   │  process     │ ◀─────────────────── │  Strategy process │
   │              │  FIX ExecReports      └──────────────────┘
   │  book/       │                              ▲
   │  fix/        │  ITCH market data (UDP        │
   │              │  multicast, one-way) ─────────┘
   └─────────────┘
```

- **`book/`** — the limit order book and matching engine. Two sorted sides
  (bids highest-first, asks lowest-first), price-time priority, and an
  O(1)-ish order-id → location lookup for fast cancels. No networking, no
  protocols — pure in-memory logic, fully unit tested.
- **`itch/`** *(planned)* — binary market-data protocol, modeled on Nasdaq
  TotalView-ITCH 5.0. Encodes book events (add/cancel/trade) into compact
  binary messages and decodes them back; zero-copy parsing over real
  historical ITCH sample data.
- **`fix/`** *(planned)* — a FIX 4.4 session and application-layer engine
  built from scratch (Logon/Heartbeat/ResendRequest, NewOrderSingle/
  ExecutionReport), used as the order-entry path into the matching engine.
- **`net/`** *(planned)* — UDP multicast and TCP transport, plus lock-free
  SPSC ring buffers and a seqlock for handing data between threads without
  ever blocking the matching engine's single hot thread.
- **`bench/`** *(planned)* — `rdtsc`-based nanosecond timing and HDR
  histogram percentile reporting (p50/p99/p99.9), used to measure every
  stage above.

## Design notes

- **Prices are integer ticks (`int64_t`), never floating point** — avoids
  rounding-error bugs in comparisons and allows direct use as an array index
  in future optimized book layouts.
- **Every order gets a monotonic sequence number on arrival** — gives
  deterministic FIFO time-priority within a price level without reading a
  clock (slower, and can collide).
- **A trade always executes at the resting order's price**, never the
  aggressor's — the resting order provided liquidity and arrived first, so
  the aggressor gets whatever price improvement is available.
- **`std::list` for price-level order queues, not `std::vector`** — list
  iterators remain valid across insertions/removals elsewhere in the same
  list, which is what makes an O(1) order-id → location lookup table safe to
  maintain alongside the book.

## Status

| Module | Status |
|---|---|
| `book/` | Baseline implementation complete: add, cancel, matching (full/partial fill, multi-level sweep), price-time priority. 7/7 unit tests passing. |
| `book/` — optimized variants (flat array, intrusive lists) | Planned |
| `bench/` | Planned |
| `itch/` | Planned |
| `fix/` | Planned |
| `net/` | Planned |

## Building

Requires CMake 3.16+ and a C++20 compiler.

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

GoogleTest is fetched automatically via CMake's `FetchContent` — no manual
setup required.
