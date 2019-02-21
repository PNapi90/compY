import numpy as np
import matplotlib.pyplot as plt

s = np.random.normal(0,10,10000)

plt.close("all")

from matplotlib import rc
rc('text', usetex=True)



data = np.loadtxt("OutputFolder/SingleG2")
data2 = np.loadtxt("OutputFolder/DoubleG2")

ens = np.zeros(len(data))
iter = 0
for i in range(len(data)):
    if data[i,4] == 2:
        ens[iter] = data[i,2]
        iter +=1  

above = 0
above2 = 0 

for i in range(len(data)):
    if data[i,0]-data[i,1] >= 100:
        above += 1

for i in range(len(data2)):
    if data2[i,0]-data2[i,1] >= 100:
        above2 += 1

print(above,len(data),above/len(data))
print(above2,len(data2),above2/len(data2))


plt.figure(1,figsize=(5,4))
plt.clf()

plt.hist(data[:,0]-data[:,1],bins=1200,range=[0,600],histtype="step",color="royalblue")
plt.hist(data2[:,0]-data2[:,1],bins=1200,range=[0,600],histtype="step",color="r")

plt.figure(2,figsize=(5,4))
plt.clf()

plt.hist(ens[:iter],bins=700//4,range=[0,700],histtype="step",color="royalblue")
plt.hist(data2[:,2],bins=700//4,range=[0,700],histtype="step",color="r")




from matplotlib.colors import LogNorm


plt.figure(3,figsize=(5,4))
plt.clf()

plt.hist2d(data[:,2],data[:,0]-data[:,1],bins=(700//4,150),range=[[0,700],[0,600]],cmap = plt.cm.jet,norm=LogNorm())

