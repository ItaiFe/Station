#!/usr/bin/env python3
"""Fake Flamingo: print Station UDP packets.

Usage: python3 tools/listen.py [port]   (default 5000)
Build the station with -DFLAMINGO_HOST=\\"<this-machine>.local\\" to point it here.
"""
import socket
import sys
import time

NAMES = ["red", "green", "blue", "yellow", "white"]


def decode(packet: bytes) -> str:
    r"""
    >>> decode(bytes([1, 0x05]))
    'station 1  0x05  red+blue'
    >>> decode(bytes([3, 0x1F]))
    'station 3  0x1F  red+green+blue+yellow+white  (ALL -> special mode)'
    >>> decode(bytes([2, 0]))
    'station 2  0x00  released'
    >>> decode(b'\x01')
    'malformed packet: 01'
    """
    if len(packet) != 2:
        return f"malformed packet: {packet.hex()}"
    station, mask = packet
    if mask == 0:
        return f"station {station}  0x00  released"
    names = "+".join(n for i, n in enumerate(NAMES) if mask & (1 << i))
    suffix = "  (ALL -> special mode)" if mask == 0x1F else ""
    return f"station {station}  0x{mask:02X}  {names}{suffix}"


def main() -> None:
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 5000
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", port))
    sock.settimeout(0.2)
    print(f"Listening for stations on UDP {port} (Ctrl+C to stop)")
    last = {}
    counts = {}
    window_start = time.monotonic()
    while True:
        try:
            data, addr = sock.recvfrom(64)
            key = data[0] if data else None
            counts[key] = counts.get(key, 0) + 1
            if last.get(key) != data:
                last[key] = data
                print(f"{time.strftime('%H:%M:%S')}  {addr[0]:<15}  {decode(data)}")
        except socket.timeout:
            pass
        if time.monotonic() - window_start >= 1.0:
            for key, n in counts.items():
                print(f"          station {key}: {n} pkt/s")
            counts.clear()
            window_start = time.monotonic()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
