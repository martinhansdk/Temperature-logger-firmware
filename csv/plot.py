from DSV import DSV
import matplotlib.pyplot as plt
from numpy import *

csvfile='2.csv'

data = file(csvfile).read()
data = DSV.organizeIntoLines(data, textQualifier=None)
data = DSV.importDSV(data, delimiter=';', textQualifier=None)
# skip the headers
data=data[1:]


def dat(no):
    return array([float(d[no]) for d in data])

(t, tin, tout, door) = (dat(0), dat(1), dat(2), dat(3))

plt.figure(1)
plt.subplot(211)
plt.plot(t, tout, 'b-', t, tin, 'r-', door, 'g-')

plt.subplot(212)
plt.plot(t, tin-tout, 'c-')
plt.show()

