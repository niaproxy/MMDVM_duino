# MMDVM_duino
Use arduino based bords as MMDVM hotspot over network.

socat pty,link=/dev/ttyV0,raw tcp:192.168.0.155:7777

diff --git a/SerialController.cpp b/SerialController.cpp
index 43e9e2a..8336149 100644
--- a/SerialController.cpp
+++ b/SerialController.cpp
@@ -331,16 +331,16 @@ bool CSerialController::open()
                        unsigned int y;
                        if (::ioctl(m_fd, TIOCMGET, &y) < 0) {
                                LogError("Cannot get the control attributes for %s", m_device.c_str());
-                               ::close(m_fd);
-                               return false;
+                               //::close(m_fd);
+                               //return false;
                        }
 
                        y |= TIOCM_RTS;
 
                        if (::ioctl(m_fd, TIOCMSET, &y) < 0) {
                                LogError("Cannot set the control attributes for %s", m_device.c_str());
-                               ::close(m_fd);
-                               return false;
+                               //::close(m_fd);
+                               //return false;
                        }
                }
