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
  ser = serial.Serial('COM10')
except:
  print("An exception occurred")


i = 0
global bbox
global data 

dataImage = "" 
bboxImage = "" 
_ = ""

flagVar = 0



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



def thread_function():
    while True:
        print ("Message from arduino: ")
        try:
          msg = ser.readline()
        except:
          print("An exception occurred")
        print(msg.decode())

def thread_function_2():
    while True:
      if bboxImage is not None and dataImage != "":
        try:
          ser.write(dataImage.encode())
        except:
          print("An exception occurred")        

        time.sleep(5)
        # print(dataImage)


if __name__ == "__main__":

  t1 = threading.Thread(target=thread_function)
  t1.start()
  t2 = threading.Thread(target=thread_function_2)
  t2.start()

  while True:
      img = cap.read()[1]
      dataImage, bboxImage, _ = detector.detectAndDecode(img)
      if flagVar == 0:

        flagVar = 1
      img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
      img = ImageTk.PhotoImage(Image.fromarray(img))
      L1['image'] = img
      root.update()


