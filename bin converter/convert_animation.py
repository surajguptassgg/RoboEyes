import re
import os
import struct

# Configuration
header_file_path = "fire.h"  # Path to your header file
output_directory = "animation_output"  # Output directory for binary files
frame_width = 536
frame_height = 187

# Create output directory if it doesn't exist
os.makedirs(output_directory, exist_ok=True)

# Read the header file
with open(header_file_path, 'r') as file:
    header_content = file.read()

# Extract frame data using regular expressions
frame_data_pattern = r'const uint16_t (fire\w+) \[\] PROGMEM = \{([\s\S]*?)\};'
frames_data = re.findall(frame_data_pattern, header_content)

# Extract frame array names and content
frame_arrays = []
for frame_name, frame_content in frames_data:
    # Clean up the content and convert to integers
    values = re.findall(r'0x[0-9A-Fa-f]+', frame_content)
    pixel_values = [int(value, 16) for value in values]
    frame_arrays.append((frame_name, pixel_values))
    print(f"Found frame: {frame_name} with {len(pixel_values)} pixels")

# Verify we have the expected number of pixels for each frame
expected_pixels = frame_width * frame_height
for frame_name, pixel_values in frame_arrays:
    if len(pixel_values) != expected_pixels:
        print(f"Warning: Frame {frame_name} has {len(pixel_values)} pixels, expected {expected_pixels}")

# Find the array of frame pointers
frames_array_pattern = r'const uint16_t\* \w+\[\d+\] = \{([\s\S]*?)\}'
frames_array_match = re.search(frames_array_pattern, header_content)

# Get the order of frames
frames_order = []
if frames_array_match:
    frames_array_content = frames_array_match.group(1)
    frame_names = re.findall(r'(\w+)', frames_array_content)
    frames_order = frame_names
    print(f"Found frame order: {frames_order}")
else:
    print("Warning: Could not find frame order array. Using order as found in file.")
    frames_order = [frame_name for frame_name, _ in frame_arrays]

# Create a mapping of frame names to pixel data
frame_name_to_data = {name: data for name, data in frame_arrays}

# Save each frame as a binary file in the correct order
for i, frame_name in enumerate(frames_order):
    if frame_name in frame_name_to_data:
        # Get pixel data for this frame
        pixel_data = frame_name_to_data[frame_name]
        
        # Create binary file path
        binary_file_path = os.path.join(output_directory, f"frame_{i:02d}.bin")
        
        # Write binary data (16-bit values)
        with open(binary_file_path, 'wb') as bin_file:
            for pixel in pixel_data:
                # Write as little-endian 16-bit values
                bin_file.write(struct.pack('<H', pixel))
        
        print(f"Saved frame {i}: {frame_name} to {binary_file_path}")

# Save metadata file
frame_count = len(frames_order)
metadata_path = os.path.join(output_directory, "metadata.txt")
with open(metadata_path, 'w') as metadata_file:
    metadata_file.write(f"{frame_count},{frame_width},{frame_height}")

print(f"Saved metadata file to {metadata_path}")
print(f"Conversion complete! {frame_count} frames converted to binary files.")