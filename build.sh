#!/bin/bash
#########################################################################################
#
#  Build script for the ProtoCentral TLA20xx Arduino library.
#  Compiles and uploads any example to any board via arduino-cli.
#
#  Copyright (c) 2022 ProtoCentral Electronics
#  Licensed under the MIT License (http://opensource.org/licenses/MIT)
#
#  Usage:
#    ./build.sh                          # compile every example (Uno)
#    ./build.sh compile -e NAME          # compile a single example
#    ./build.sh compile -b FQBN          # compile every example for another board
#    ./build.sh upload -e NAME           # compile + upload one example, then monitor
#    ./build.sh upload -e NAME -b FQBN   # ...to a specific board
#    ./build.sh monitor                  # serial monitor only
#    ./build.sh list                     # list available examples
#
#########################################################################################

set -u

LIBRARY_PATH="$(cd "$(dirname "$0")" && pwd)"
BOARD_FQBN="arduino:avr:uno"
BAUD_RATE="115200"
BUILD_ROOT="/tmp/tla20xx_build"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_info()    { echo -e "${BLUE}[INFO]${NC} $1"; }
print_success() { echo -e "${GREEN}[OK]${NC} $1"; }
print_warning() { echo -e "${YELLOW}[WARN]${NC} $1"; }
print_error()   { echo -e "${RED}[ERROR]${NC} $1"; }

show_help() {
    cat <<EOF
ProtoCentral TLA20xx build script (default board: Arduino Uno)

Usage: $0 [COMMAND] [OPTIONS]

Commands:
  compile     Compile examples (all by default, or one with -e)
  upload      Compile + upload a single example (requires -e or default)
  monitor     Open serial monitor
  list        List available examples

Options:
  -e, --example NAME   Example folder name (default for upload: 01.SingleEnded_Read)
  -p, --port PORT      Serial port (auto-detected if not specified)
  -b, --fqbn FQBN      Board FQBN (default: $BOARD_FQBN)
  -r, --baud RATE      Serial monitor baud rate (default: $BAUD_RATE)
  -h, --help           Show this help

Examples:
  $0                                # compile every example
  $0 compile -e 01.SingleEnded_Read
  $0 upload  -e 01.SingleEnded_Read
  $0 upload  -e 06.OpenView_Streaming -b arduino:renesas_uno:minima -r 57600
  $0 monitor -p /dev/cu.usbmodem14101
EOF
}

check_arduino_cli() {
    if ! command -v arduino-cli &>/dev/null; then
        print_error "arduino-cli not found"
        echo "Install from https://arduino.github.io/arduino-cli/"
        exit 1
    fi
}

install_core() {
    # Derive the platform (vendor:arch) from the board FQBN (vendor:arch:board).
    local core_name="${BOARD_FQBN%:*}"
    if ! arduino-cli core list 2>/dev/null | grep -q "$core_name"; then
        print_info "Installing $core_name core..."
        arduino-cli core update-index
        arduino-cli core install "$core_name"
    fi
    print_success "$core_name core ready"
}

list_examples() {
    echo "Available examples:"
    for dir in "$LIBRARY_PATH/examples"/*/; do
        [[ -d "$dir" ]] || continue
        echo "  - $(basename "$dir")"
    done
}

compile_one() {
    local name="$1"
    local sketch="$LIBRARY_PATH/examples/$name"

    if [[ ! -d "$sketch" ]]; then
        print_error "Example not found: $name"
        list_examples
        return 1
    fi

    local build_dir="$BUILD_ROOT/$name"
    mkdir -p "$build_dir"

    print_info "Compiling $name..."

    if arduino-cli compile \
        --fqbn "$BOARD_FQBN" \
        --library "$LIBRARY_PATH" \
        --build-path "$build_dir" \
        --warnings default \
        "$sketch"; then
        print_success "$name compiled"
        return 0
    else
        print_error "$name failed to compile"
        return 1
    fi
}

compile_all() {
    local total=0 passed=0 failed=0 failed_names=()

    for dir in "$LIBRARY_PATH/examples"/*/; do
        [[ -d "$dir" ]] || continue
        local name; name=$(basename "$dir")
        total=$((total + 1))
        if compile_one "$name"; then
            passed=$((passed + 1))
        else
            failed=$((failed + 1))
            failed_names+=("$name")
        fi
    done

    echo ""
    print_info "Build summary: $passed/$total passed, $failed failed"
    if (( failed > 0 )); then
        for n in "${failed_names[@]}"; do
            print_error "  $n"
        done
        exit 1
    fi
}

detect_port() {
    if [[ -n "${SERIAL_PORT:-}" ]]; then return; fi

    print_info "Detecting serial port..."
    if [[ "$OSTYPE" == "darwin"* ]]; then
        SERIAL_PORT=$(ls /dev/cu.usbmodem* /dev/cu.usbserial* 2>/dev/null | head -1)
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        SERIAL_PORT=$(ls /dev/ttyACM* /dev/ttyUSB* 2>/dev/null | head -1)
    fi

    if [[ -z "${SERIAL_PORT:-}" ]]; then
        print_error "No serial port detected. Connect the board or pass -p PORT."
        exit 1
    fi
    print_success "Using port: $SERIAL_PORT"
}

upload_one() {
    local name="${EXAMPLE_NAME:-01.SingleEnded_Read}"
    compile_one "$name" || exit 1
    detect_port

    print_info "Uploading $name to $SERIAL_PORT..."
    arduino-cli upload \
        --fqbn "$BOARD_FQBN" \
        --port "$SERIAL_PORT" \
        --input-dir "$BUILD_ROOT/$name"

    if [[ $? -eq 0 ]]; then
        print_success "Upload successful"
        sleep 1
        print_info "Starting serial monitor (Ctrl+C to exit)..."
        arduino-cli monitor --port "$SERIAL_PORT" --config baudrate=$BAUD_RATE
    else
        print_error "Upload failed"
        exit 1
    fi
}

monitor_only() {
    detect_port
    print_info "Serial monitor on $SERIAL_PORT @ $BAUD_RATE baud..."
    arduino-cli monitor --port "$SERIAL_PORT" --config baudrate=$BAUD_RATE
}

# -------- argument parsing --------
COMMAND=""
EXAMPLE_NAME=""
SERIAL_PORT=""

while [[ $# -gt 0 ]]; do
    case $1 in
        compile|upload|monitor|list)
            COMMAND="$1"; shift ;;
        -e|--example)
            EXAMPLE_NAME="$2"; shift 2 ;;
        -p|--port)
            SERIAL_PORT="$2"; shift 2 ;;
        -b|--fqbn)
            BOARD_FQBN="$2"; shift 2 ;;
        -r|--baud)
            BAUD_RATE="$2"; shift 2 ;;
        -h|--help)
            show_help; exit 0 ;;
        *)
            print_error "Unknown argument: $1"
            show_help; exit 1 ;;
    esac
done

# Default action when no command given: compile every example.
COMMAND="${COMMAND:-compile}"

check_arduino_cli

case "$COMMAND" in
    compile)
        install_core
        if [[ -n "$EXAMPLE_NAME" ]]; then
            compile_one "$EXAMPLE_NAME" || exit 1
        else
            compile_all
        fi
        ;;
    upload)
        install_core
        upload_one
        ;;
    monitor)
        monitor_only
        ;;
    list)
        list_examples
        ;;
esac
