# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

pocketGem is a GTK-based application written in C for the Next Thing pocketCHIP, designed to interface with Google's Gemini AI. The application uses GTK+3 and XFCE libraries to run on the pocketCHIP's 480x272 touchscreen.

## Build Commands

```bash
# Build the application
make

# Build and run
make run

# Clean build artifacts
make clean

# Install to /usr/local/bin (requires sudo on CHIP)
sudo make install
```

## Project Structure

```
pocketGem/
├── src/           # C source files
│   └── main.c     # Main application entry point with GTK initialization
├── tests/         # Test files
├── assets/        # UI resources, icons, images
├── docs/          # Documentation
├── build/         # Build artifacts (generated)
└── Makefile       # Build configuration
```

## Architecture

- **GTK+3**: UI framework for building the graphical interface
- **GtkApplication**: Application framework handling lifecycle and window management
- **Target Platform**: Next Thing pocketCHIP (480x272 display, Debian-based OS)

## Development Requirements

On the pocketCHIP or development machine, install:
```bash
sudo apt-get install build-essential libgtk-3-dev pkg-config
```

## Key Implementation Notes

- Window size is set to 480x272 to match the pocketCHIP screen dimensions
- Application ID: `com.pocketchip.pocketgem`
- The application follows GTK best practices with proper signal handling and resource cleanup
