echo "Press Ctrl-a k to exit"
sleep 2
TTY=$1
stty -F $TTY cs8 9600 ignbrk -brkint -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts
screen $TTY 9600
