import sys

import openai
from imageai.Detection import ObjectDetection
from PIL import Image, ImageDraw

def AnalysisInput(p_detected: ObjectDetection, p_filepath: str, p_miniProbability: int = 50) -> list :
    l_detections = p_detected.detectObjectsFromImage(
        input_image=p_filepath,
        output_image_path="{0}_detection.jpg".format(p_filepath.split(".")[0]),
        minimum_percentage_probability=p_miniProbability
    )
    return l_detections

if __name__ == "__main__":
    if (len(sys.argv) < 2):
        print("Please specify a file path to convert")
        sys.exit(-1)

    filename = sys.argv[1]
    filename_noext = filename.split(".")[0]

    lpe_filename = sys.argv[2]

    prompt = " ".join(sys.argv[3:])

    print("Loading {} ({})".format(filename, filename_noext))

    key_file = open("api.key", "r")
    api_key = key_file.read().strip()

    openai.api_key = api_key

    # init object detector
    l_objectDetector = ObjectDetection()
    l_objectDetector.setModelTypeAsYOLOv3()
    l_objectDetector.setModelPath("yolo.h5")
    l_objectDetector.loadModel()

    l_detections = AnalysisInput(l_objectDetector, filename)

    # for eachObject in l_detections:
    #     print("{} : {}; {}".format(eachObject["name"], eachObject["percentage_probability"], eachObject["box_points"]))

    l_image = Image.open(lpe_filename)
    l_image.putalpha(255)

    draw = ImageDraw.Draw(l_image)

    for eachObject in l_detections:
        print("{} : {}; {}".format(eachObject["name"], eachObject["percentage_probability"], eachObject["box_points"]))      

        draw.rectangle(eachObject["box_points"], fill=(0, 0, 0, 0))

        l_image.save(filename_noext + "_mask.png")

        response = openai.Image.create_edit(image=open(lpe_filename, "rb"), mask=open(filename_noext + "_mask.png", "rb"), prompt=prompt, n=5, size="512x512")
        for r in response['data']:
            print(r['url'])
        break



    
        
