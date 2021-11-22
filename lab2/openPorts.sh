echo "Openning ports /dev/ttyS10 and /dev/ttyS11"

sudo socat -d -d PTY,link=/dev/ttyS10,mode=777 PTY,link=/dev/ttyS11,mode=777
