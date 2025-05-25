import sys
import numpy as np
from PIL import Image
import math

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python create_image.py <input_bin> <output_png>")
        sys.exit(1)
    input_bin = sys.argv[1]
    output_png = sys.argv[2]

    with open(input_bin, 'rb') as f:
        data = f.read()

    data = np.frombuffer(data, dtype=np.uint8)

    # Try to infer dimensions
    num_pixels = data.size // 3

    def find_closest_factors(n):
        # Try to find factors as close as possible to a square
        for i in range(int(math.sqrt(n)), 0, -1):
            if n % i == 0:
                return i, n // i
        return n, 1

    height, width = find_closest_factors(num_pixels)

    if height * width * 3 != data.size:
        print(f"Cannot reshape data: {data.size} bytes is not a multiple of 3*W*H")
        sys.exit(1)

    img = data.reshape((height, width, 3))
    im = Image.fromarray(img, 'RGB')
    im.save(output_png)