import streamlit as st
import subprocess
import os
import tempfile
from PIL import Image

st.title("Image Compression and Decompression using Huffman Coding")

uploaded_file = st.file_uploader("Upload an image (PNG or JPG) or .huff file", type=["png", "jpg", "jpeg", "huff"])
action = st.radio("Select Action", ("Compress", "Decompress"))

if uploaded_file is not None:
    if action == "Compress" and not uploaded_file.type.startswith("image/"):
        st.warning("Please upload a valid PNG or JPG image for compression.")
    elif action == "Decompress" and not uploaded_file.name.endswith(".huff"):
        st.warning("Please upload a valid .huff file for decompression.")
    else:
        with tempfile.TemporaryDirectory() as tmpdir:
            input_path = os.path.join(tmpdir, uploaded_file.name)
            with open(input_path, "wb") as f:
                f.write(uploaded_file.read())

            if action == "Compress":
                # Convert to PNG if JPG is uploaded
                if uploaded_file.type in ["image/jpeg", "image/jpg"]:
                    img = Image.open(input_path).convert("RGB")
                    input_path = os.path.join(tmpdir, "converted_image.png")
                    img.save(input_path)

                bin_path = os.path.join(tmpdir, "output_pixels.bin")
                huff_path = os.path.join(tmpdir, "compressed_output.huff")

                # Step 1: Extract Pixels
                result = subprocess.run(["python3", "extract_pixels.py", input_path, bin_path])
                if result.returncode != 0:
                    st.error("Pixel extraction failed.")
                    st.stop()

                # Step 2: Huffman Encoding
                result = subprocess.run(["./huffman", bin_path, huff_path])
                if result.returncode != 0:
                    st.error("Huffman compression failed.")
                    st.stop()

                st.success("Compression complete!")
                with open(huff_path, "rb") as f:
                    st.download_button("Download Compressed File", f, file_name="compressed_output.huff")

            elif action == "Decompress":
                output_bin = os.path.join(tmpdir, "decompressed_output.bin")
                output_img = os.path.join(tmpdir, "restored_image.png")

                # Step 1: Decompress Huffman
                result = subprocess.run(["./decompressor", input_path, output_bin])
                if result.returncode != 0:
                    st.error("Decompression failed.")
                    st.stop()

                # Step 2: Create Image
                result = subprocess.run(["python3", "create_image.py", output_bin, output_img])
                if result.returncode != 0:
                    st.error("Image creation failed.")
                    st.stop()

                st.success("Decompression complete!")
                st.image(output_img, caption="Restored Image")
                with open(output_img, "rb") as f:
                    st.download_button("Download Restored PNG", f, file_name="restored_image.png")
