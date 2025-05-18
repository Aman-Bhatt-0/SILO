#!/bin/bash

set -e  # Exit on any error

# Variables
ROOTFS_URL="https://dl-cdn.alpinelinux.org/alpine/latest-stable/releases/x86_64/alpine-minirootfs-3.21.3-x86_64.tar.gz"
ROOTFS_ARCHIVE="alpine-minirootfs-3.21.3-x86_64.tar.gz"
ROOTFS_DIR="tmp-alpine-rootfs"
CONTAINER_EXEC="./mycontainer"

# Step 1: Build the container runtime
echo "[+] Building container runtime..."
make clean
make

# Step 2: Download Alpine rootfs if not already present
if [ ! -f "$ROOTFS_ARCHIVE" ]; then
    echo "[+] Downloading Alpine mini root filesystem..."
    wget "$ROOTFS_URL"
else
    echo "[*] Alpine rootfs archive already exists. Skipping download."
fi

# Step 3: Extract the root filesystem
echo "[+] Extracting Alpine rootfs to ./$ROOTFS_DIR..."
rm -rf "$ROOTFS_DIR"
mkdir "$ROOTFS_DIR"
tar -xzf "$ROOTFS_ARCHIVE" -C "$ROOTFS_DIR"

# Step 4: Run the container with /bin/sh
echo "[+] Launching container..."
sudo $CONTAINER_EXEC -m "$ROOTFS_DIR" -c /bin/sh

