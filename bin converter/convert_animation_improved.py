import re
import os
import struct
import sys

# Configuration
header_file_path = "trip.h"  # Path to your header file
output_directory = "animation_output"  # Output directory for binary files
frame_width = 536
frame_height = 240

# Create output directory if it doesn't exist
os.makedirs(output_directory, exist_ok=True)

print(f"Reading header file: {header_file_path}")
try:
    # Read the header file
    with open(header_file_path, 'r') as file:
        header_content = file.read()
    print(f"Successfully read header file: {len(header_content)} characters")
except Exception as e:
    print(f"Error reading header file: {e}")
    sys.exit(1)

# Extract frame data using regular expressions
print("Extracting frame data...")
frame_data_pattern = r'const uint16_t (\w+) \[\] PROGMEM = \{([\s\S]*?)\};'
frames_data = re.findall(frame_data_pattern, header_content)
print(f"Found {len(frames_data)} frame data sections")

# Extract frame array names and content
frame_arrays = []
for frame_name, frame_content in frames_data:
    # Clean up the content and convert to integers
    print(f"Processing frame: {frame_name}")
    values = re.findall(r'0x[0-9A-Fa-f]+', frame_content)
    print(f"  Found {len(values)} pixel values")
    
    if len(values) == 0:
        print(f"  WARNING: No pixel values found for {frame_name}")
        continue
        
    try:
        pixel_values = [int(value, 16) for value in values]
        frame_arrays.append((frame_name, pixel_values))
    except ValueError as e:
        print(f"  ERROR: Failed to convert hex values: {e}")
        continue

print(f"Successfully processed {len(frame_arrays)} frames")

# Verify we have the expected number of pixels for each frame
expected_pixels = frame_width * frame_height
for frame_name, pixel_values in frame_arrays:
    if len(pixel_values) != expected_pixels:
        print(f"Warning: Frame {frame_name} has {len(pixel_values)} pixels, expected {expected_pixels}")

# Find the array of frame pointers
print("Finding frame pointer array...")
frames_array_pattern = r'const uint16_t\* \w+\[\d+\] = \{([\s\S]*?)\}'
frames_array_match = re.search(frames_array_pattern, header_content)

# Get the order of frames
frames_order = []
if frames_array_match:
    frames_array_content = frames_array_match.group(1)
    frame_names = re.findall(r'(\w+)', frames_array_content)
    frames_order = frame_names
    print(f"Found frame order with {len(frames_order)} entries")
else:
    print("Warning: Could not find frame order array. Using order as found in file.")
    frames_order = [frame_name for frame_name, _ in frame_arrays]
    print(f"Using default order with {len(frames_order)} entries")

# Create a mapping of frame names to pixel data
frame_name_to_data = {name: data for name, data in frame_arrays}

# Save each frame as a binary file in the correct order
print("\nSaving binary files...")
binary_files_created = 0

for i, frame_name in enumerate(frames_order):
    print(f"Processing frame {i}: {frame_name}")
    
    if frame_name not in frame_name_to_data:
        print(f"  ERROR: Frame name '{frame_name}' not found in frame data dictionary!")
        print(f"  Available frames: {list(frame_name_to_data.keys())}")
        continue
    
    # Get pixel data for this frame
    pixel_data = frame_name_to_data[frame_name]
    
    if not pixel_data:
        print(f"  ERROR: No pixel data for frame {frame_name}")
        continue
    
    # Create binary file path
    binary_file_path = os.path.join(output_directory, f"frame_{i:02d}.bin")
    print(f"  Writing to: {binary_file_path}")
    
    try:
        # Write binary data (16-bit values)
        with open(binary_file_path, 'wb') as bin_file:
            bytes_written = 0
            for pixel in pixel_data:
                bin_file.write(struct.pack('<H', pixel))
                bytes_written += 2
            
            print(f"  Success! Wrote {bytes_written} bytes ({len(pixel_data)} pixels)")
            binary_files_created += 1
    except Exception as e:
        print(f"  ERROR writing binary file: {e}")

print(f"\nBinary files created: {binary_files_created}")

# Save metadata file
frame_count = len(frames_order)
metadata_path = os.path.join(output_directory, "metadata.txt")

try:
    with open(metadata_path, 'w') as metadata_file:
        metadata_file.write(f"{frame_count},{frame_width},{frame_height}")
    print(f"Saved metadata file to {metadata_path}")
except Exception as e:
    print(f"ERROR writing metadata file: {e}")

print(f"Conversion complete! {frame_count} frames processed, {binary_files_created} binary files created.")