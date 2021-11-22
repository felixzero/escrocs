from imutils.video import VideoStream
from threading import Thread
import time
import imutils
import cv2
import sys

# threading for real time display from here:
# https://stackoverflow.com/questions/55828451/video-streaming-from-ip-camera-in-python-using-opencv-cv2-videocapture

class VideoStreamWidget(object):
    def __init__(self, src=0):
        # Create a VideoCapture object
        self.capture = cv2.VideoCapture(src)
        # Start the thread to read frames from the video stream
        self.thread = Thread(target=self.update, args=())
        self.thread.daemon = True
        self.thread.start()
    
    def __del__(self):
        self.thread.stop()
        self.capture.release()
        cv2.destroyAllWindows()

    def update(self):
        # Read the next frame from the stream in a different thread
        while True:
            if self.capture.isOpened():
                (self.status, self.frame) = self.capture.read()
            else:
                print("No stream opened!")
                exit(1)

    def show_frame(self):
        # Display frames in main program
        if self.status:
            frame = self.frame
            # detect ArUco markers in the input frame
            (corners, ids, rejected) = cv2.aruco.detectMarkers(frame,arucoDict, parameters=arucoParams)
            
            self.frame = cv2.aruco.drawDetectedMarkers(frame,corners, ids)
            elmt_color="N/A"
            if len(corners) > 0:
                ids = ids.flatten()
                check = any(map(lambda v: v in ids, [17,13,36,47]))
                if not check:
                    print("ArUCo detected, but not sample ID.")
                else:
                    # loop over the detected ArUCo corners
                    for (markerCorner, markerID) in zip(corners, ids):
                        # extract the marker corners (which are always returned in
                        # top-left, top-right, bottom-right, and bottom-left order)
                        corners = markerCorner.reshape((4, 2))
                        (topLeft, topRight, bottomRight, bottomLeft) = corners
                        # convert each of the (x, y)-coordinate pairs to integers
                        ##topRight = (int(topRight[0]), int(topRight[1]))
                        ##bottomRight = (int(bottomRight[0]), int(bottomRight[1]))
                        ##bottomLeft = (int(bottomLeft[0]), int(bottomLeft[1]))
                        topLeft = (int(topLeft[0]), int(topLeft[1]))
                        
                        if markerID == 13:
                            elmt_color = "BLEU"
                        elif markerID == 36:
                            elmt_color = "VERT"
                        elif markerID == 47:
                            elmt_color = "ROUGE"
                        elif markerID == 17:
                            elmt_color = "unknown"
                        else:
                            elmt_color="N/A"

                        print(elmt_color)

                        if markerID in [17,13,36,47]:
                            cv2.putText(frame, elmt_color,(topLeft[0], topLeft[1] - 15), cv2.FONT_HERSHEY_SIMPLEX,0.5, (255, 0, 255), 2)
                    #endfor
            #end if
            cv2.imshow("Test ArUco", frame)
            time.sleep(1)
        #end if

        # Press Q on keyboard to stop streaming
        key = cv2.waitKey(1)
        if key == ord('q'):
            self.capture.release()
            cv2.destroyAllWindows()
            exit(1)

if __name__ == '__main__':
    try:
        ARUCO_DICT = {
            "DICT_4X4_50": cv2.aruco.DICT_4X4_50,
            "DICT_4X4_100": cv2.aruco.DICT_4X4_100,
            "DICT_4X4_250": cv2.aruco.DICT_4X4_250,
            "DICT_4X4_1000": cv2.aruco.DICT_4X4_1000,
            "DICT_5X5_50": cv2.aruco.DICT_5X5_50,
            "DICT_5X5_100": cv2.aruco.DICT_5X5_100,
            "DICT_5X5_250": cv2.aruco.DICT_5X5_250,
            "DICT_5X5_1000": cv2.aruco.DICT_5X5_1000,
            "DICT_6X6_50": cv2.aruco.DICT_6X6_50,
            "DICT_6X6_100": cv2.aruco.DICT_6X6_100,
            "DICT_6X6_250": cv2.aruco.DICT_6X6_250,
            "DICT_6X6_1000": cv2.aruco.DICT_6X6_1000,
            "DICT_7X7_50": cv2.aruco.DICT_7X7_50,
            "DICT_7X7_100": cv2.aruco.DICT_7X7_100,
            "DICT_7X7_250": cv2.aruco.DICT_7X7_250,
            "DICT_7X7_1000": cv2.aruco.DICT_7X7_1000,
            "DICT_ARUCO_ORIGINAL": cv2.aruco.DICT_ARUCO_ORIGINAL,
            "DICT_APRILTAG_16h5": cv2.aruco.DICT_APRILTAG_16h5,
            "DICT_APRILTAG_25h9": cv2.aruco.DICT_APRILTAG_25h9,
            "DICT_APRILTAG_36h10": cv2.aruco.DICT_APRILTAG_36h10,
            "DICT_APRILTAG_36h11": cv2.aruco.DICT_APRILTAG_36h11
        }

        # load the ArUCo dictionary
        arucoDict = cv2.aruco.Dictionary_get(ARUCO_DICT["DICT_4X4_250"])
        arucoParams = cv2.aruco.DetectorParameters_create()

        #vs = VideoStream(src=0).start()
        video_stream_widget = VideoStreamWidget(0)
        time.sleep(2.0)

        print("I see U!")

        while True:
            try:
                video_stream_widget.show_frame()
            except AttributeError:
                pass
            except KeyboardInterrupt:
                print('Interrupted')
                cv2.destroyAllWindows()
                sys.exit(0)

        #Standard cleanup (unused for now?)
        print("Game is done !")
        
    except KeyboardInterrupt:
        print('Interrupted')
        cv2.destroyAllWindows()
        sys.exit(0)
