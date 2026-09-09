import os
import json
import time
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Configure system monitoring paths matching your central hub layout
STATE_MATRIX_PATH = "workspace/Metastasis-Tracker-AI/src/data/state_matrix.json"

def read_latest_gpu_telemetry():
    """
    Parses the master configuration file to ingest the latest real-time 
    hardware microsecond telemetry captured by the RHI thread layers.
    """
    if not os.path.exists(STATE_MATRIX_PATH):
        # Fallback template if file is temporarily inaccessible during atomic disc writes
        return {"execution_time": 0, "budget": 1500}
        
    try:
        with open(STATE_MATRIX_PATH, 'r') as file:
            data = json.load(file)
            profile = data.get("gpu_telemetry_profile", {})
            execution_time = profile.get("last_measured_execution_time", 0)
            budget = profile.get("target_execution_budget_microseconds", 1500)
            return {"execution_time": execution_time, "budget": budget}
    except (json.JSONDecodeError, IOError):
        # File buffering safety valve
        return None

def launch_telemetry_dashboard(max_display_points=100):
    print("[*] Launching Real-Time GPU Shader Performance Monitor...")
    
    # Initialize scrolling data vectors
    time_series = list(range(max_display_points))
    telemetry_buffer = [0.0] * max_display_points
    budget_buffer = [1500.0] * max_display_points

    # Setup high-contrast visual canvas layout
    plt.style.use('dark_background')
    fig, ax = plt.subplots(figsize=(10, 5.5))
    fig.canvas.manager.set_window_title('RT Hardware Diagnostic Panel')
    
    # Instantiate plot line trackers
    gpu_line, = ax.plot(time_series, telemetry_buffer, label='GPU Deformation Shader Execution Time', color='#00ffcc', linewidth=2)
    budget_line, = ax.plot(time_series, budget_buffer, label='Target Latency Safety Budget', color='#ff3366', linewidth=1.5, linestyle='--')
    
    # Refine axes boundaries and styling cues
    ax.set_title('Real-Time Shader Execution Telemetry & Latency Profiler', fontsize=12, pad=15, color='white')
    ax.set_xlabel('Rolling Sample Count Index', fontsize=10, color='gray')
    ax.set_ylabel('Execution Duration (Microseconds)', fontsize=10, color='gray')
    ax.set_xlim(0, max_display_points - 1)
    ax.set_ylim(0, 2500) # Formatted to visualize fluctuations above the 1500 microsecond barrier
    ax.grid(True, which='both', linestyle=':', color='#333333', alpha=0.7)
    ax.legend(loc='upper right', facecolor='#111111', edgecolor='#333333')

    # Real-time text indicator overlays
    telemetry_text = ax.text(2, 2300, '', fontsize=10, weight='bold', color='#00ffcc')

    def update_plot_frame(frame):
        telemetry_data = read_latest_gpu_telemetry()
        
        if telemetry_data is not None:
            # Shift data window down to create a scrolling real-time effect
            telemetry_buffer.append(telemetry_data["execution_time"])
            budget_buffer.append(telemetry_data["budget"])
            
            telemetry_buffer.pop(0)
            budget_buffer.pop(0)
            
            # Update physical coordinates of line data strings
            gpu_line.set_ydata(telemetry_buffer)
            budget_line.set_ydata(budget_buffer)
            
            # Recalculate dynamic saturation warnings
            current_time = telemetry_data["execution_time"]
            budget_limit = telemetry_data["budget"]
            status_flag = "CRITICAL LIMIT" if current_time > budget_limit else "NOMINAL"
            
            telemetry_text.set_text(f"Latest Execution: {current_time} us | Threshold State: {status_flag}")
            if status_flag == "CRITICAL LIMIT":
                telemetry_text.set_color('#ff3366')
            else:
                telemetry_text.set_color('#00ffcc')
                
        return gpu_line, budget_line, telemetry_text

    # Loop graph updates dynamically every 50 milliseconds (20 Hz sampling sweep rate)
    ani = animation.FuncAnimation(fig, update_plot_frame, interval=50, blit=True, save_count=100)
    
    print("[+] Dashboard interface active. Streaming data channels from the RHI engine bridge...")
    plt.show()

if __name__ == "__main__":
    # Create target verification workspace paths if checking out standalone environments
    os.makedirs(os.path.dirname(STATE_MATRIX_PATH), exist_ok=True)
    launch_telemetry_dashboard()
