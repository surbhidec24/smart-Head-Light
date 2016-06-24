import matplotlib.pyplot as plt 
import numpy as np 
import csv

def getColumn(filename, column):
    results = csv.reader(open(filename), delimiter="\t")
    return [result[column] for result in results]

filename = '1.csv'
time = getColumn(filename,0)
dx = getColumn(filename,1)
dy = getColumn(filename,2)

plt.figure()
plt.xlabel("Time(ms)")
plt.ylabel("Velocity")
plt.plot(time,dx)
plt.plot(time,dy,"r--")
plt.show()