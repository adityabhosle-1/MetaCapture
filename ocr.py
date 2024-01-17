#!pip install pytesseract
from PIL import Image
import pytesseract

# Path to the Tesseract OCR executable (change it based on your installation)
# pytesseract.pytesseract.tesseract_cmd = '/usr/bin/tesseract'  # Commented as working in colab

def perform_ocr(image_path):
    # Open the image using Pillow
    img = Image.open(image_path)

    # Perform OCR using pytesseract with data output
    ocr_data = pytesseract.image_to_data(img, output_type=pytesseract.Output.DICT)

    # Extract bounding box coordinates and corresponding text
    bounding_boxes_with_text = []
    current_sentence = {"text": "", "bbox": None}
    for i in range(len(ocr_data['text'])):
        if int(ocr_data['conf'][i]) > 0:
            word_bbox = (int(ocr_data['left'][i]), int(ocr_data['top'][i]), int(ocr_data['left'][i] + ocr_data['width'][i]),
                         int(ocr_data['top'][i] + ocr_data['height'][i]))
            word_text = ocr_data['text'][i]

            # Check if the word is the start of a new line
            if ocr_data['line_num'][i] != current_sentence.get("line_num", -1):
                # Save the previous sentence
                if current_sentence["text"]:
                    bounding_boxes_with_text.append((current_sentence["bbox"], current_sentence["text"].strip()))

                # Start a new sentence
                current_sentence = {"text": word_text, "bbox": word_bbox, "line_num": ocr_data['line_num'][i]}
            else:
                # Continue the current sentence
                current_sentence["text"] += " " + word_text
                current_sentence["bbox"] = (min(current_sentence["bbox"][0], word_bbox[0]),
                                            min(current_sentence["bbox"][1], word_bbox[1]),
                                            max(current_sentence["bbox"][2], word_bbox[2]),
                                            max(current_sentence["bbox"][3], word_bbox[3]))

    # Save the last sentence
    if current_sentence["text"]:
        bounding_boxes_with_text.append((current_sentence["bbox"], current_sentence["text"].strip()))

    return bounding_boxes_with_text

# Example usage
image_path = 'path_to_img'
result_info = perform_ocr(image_path)

# Print the coordinates and text for each detected sentence
for bbox, text in result_info:
    print(f"Bounding Box: {bbox}, Text: {text}")
