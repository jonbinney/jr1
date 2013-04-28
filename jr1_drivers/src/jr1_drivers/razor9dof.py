import numpy as np
import rospy

class Razor9DOF:
    def __init__(self, ser_port, user_callback):
        self._imu_line_re = re.compile('#([AMG])-R=([.0-9-]+),([.0-9-]+),([.0-9-]+)\r\n')
        self._ser = serial.Serial(ser_port, timeout=1.0)
        rospy.loginfo('Opened serial port %s' % self._ser.portstr)
        self._user_callback = user_callback
        
    def run(self):
        # ask for raw data in text mode
        self._ser.write('#osrt')
        
        acc, mag, gyr = None, None, None
        while rospy.is_shutdown():
            line = self._ser.readline()
            m = self._imu_line_re.match(line)
            if m is None:
                rospy.logwarn('Unrecognized line from IMU: %s' % str([line]))
                continue
            
            sensor, x1, x2, x3 = m.groups()
            if sensor == 'A':
                # clear out magnetometer and gyro values just in case we're
                # out of sync
                mag, gyr = None, None
                acc = np.array(float(x1), float(x2), float(x3))
            elif sensor == 'M':
                mag = np.array(float(x1), float(x2), float(x3))
            elif sensor == 'G':
                gyr = np.array(float(x1), float(x2), float(x3))
            else:
                rospy.logwarn('Unknown sensor type %s' % sensor)

            if not None in (acc, mag, gyr):
                rospy.loginfo('Got data: %s' % str((acc, mag, gyr)))
                self._user_callback(acc, mag, gyr)
                acc, mag, gyr = None, None, None
            

