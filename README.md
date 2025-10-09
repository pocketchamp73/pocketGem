# pocketGem

A GTK-based application for the Next Thing pocketCHIP that provides a native interface to Google's Gemini AI.

## Features

- Native GTK+3 interface optimized for pocketCHIP's 480x272 touchscreen
- Direct integration with Gemini API
- Scrollable conversation view
- Simple text input for questions
- Debug logging for troubleshooting

## Prerequisites

Install required dependencies on your pocketCHIP:

```bash
sudo apt-get update
sudo apt-get install build-essential libgtk-3-dev pkg-config libcurl4-openssl-dev
```

For detailed dependency information, see [docs/dependencies.md](docs/dependencies.md).

## Setup

1. Clone this repository to your pocketCHIP
2. Copy the example config file and add your API key:
   ```bash
   cp config.h.example config.h
   # Edit config.h and replace YOUR_API_KEY_HERE with your Gemini API key
   ```
3. Build the application:
   ```bash
   make build
   ```

## Usage

Run the application:
```bash
make run
```

Or install system-wide and run from anywhere:
```bash
sudo make install
pocketgem
```

## Build Commands

- `make` or `make build` - Build the application
- `make run` - Build and run the application
- `make clean` - Remove build artifacts
- `make install` - Install to /usr/local/bin

## Debug Logging

Debug logging is enabled by default and logs to `/tmp/pocketgem_debug.log`. This includes:
- API requests and responses
- HTTP status codes
- JSON parsing details
- Error messages

To disable debug logging, set `GEMINI_DEBUG` to `0` in `src/gemini.h`.

## Project Structure

```
pocketGem/
├── src/
│   ├── main.c      - GTK UI and application logic
│   ├── gemini.c    - Gemini API client implementation
│   └── gemini.h    - API configuration and headers
├── docs/
│   └── dependencies.md - Detailed dependency information
├── Makefile        - Build configuration
└── CLAUDE.md       - Developer guidance for Claude Code
```

## License

This project is provided as-is for use with the Next Thing pocketCHIP.
