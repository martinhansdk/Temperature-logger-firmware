import serial, re
ser=serial.Serial('/dev/ttyUSB0', 9600, timeout=10)
ser.flushInput()
ser.flushOutput()
ser.write("getdata\r\n")
ser.flush()
mo=None
for x in range(10):
    recordline=ser.readline()

    mo=re.match(r'records=(\d+)', recordline)
    if mo:
        out=open('logger.csv', 'w')
        records=int(mo.group(1))
        print "getting %d records" % records
        for i in range(records):
            out.write(ser.readline())
        out.close()
        print "wrote logger.csv"
        sys.exit(0)
    else:
        print "ignoring", recordline
    ser.close()
    
print "ERROR, no records header found"
