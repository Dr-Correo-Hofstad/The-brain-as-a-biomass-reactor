# ====================================================================
# AUTOMATION TARGETS: RT Unified Validation and Performance Pipeline
# ====================================================================

# Core variables and system relative paths
PYTHON        = python3
WORKSPACE_DIR = workspace/Metastasis-Tracker-AI
SRC_DIR       = $(WORKSPACE_DIR)/src
DATA_DIR      = $(SRC_DIR)/data

.PHONY: stress-test-loop check-env clean-matrix

# Verifies that local filesystem structures exist before executing loops
check-env:
	@mkdir -p $(DATA_DIR)
	@if [ ! -f $(DATA_DIR)/state_matrix.json ]; then \
		echo "[*] State matrix absent. Pre-compiling baseline architecture layout..."; \
		$(PYTHON) $(SRC_DIR)/integration/compile_system.py; \
	fi

# Master automated stress pipeline orchestrator target
stress-test-loop: check-env
	@echo "===================================================================="
	@echo "[*] LAUNCHING UNIFIED HARDWARE STRESS & COMPILER RECOVERY LOOP"
	@echo "===================================================================="
	@echo "[*] Spawning live telemetry monitoring timeline dashboard..."
	@$(PYTHON) $(SRC_DIR)/monitor_gpu_telemetry.py & \
	MONITOR_PID=$$!; \
	\
	echo "[*] Commencing continuous automated scaling evaluation background processes..."; \
	(for i in {1..35}; do \
		$(PYTHON) $(SRC_DIR)/integration/compile_system.py; \
		sleep 0.8; \
	done) & \
	COMPILER_PID=$$!; \
	\
	echo "[*] Igniting main structural load stress spike injector sequence..."; \
	$(PYTHON) $(SRC_DIR)/generate_stress_spikes.py 35; \
	\
	echo "[*] Stress timeline finalized. Terminating live orchestration threads..."; \
	kill $$COMPILER_PID 2>/dev/null || true; \
	sleep 2; \
	kill $$MONITOR_PID 2>/dev/null || true; \
	echo "[+] UNIFIED STRESS RUN TERMINATED. Evaluation metrics compiled."

clean-matrix:
	@rm -f $(DATA_DIR)/state_matrix.json
	@echo "[+] Stale data matrices flushed cleanly from the workspace."

# Append these lines to your existing central Makefile workspace interface

.PHONY: test-chart-layout deploy-daemon

# Automated markdown layout compliance validation gate
test-chart-layout:
	@$(PYTHON) tools/test_chart_layout.py

# Mainframe service installation sequence trigger hook
deploy-daemon:
	@echo "[*] Preparing environment for automated daemon scaling injection..."
	@sudo bash run_pipeline.sh
