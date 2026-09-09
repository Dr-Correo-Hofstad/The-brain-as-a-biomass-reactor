#!/bin/bash
# ====================================================================
# SYSTEMD DAEMON DEPLOYMENT AUTOMATION INFRASTRUCTURE
# Target Platform: AlmaLinux 9.x / CentOS Stream Enterprise Mainframes
# ====================================================================

set -e

WORKSPACE_ROOT="/workspace/Metastasis-Tracker-AI"
SERVICE_NAME="rt-biosim-pipeline.service"

echo "[*] Initializing Mainframe Systemd Daemon Deployment Sequence..."

# 1. Verify environment execution privileges
if [ "$EUID" -ne 0 ]; then
    echo "[!] CRITICAL ERROR: This deployment workflow script must be executed with root/sudo privileges."
    exit 1
fi

# 2. Sync local directory requirements
echo "[*] Syncing workspace scaffolding folders..."
mkdir -p "$WORKSPACE_ROOT/hardware"
mkdir -p "$WORKSPACE_ROOT/src/data"
mkdir -p "$WORKSPACE_ROOT/tools"

# 3. Provision systemd unit configurations to the server configuration tree
echo "[*] Copying service configuration files to system targets..."
cp tools/rt-biosim-pipeline.service /etc/systemd/system/"$SERVICE_NAME"

# 4. Refresh systemd service boundaries and enforce start hooks
echo "[*] Reloading systemd daemon configurations..."
systemctl daemon-reload

echo "[*] Activating high-availability bootstrap targets on boot..."
systemctl enable "$SERVICE_NAME"

echo "[*] Triggering immediate system service spin-up..."
systemctl restart "$SERVICE_NAME"

# 5. Check daemon health performance
sleep 2
if systemctl is-active --quiet "$SERVICE_NAME"; then
    echo "[+] SUCCESS: RT Biosimulation validation engine running smoothly in the background."
    echo "[*] To track live mainframe telemetry, execute: journalctl -u $SERVICE_NAME -f"
else
    echo "[!] DEPLOYMENT ERROR: Systemd daemon failed initialization. Check journal logs."
    exit 1
fi
