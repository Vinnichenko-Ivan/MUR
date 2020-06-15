"""This library use for computer vision modules."""

import numpy as np
import cv2
import math
import pymurapi as mur

class Vision:
    def __init__(self):
        self.img = np.array()
        self.mask = np.array()
        self.box = cv2.boxPoints()
        self.colors = {}
    def binary(self, num):
        original = auv.get_image_bottom()
        self.img = np.copy(original)
        self.img = cv2.cvtColor(self.img, cv2.COLOR_BGR2HSV)
        self.mask = cv2.inRange(self.img, self.colors[num][0], self.colors[num][1])
    def __find_by_area(self, contours, edges = -1):
        max_area = 0
        max_contour = []
        for contour in contours:
            if cv2.contourArea(contour) > max_area and cv2.contourArea(max_contour) > 100:
                cv2.approxPolyDP(contour, 5, False)
                if contour.shape[0] is edges or edges is -1:
                    max_contour = contour
                    max_area = cv2.contourArea(max_contour)
        if max_contour.shape[0] > 0:
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



