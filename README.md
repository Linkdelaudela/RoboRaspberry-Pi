RoboRaspberry-Pi
================

C++,OpenCV and Libusb  project from my 2 year degree in Computer Science

The project aim was to build a robot controlled by a raspBerry Pi.
This code can control 2 wheels with a pololu controller.

And can stream the video from a webcam on the raspBerry pi. 


WARNING : This project has been in my school for 3 years so I haven't created the Libusb part of this program even if I use some of this functions. 

Exemple de Compilation avec opencv: 
      gcc client_webcam.c -o  clientwebcam  -Wall \`pkg-config --libs opencv\`
