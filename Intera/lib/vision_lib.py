"""This library use for computer vision modules."""

import numpy as np
import cv2
import math
import pymurapi as mur

class Vision:
    def __init__(self, view):
        if view != "Bottom" and view != "Front":
            raise ValueError("Wrong camera view.")
        self.auv = mur.mur_init()
        self.img = np.array([])
        self.mask = np.array([])
        self.original = np.array([])
        self.box = None
        self.view = view
    def get_frame(self):
        if self.view is "Front":
            self.original = self.auv.get_image_bottom()
        else:
            self.original = self.auv.get_image_front()
    def cut_frame(self, x, y):
        return self.original[y[0]:y[1], x[0]:x[1]]
    def binary(self, color):
        self.img = cv2.cvtColor(self.img, cv2.COLOR_BGR2HSV)
        self.mask = cv2.inRange(self.img, color[0], color[1])
    def __find_by_area(self, contours, edges = -1):
        max_area = 0
        max_contour = []
        for contour in contours:
            if cv2.contourArea(contour) > max_area and cv2.contourArea(contour) > 100:
                cv2.approxPolyDP(contour, 5, False)
                if contour.shape[0] is edges or edges is -1:
                    max_contour = contour
                    max_area = cv2.contourArea(max_contour)
        if len(max_contour) > 0:
            return max_contour
        else:
            return -1
    def search_figure_by_edge(self, edges):
        if self.mask.shape[0] is 0:
            return 0, -1, -1
        contours, hierarchy = cv2.findContours(self.mask, 1, 2)
        if len(contours) is 0:
            return 0, -1, -1
        max_contour = self.__find_by_area(contours, edges)
        if max_contour is -1:
            return 0, -1, -1
        rect = cv2.minAreaRect(max_contour)
        self.box = cv2.boxPoints(rect)
        self.box = np.int0(self.box)
        center = (int(rect[0][0]), int(rect[0][1]))
        area = int(rect[1][0] * rect[1][1])
        return 1, center, area



