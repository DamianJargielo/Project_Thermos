import cv2
import numpy as np
import urllib.request
import time

URL = "" #Camera URL

def main():
    print(f"Connecting to {URL}...")
    
    cv2.namedWindow("ESP32-S3 Camera", cv2.WINDOW_AUTOSIZE)

    while True:
        try:
            img_response = urllib.request.urlopen(URL)
            
            img_np = np.array(bytearray(img_response.read()), dtype=np.uint8)
            
            frame = cv2.imdecode(img_np, -1)
            
            if frame is not None:
                cv2.imshow("ESP32-S3 Camera", frame)
            else:
                print("Failed to decode frame")

            # Q to quit
            if cv2.waitKey(1) & 0xFF == ord('q'):
                print("Exiting...")
                break
                
        except Exception as e:
            print(f"Error fetching frame: {e}")
            time.sleep(1)

    cv2.destroyAllWindows()

if __name__ == '__main__':
    main()