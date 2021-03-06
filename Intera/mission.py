import numpy as np
import sys
from lib.vision_lib import Vision
import cv2
colors = {"blue": ((124, 157, 184), (150, 242, 255)), "red": ((0, 157, 184), (16, 242, 255)), "white": ((0, 0, 102), (255, 8, 255)), "black": ((0, 0, 0), (255, 255, 48)), "green": ((50, 102, 132), (97, 255, 255)), "yellow": ((23, 138, 164), (66, 249, 255))}
camera_1 = Vision("Bottom")
camera_2 = Vision("Front")
camera_1.get_frame()
camera_1.img = camera_1.cut_frame((0, int(camera_1.original.shape[1] / 2)), (0, camera_1.original.shape[0]))
target_center = [-1, -1]
while True:
    centers_arr = {}
    camera_1.get_frame()
    for name, value in colors.items():
        camera_1.binary(value)
        status, center, area = camera_1.search_figure_by_edge(4)
        if status is 1 and (name is "blue" or name is "red" or name is "white"):
            centers_arr[name] = center
    if list(centers_arr.keys()) is ["blue", "red", "white"] and centers_arr["blue"][1] > centers_arr["white"][1] and centers_arr["red"][0] > centers_arr["white"][0] and centers_arr["red"][0] > centers_arr["blue"][0]:
        camera_1.img = camera_1.cut_frame((int(camera_1.original.shape[1] / 2), camera_1.original.shape[1]), (0, camera_1.original.shape[0]))
        continue
    else:
        print("HERE")
        for point in list(centers_arr.values()):
            cv2.circle(camera_1.original, point, 100, (255, 0, 255), 100)
        camera_1.img = camera_1.cut_frame((0, int(camera_1.original.shape[1] / 2)), (0, camera_1.original.shape[0]))
    cv2.imshow("main", camera_1.original)
    cv2.waitKey(30)





