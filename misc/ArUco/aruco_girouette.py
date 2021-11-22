from imutils.video import VideoStream
import time
import imutils
import cv2
import sys
import RPi.GPIO as GPIO


def main():

    while True:
        # grab a frame from the video stream
        frame = vs.read()
        # detect ArUco markers in the input frame
        (corners, ids, rejected) = cv2.aruco.detectMarkers(frame,arucoDict, parameters=arucoParams)
        
        ##cv2.aruco.drawDetectedMarkers(frame,corners, ids)
        wind="unknown"
        # For now, no way to differentiate between unknown and north
        if len(corners) > 0:
            ids = ids.flatten()
            if not 17 in ids:
                print("ArUCo detected, but not 17.")
				#TODO: check if this is really a good idea. as it is the pin keeps the detection state if a SOUTH value was detected,
				#  and is not reset unless a NORTH is seen
                ###GPIO.output(11,False)
                time.sleep(1)
                continue
            #Endif
            
            # loop over the detected ArUCo corners
            for (markerCorner, markerID) in zip(corners, ids):
                # extract the marker corners (which are always returned in
                # top-left, top-right, bottom-right, and bottom-left order)
                corners = markerCorner.reshape((4, 2))
                (topLeft, topRight, bottomRight, bottomLeft) = corners
                # convert each of the (x, y)-coordinate pairs to integers
                topRight = (int(topRight[0]), int(topRight[1]))
                bottomRight = (int(bottomRight[0]), int(bottomRight[1]))
                bottomLeft = (int(bottomLeft[0]), int(bottomLeft[1]))
                topLeft = (int(topLeft[0]), int(topLeft[1]))
                
                wind = ""
                if topLeft[0] > bottomRight[0] and topLeft[1] > bottomRight[1]:
                    wind = "SOUTH"
                    GPIO.output(11,True)
                else:
                    wind="NORTH"
                    GPIO.output(11,False)
                ##cv2.putText(frame, wind,(topLeft[0], topLeft[1] - 15), cv2.FONT_HERSHEY_SIMPLEX,0.5, (255, 0, 255), 2)
                
        
        print(wind)
        time.sleep(1)
    #end while
#endfunc



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

        vs = VideoStream(src=0).start()
        time.sleep(2.0)

        print("I see U!")

        # Setup pins for communication with arduino
        GPIO.setmode (GPIO.BOARD)
        GPIO.setup (11,GPIO.OUT)
        GPIO.output(11,False)
        # run main loop
        main()
        #Standard cleanup
        print("Game is done !")
        vs.stop()
        GPIO.output(11,False)
        GPIO.cleanup()
    #Debug cleanup
    except KeyboardInterrupt:
        print('Interrupted')
        vs.stop()
        GPIO.output(11,False)
        GPIO.cleanup()
        sys.exit(0)
