# set port0 to uart mode
echo "other-uart" > /sys/class/lego-port/port0/mode

# set proper baud rate and mode
stty -F /dev/tty_ev3-ports:in1 ispeed 115200 ospeed 115200 cs8 raw