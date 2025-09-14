<div align="center">

![Wally Icon](data/icons/scalable/apps/com.qomarhsn.wally.svg)

# Wally - GNOME Wallpaper Slideshow Manager

A modern GNOME desktop application for managing wallpaper slideshows with automatic day/night theme switching.

</div>

## Features

- **Dual Theme Support** - Separate wallpaper collections for light and dark themes
- **Custom Intervals** - Set slideshow timing from 1 minute to 24 hours
- **Auto Theme Switching** - Automatically switches wallpapers based on system theme
- **Smooth Transitions** - Configurable fade effects between wallpapers
- **Clean Interface** - Simple single-page settings window

## Installation

### Build from Source

1. **Install Dependencies**:

   **Ubuntu/Debian:**
   ```bash
   sudo apt install build-essential meson ninja-build pkg-config libgtk-4-dev libadwaita-1-dev
   ```

   **Fedora:**
   ```bash
   sudo dnf install gcc-c++ meson ninja-build pkg-config gtk4-devel libadwaita-devel
   ```

   **Arch Linux:**
   ```bash
   sudo pacman -S base-devel meson ninja pkg-config gtk4 libadwaita
   ```

2. **Build and Install**:
   ```bash
   git clone https://github.com/qomarhsn/wally.git
   cd wally
   meson setup builddir
   meson compile -C builddir
   sudo meson install -C builddir
   sudo glib-compile-schemas /usr/local/share/glib-2.0/schemas/
   ```

## Usage

1. Launch Wally from applications menu or run `wally`
2. Select day and night wallpaper folders
3. Set interval and transition duration
4. Click "Apply" to start slideshow

## Requirements

- GNOME 44+
- GTK4 (>= 4.10)
- libadwaita (>= 1.4)

## License

Mozilla Public License 2.0 - see [LICENSE](LICENSE)
