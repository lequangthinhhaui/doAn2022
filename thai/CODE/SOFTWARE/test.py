from time import sleep
import cv2
import numpy as np
from tkinter import *
from PIL import Image, ImageTk


import serial

#import threading
from threading import Thread
import threading
import time


#init serial
try:
  ser = serial.Serial('COM7')
except:
  print("An exception occurred")


root = Tk()

# start the QR code Detect
detector = cv2.QRCodeDetector()

# start image
root.geometry("700x700")
root.configure(bg="black")
Label(root, text="Đặng Văn Thái", font=("time new roman", 30, "bold"), bg="black", fg = "blue").pack()
Label(root, text="Đọc QR code", font=("time new roman", 30, "bold"), bg="black", fg = "red").pack()
f1 = LabelFrame(root, bg = "red")
f1.pack()
L1 = Label(f1, bg = "red")
L1.pack()


# start camera
cap = cv2.VideoCapture(2)

i = 0

def thread_function():
    while True:
        global s 
        s = ser.read(5 )
        print(s)

if __name__ == "__main__":

    t1 = threading.Thread(target=thread_function)
    t1.start()
    while True:
        img = cap.read()[1]
        data, bbox, _ = detector.detectAndDecode(img)
        if bbox is not None:
            if data:
                i = i+1
                print("[+] Du lieu QR code:", data)
                print("[+] So lan doc duoc :", i)
        img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
        img = ImageTk.PhotoImage(Image.fromarray(img))
        L1['image'] = img
        # if data == "1":
        #     ser.write(b'1')
        # else:
        #     ser.write(b'0')


        print(s)
        root.update()