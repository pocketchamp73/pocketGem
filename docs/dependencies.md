# pocketGem Dependencies

## Build Dependencies for pocketCHIP

### Basic Setup (Minimal)

```bash
sudo apt-get update
sudo apt-get install build-essential libgtk-3-dev pkg-config
```

### Comprehensive Setup (Recommended)

For a complete GTK+3 development environment with all dependencies:

```bash
sudo apt-get update
sudo apt-get install build-essential pkg-config \
  libgtk-3-dev \
  libglib2.0-dev \
  libcairo2.0-dev \
  libpango1.0-dev \
  libgdk-pixbuf2.0-dev \
  libatk1.0-dev
```

### Additional Useful Packages

```bash
# Git for version control
sudo apt-get install git

# Additional development tools
sudo apt-get install gdb valgrind
```

## pocketCHIP Repository Configuration

Since Next Thing Co. is no longer in business, if you need to add repositories, you can use the community mirror. Add these lines to `/etc/apt/sources.list`:

```
deb http://chip.jfpossibilities.com/chip/debian/repo jessie main
deb http://chip.jfpossibilities.com/chip/debian/pocketchip jessie main
```

## Platform Information

- **OS**: Debian Jessie
- **GTK Version**: GTK+3
- **Display**: 480x272 touchscreen
