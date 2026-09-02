# HuffChat — Real-Time Message Compression using Huffman Coding

HuffChat is a chat-based application that compresses text messages in real time
using Huffman coding before "sending" them, and decompresses them back to the
exact original message on "delivery."

## Problem
Most chat systems don't compress individual text messages — each character takes
fixed space regardless of how often it repeats. HuffChat demonstrates how Huffman
coding (used in ZIP files and other real compression tools) can reduce message
size, with live compression statistics shown to the user.

## Tech Stack
- **Backend:** C++, cpp-httplib, nlohmann/json
- **Frontend:** HTML, CSS, JavaScript
- **Storage:** CSV (for message/frequency data)

## Features (Planned)
- [ ] Character frequency counting
- [ ] Huffman Tree construction (priority queue-based)
- [ ] Message encoding/decoding
- [ ] REST API (/compress, /decompress)
- [ ] Live chat-style frontend with compression stats
- [ ] Support for compressing uploaded .txt files

## Status
🚧 In development — Phase-I (Proposal & Design)

## Team — Tarnished Coders
- Vedansh Parashar (Lead) — partial Frontend, backend, documentation
- Srijan Parashar — Backend
- Saket Pravin Chandure — Database, backend
- Pramesh Singh Dhaila — Backend, testing
