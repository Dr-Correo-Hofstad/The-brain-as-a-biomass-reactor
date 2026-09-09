import math

class HexadecimalVideoCodec:
    def __init__(self):
        # Establishes the 16 available analog voltage step targets (0x0 to 0xF)
        self.hex_voltage_steps = [format(i, 'X') for i in range(16)]
        
    def compress_pixel_to_hex_token(self, luminance_value):
        """
        Maps a standard 8-bit image grayscale byte value (0-255)
        directly into a compressed 4-bit hexadecimal analog step register.
        """
        # Clamp bounds
        clamped = max(0, min(255, luminance_value))
        # Quantize step distribution down to 16 intervals
        quantized_step = math.floor(clamped / 16.0)
        return self.hex_voltage_steps[quantized_step]

    def encode_image_stream_to_matrix(self, mock_pixel_grid):
        print(f"[*] Initializing Tensor Compression Pipeline via Hex Gating Codec...")
        compressed_stream = []
        
        for row_idx, row in enumerate(mock_pixel_grid):
            row_tokens = []
            for col_idx, pixel in enumerate(row):
                token = self.compress_pixel_to_hex_token(pixel)
                row_tokens.append(token)
            compressed_stream.append("".join(row_tokens))
            
        print(f"[+] Spatial Frame Compression Complete. Reduced bit depth from 8-bit to 4-bit.")
        return compressed_stream

if __name__ == "__main__":
    codec = HexadecimalVideoCodec()
    
    # 4x4 mock video pixel segment matrix generated from visual input capture simulation
    mock_sensor_input = [,
 ,
 ,
        [175, 200, 230, 255]
    ]
    
    hex_output = codec.encode_image_stream_to_matrix(mock_sensor_input)
    print("--- Compressed Output Hexadecimal Stream ---")
    for index, encoded_line in enumerate(hex_output):
        print(f"Line Segment Register [{index}]: 0x{encoded_line}")
