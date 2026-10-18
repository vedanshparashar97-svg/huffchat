
# HuffChat — Real-Time Message Compression Using Huffman Coding

HuffChat is a C++ project that demonstrates how Huffman coding can compress text into a variable-length binary representation and reconstruct the original text through decoding.

The project is being developed in phases. The current phase focuses on the Huffman compression engine, compression statistics, and CSV-based history storage. A chat-style interface and HTTP API are planned for a later phase.

## Current Features

- Character frequency counting
- Huffman tree construction using a priority queue
- Huffman code generation
- Text encoding and decoding
- Compression statistics:
  - Original size in bits
  - Encoded size in bits
  - Percentage of bits saved
  - Compression execution time
- CSV history storage for compression statistics
- Round-trip tests for normal text, repeated characters, empty input, and longer text

## Technology Stack

- **Language:** C++17
- **Core:** Huffman coding implemented in C++
- **Data storage:** CSV
- **Planned web server:** cpp-httplib
- **Planned JSON handling:** nlohmann/json
- **Planned frontend:** HTML, CSS, and JavaScript

## Project Structure

```text
huffchat/
├── EncoderDecoder.cpp
├── EncoderDecoder.h
├── frequency_map.cpp
├── frequency_map.h
├── HuffmanNode.h
├── TreeBuilder.cpp
├── TreeBuilder.h
├── HuffmanCompressor.cpp
├── HuffmanCompressor.h
├── HistoryStore.cpp
├── HistoryStore.h
├── main.cpp
├── README.md
└── .gitignore
```

## Build and Run

Compile the current core implementation from the project directory:

```bash
g++ -std=c++17 main.cpp frequency_map.cpp TreeBuilder.cpp EncoderDecoder.cpp HuffmanCompressor.cpp HistoryStore.cpp -o huffchat_test
```

Run the test/demo program:

```bash
./huffchat_test
```

The program demonstrates compression, decompression, statistics, and CSV history output.

## Compression Statistics

The original size is calculated using 8 bits per input character. The encoded size is the length of the Huffman bit string.

The reported percentage saved compares these two sizes. It does not include the code table or other metadata that would be required to transmit a complete compressed representation.

## CSV History

The history store records:

- Timestamp
- Original size in bits
- Encoded size in bits
- Percentage saved
- Compression time in milliseconds

The CSV stores compression statistics rather than the original message text. The runtime `history.csv` file is excluded from Git.

## Planned Work

- HTTP endpoints for compression, decompression, and history
- Chat-style frontend using HTML, CSS, and JavaScript
- Frontend integration with the C++ backend
- Text-file upload support
- Further validation and end-to-end testing

These features are planned and should not be considered complete until implemented and tested.

## Team — Tarnished Coders

- **Vedansh Parashar** — Project lead, compressor wrapper, statistics, CSV history, and documentation
- **Srijan Parashar** — Frontend and testing
- **Saket Pravin Chandure** — Huffman tree and algorithm backend
- **Pramesh Singh Dhaila** — Frequency map, encoding, and decoding

## Project Status

**Current phase:** Huffman compression engine and CSV history storage.

The core has compiled successfully in the development environment, and the current demo has passed four round-trip test cases. Web integration and end-to-end chat functionality remain future work.
