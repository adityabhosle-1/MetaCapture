#!pip install pytesseract
from PIL import Image
import pytesseract

# Path to the Tesseract OCR executable (change it based on your installation)
#pytesseract.pytesseract.tesseract_cmd = '/usr/bin/tesseract' - i have commented this out as i worked on colab

def perform_ocr(image_path):
    # Open the image using Pillow
    img = Image.open(image_path)

    # Perform OCR using pytesseract
    text = pytesseract.image_to_string(img)

    return text

# Example usage
image_path = '/content/WhatsApp Image 2024-01-16 at 19.03.35.jpeg'
result_text = perform_ocr(image_path)

# Print the extracted text
print("Extracted Text:")
print(result_text)