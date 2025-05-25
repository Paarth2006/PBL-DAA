from PIL import Image
import numpy as np
import sys

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python extract_pixels.py <input_image> <output_bin>")
        sys.exit(1)
    image_path = sys.argv[1]
    output_bin = sys.argv[2]
    image = Image.open(image_path).convert('RGB')
    arr = np.array(image)
    arr.astype('uint8').tofile(output_bin)