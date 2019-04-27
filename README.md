# simple-find-human
use kinect to detect human and move to him

Firstly,detect the human body with caffemodel.

Secondly,calculate the direction of the body and convert it to an angle

Thirdly,obtain the distance between the human and kinect

Finally,send linear and angular speed to /cmd_vel(a ros topic)
