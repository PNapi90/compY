import numpy as np
import matplotlib.pyplot as plt

import math

plt.close("all")

#from mpl_toolkits.mplot3d import Axes3D  # noqa: F401 unused import


from matplotlib import rc
rc('text', usetex=True)




data = np.loadtxt("TmpFile_S")

fig,ax = plt.subplots(3,2,figsize=(8,8))


plottedPoints = 0
nextG = True

x = np.zeros((2,3))

r = np.zeros(2)

MAX_LEN = 4000#len(data)

R_Arr = np.zeros((MAX_LEN,3))
EArr = np.zeros(MAX_LEN)
TH_PH = np.zeros((MAX_LEN*2,6))

TH_PH_Paths = np.zeros((MAX_LEN*100,3))
th_ph_c = 0
pathPoints = 0

EnergyBool = False

delta = 400 - 661.7/2


tmpDist = 0
tmpAir = 0

#phi[deg]*sin(th) vs th(deg)

fig2,ax2 = plt.subplots(1,figsize=(5,4))

def GetV(s):
    v = np.zeros(2,dtype=int)
    t = 0
    for i in range(5):
        for j in range(4):
            if t == s:
                v[0] = i
                v[1] = j
                return v
            t += 1
    print(s , "not known!")
    return v



#fig2 = plt.figure(1000,figsize=(5,4))
#axelF = fig2.add_subplot(111, projection='3d')


AngleArrays = np.zeros((20,1000000,2))
AngleIters =  np.zeros(20,dtype=int)

tmpAngleArray = np.zeros((50,2))



distranges = [600/20*i for i in range(20)]

figG,axGiant  = plt.subplots(5,4,figsize=(25,16))
s = 0
for i in range(5):
    for j in range(4):
        axGiant[i,j].text(-100,45,"$d \\to $  " + str(int(distranges[s])))
        axGiant[i,j].set_xlim(-200,200)
        axGiant[i,j].set_ylim(100,300)

        s += 1
s = 0

for i in range(len(data)):

    if plottedPoints >= MAX_LEN:
        break


    if data[i,0] < 0:
        nextG = True
        if EnergyBool or True:
            if plottedPoints < 1000:
                ax[0,0].plot(x[:,0],x[:,1],color="dodgerblue",ls="--",lw=0.1)
                ax[0,0].plot(x[:,0],x[:,1],color="crimson",ls="None",marker="o",ms=1.5)
                ax[1,0].plot(x[:,0],x[:,2],color="dodgerblue",ls="--",lw=0.1)
                ax[1,0].plot(x[:,0],x[:,2],color="crimson",ls="None",marker="o",ms=1.5)
                ax[0,1].plot(x[:,2],x[:,1],color="dodgerblue",ls="--",lw=0.1)
                ax[0,1].plot(x[:,2],x[:,1],color="crimson",ls="None",marker="o",ms=1.5)

            
            thetas = np.array([math.acos(x[i,2]/R_Arr[plottedPoints,i])*180/np.pi for i in range(2)])
            phis = np.array([math.atan2(x[i,1],x[i,0])*180/np.pi for i in range(2)])

            if tmpAir > 0:

                for k in range(2):
                    TH_PH[th_ph_c,0] = thetas[k]
                    TH_PH[th_ph_c,1] = phis[k]
                    TH_PH[th_ph_c,2] = R_Arr[plottedPoints,k]
                    TH_PH[th_ph_c,3] = tmpDist
                    TH_PH[th_ph_c,4] = tmpAir
                    TH_PH[th_ph_c,5] = anlgeTmp

                    diff = tmpDist# - tmpAir
                    for s in range(19):
                        if diff >= distranges[s] and diff < distranges[s+1]:
                            for l in range(50):
                                AngleArrays[s,AngleIters[s],0] = np.sin(tmpAngleArray[l,0])*np.cos(tmpAngleArray[l,1])#np.sin(thetas[k]*np.pi/180)*np.cos(phis[k]*np.pi/180)
                                AngleArrays[s,AngleIters[s],1] = np.sin(tmpAngleArray[l,0])*np.sin(tmpAngleArray[l,1])#np.sin(thetas[k]*np.pi/180)*np.sin(phis[k]*np.pi/180)
                                AngleIters[s] += 1
                            v = GetV(s)
                            axGiant[v[0],v[1]].plot(x[:,0],x[:,2],color="dodgerblue",ls="--",lw=0.1)
                            break

                    th_ph_c += 1

                for k in range(100):
                    TH_PH_Paths[pathPoints,0] = (thetas[1] - thetas[0])/100*k + thetas[0]
                    TH_PH_Paths[pathPoints,1] = (phis[1] - phis[0])/100*k + phis[0]
                    TH_PH_Paths[pathPoints,2] = (R_Arr[plottedPoints,1] - R_Arr[plottedPoints,0])/100*k + R_Arr[plottedPoints,0]
                    pathPoints += 1

                if plottedPoints < 1000:
                    ax2.plot(np.sin(thetas*np.pi/180)*phis,thetas,color="crimson",ls="None",ms=1.5,marker="*")
                    ax2.plot(np.sin(thetas*np.pi/180)*phis,thetas,color="dodgerblue",ls="--",lw=0.2)


                amount = 25
            """line = np.zeros(3)
            for k in range(amount):
                for o in range(3):
                    line[o] = (x[1,o] - x[0,o])/amount*k + x[0,o]
                axelF.scatter(line[0],line[1],line[2],color="dodgerblue",marker="o",s=0.2)#,markersize=1.5)
            axelF.scatter(x[:,0],x[:,1],x[:,2],color="crimson",marker="^",s=0.95)#,markersize=1.5)
            """
            plottedPoints += 1
        continue

    if nextG:
        r[0] = 0
        r[1] = 0
        signs = [1,1,1]
        scalar = 0
        for j in range(3):
            x[0,j] = data[i,j+1]*signs[j]
            x[1,j] = data[i+1,j+1]*signs[j]
            r[0] += data[i,j+1]**2
            r[1] += data[i+1,j+1]**2
            scalar += data[i,j+1]*data[i+1,j+1]
        for j in range(2):
            R_Arr[plottedPoints,j] = np.sqrt(r[j])
        xtmp = np.zeros(3)
        ntmp = 0
        for j in range(50):
            ntmp = 0
            for k in range(3):
                xtmp[k] = (x[1,k]-x[0,k])/25*j + x[0,k]
                ntmp += xtmp[k]**2
            tmpAngleArray[j,0] = math.acos(xtmp[2]/np.sqrt(ntmp))
            tmpAngleArray[j,1] = math.atan2(xtmp[1],xtmp[0])
            

        R_Arr[plottedPoints,2] = data[i,0]
        tmpDist = data[i,4]
        tmpAir = data[i,5]
        anlgeTmp = math.acos(scalar/np.sqrt(r[0]*r[1]))*180/np.pi
        EnergyBool = bool(data[i,0] >= 661.7/2 - delta) and (data[i,0] <= 661.7/2 + delta)
        nextG = False
        EArr[plottedPoints] = data[i,0]

from matplotlib.colors import LogNorm

ax[1,1].hist2d(TH_PH[:plottedPoints,0],TH_PH[:plottedPoints,1],bins=(90,180),range=[[0,180],[-180,180]],cmap = plt.cm.jet,norm=LogNorm())

# theta phi
ax[2,1].hist2d(TH_PH_Paths[:pathPoints,0],TH_PH_Paths[:pathPoints,1],bins=(180,180),range=[[0,180],[-180,180]],cmap = plt.cm.jet,norm=LogNorm())
ax[2,1].hist2d(TH_PH[:plottedPoints,0],TH_PH[:plottedPoints,1],bins=(90,180),range=[[0,180],[-180,180]],cmap = plt.cm.winter,norm=LogNorm(),alpha=0.4)

#sphere projection
ax[2,0].hist2d(np.sin(TH_PH_Paths[:pathPoints,0]*np.pi/180)*np.cos(TH_PH_Paths[:pathPoints,1]*np.pi/180),np.sin(TH_PH_Paths[:pathPoints,0]*np.pi/180)*np.sin(TH_PH_Paths[:pathPoints,1]*np.pi/180),bins=(100,100),range=[[-1,1],[-1,1]],cmap = plt.cm.jet,norm=LogNorm())
ax[2,0].hist2d(np.sin(TH_PH[:plottedPoints,0]*np.pi/180)*np.cos(TH_PH[:plottedPoints,1]*np.pi/180),np.sin(TH_PH[:plottedPoints,0]*np.pi/180)*np.sin(TH_PH[:plottedPoints,1]*np.pi/180),bins=(100,100),range=[[-1,1],[-1,1]],cmap = plt.cm.winter,norm=LogNorm())



#ax[1,1].hist(R_Arr[:,0],bins=100,range=[220,270],histtype="step",color="crimson")
#ax[1,1].hist(R_Arr[:,1],bins=100,range=[220,270],histtype="step",color="dodgerblue")
#ax[1,0].gca().invert_yaxis()
ax[0,0].axis('equal')
ax[1,0].axis('equal')
ax[0,1].axis('equal')
ax[2,0].axis('equal')
ax[1,1].set_xlim(0,80)
ax[2,1].set_xlim(0,80)

#ax[1,1].axhline(235,color="k",lw=1,ls="--")
#ax[1,1].axvline(235,color="k",lw=1,ls="--")


plt.figure(22,figsize=(5,4))
plt.clf()
plt.hist2d(np.sin(TH_PH_Paths[:pathPoints,0]*np.pi/180)*np.cos(TH_PH_Paths[:pathPoints,1]*np.pi/180),np.sin(TH_PH_Paths[:pathPoints,0]*np.pi/180)*np.sin(TH_PH_Paths[:pathPoints,1]*np.pi/180),bins=(200,200),range=[[-1,1],[-1,1]],cmap = plt.cm.jet,cmin=1)#,norm=LogNorm())
plt.hist2d(np.sin(TH_PH[:th_ph_c,0]*np.pi/180)*np.cos(TH_PH[:th_ph_c,1]*np.pi/180),np.sin(TH_PH[:th_ph_c,0]*np.pi/180)*np.sin(TH_PH[:th_ph_c,1]*np.pi/180),bins=(100,100),range=[[-1,1],[-1,1]],cmap = plt.cm.autumn,cmin=1)#,norm=LogNorm())
plt.colorbar()




dataA = np.loadtxt("Gamma_Single_Cs/Angles.dat")

#ganil angles

ganilArray = np.zeros((1000,3))
gIter = 0
for i in range(3):
    dataG = np.loadtxt("inputdata/GANIL_ANGLES_"+str(i+1)+".Angles")
    print("Angles:",dataG)
    for j in range(len(dataG)):
        for k in range(len(dataG[0])):
            ganilArray[gIter,k] = dataG[j,k]
        gIter += 1

print(gIter)


plt.figure(23,figsize=(5,4))
plt.clf()
#plt.hist2d(np.sin(dataA[:,0]*np.pi/180)*np.cos(dataA[:,1]*np.pi/180),np.sin(dataA[:,0]*np.pi/180)*np.sin(dataA[:,1]*np.pi/180),bins=(100,100),range=[[-1,1],[-1,1]],cmap = plt.cm.autumn,cmin=1)#,norm=LogNorm())
plt.hist2d(np.sin(TH_PH_Paths[:pathPoints,0]*np.pi/180)*np.cos(TH_PH_Paths[:pathPoints,1]*np.pi/180),np.sin(TH_PH_Paths[:pathPoints,0]*np.pi/180)*np.sin(TH_PH_Paths[:pathPoints,1]*np.pi/180),bins=(100,100),range=[[-1,1],[-1,1]],cmap = plt.cm.jet,norm=LogNorm())
plt.hist2d(np.sin(TH_PH[:th_ph_c,0]*np.pi/180)*np.cos(TH_PH[:th_ph_c,1]*np.pi/180),np.sin(TH_PH[:th_ph_c,0]*np.pi/180)*np.sin(TH_PH[:th_ph_c,1]*np.pi/180),bins=(100,100),range=[[-1,1],[-1,1]],cmap = plt.cm.cool,norm=LogNorm(),alpha=0.4)
x_a = np.zeros((2,2))
for i in range(gIter):
    
    th = ganilArray[i,0]
    phi = [ganilArray[i,1],ganilArray[i,2]]
    for k in range(2):
        x_a[k,0] = np.sin(th*np.pi/180)*np.cos(phi[k]*np.pi/180)
        x_a[k,1] = np.sin(th*np.pi/180)*np.sin(phi[k]*np.pi/180)

    plt.plot(x_a[:,0],x_a[:,1],color="k",ls="None",marker="*")
plt.axis("equal")


plt.figure(24,figsize=(5,4))
plt.clf()
#plt.hist2d(dataA[:,0],dataA[:,1],bins=(90,180),range=[[0,180],[-180,180]],cmap = plt.cm.autumn,cmin=1)#,norm=LogNorm())
plt.hist2d(np.sin(TH_PH_Paths[:pathPoints,0]*np.pi/180)*np.cos(TH_PH_Paths[:pathPoints,1]*np.pi/180),np.sin(TH_PH_Paths[:pathPoints,0]*np.pi/180)*np.sin(TH_PH_Paths[:pathPoints,1]*np.pi/180),bins=(100,100),range=[[-1,1],[-1,1]],cmap = plt.cm.jet,norm=LogNorm())

plt.hist2d(TH_PH[:th_ph_c,0],TH_PH[:th_ph_c,1],bins=(90,180),range=[[0,180],[-180,180]],cmap = plt.cm.cool,cmin=1)#,norm=LogNorm())
x_a = np.zeros((2,2))
for i in range(gIter):
    
    th = ganilArray[i,0]
    phi = [ganilArray[i,1],ganilArray[i,2]]

    plt.plot(th,phi[0],color="k",ls="None",marker="*")
    plt.plot(th,phi[1],color="k",ls="None",marker="*")



plt.figure(25,figsize=(5,4))
plt.clf()
#plt.hist2d(np.sin(dataA[:,0]*np.pi/180)*np.cos(dataA[:,1]*np.pi/180),np.sin(dataA[:,0]*np.pi/180)*np.sin(dataA[:,1]*np.pi/180),bins=(100,100),range=[[-1,1],[-1,1]],cmap = plt.cm.autumn,cmin=1)#,norm=LogNorm())
#plt.hist2d(np.sin(TH_PH_Paths[:pathPoints,0]*np.pi/180)*np.cos(TH_PH_Paths[:pathPoints,1]*np.pi/180),TH_PH_Paths[:pathPoints,3],bins=(100,200),range=[[-1,1],[100,500]],cmap = plt.cm.jet,norm=LogNorm())
plt.hist2d(np.sin(TH_PH[:th_ph_c,0]*np.pi/180)*np.cos(TH_PH[:th_ph_c,1]*np.pi/180),TH_PH[:th_ph_c,5],bins=(100,180),range=[[-1,1],[0,180]],cmap = plt.cm.jet,norm=LogNorm())


dataTh = np.loadtxt("Thrown.dat")



plt.figure(26,figsize=(5,4))
plt.clf()
plt.hist2d(np.sin(TH_PH_Paths[:pathPoints,0]*np.pi/180)*np.cos(TH_PH_Paths[:pathPoints,1]*np.pi/180),np.sin(TH_PH_Paths[:pathPoints,0]*np.pi/180)*np.sin(TH_PH_Paths[:pathPoints,1]*np.pi/180),bins=(100,100),range=[[-1,1],[-1,1]],cmap = plt.cm.winter,norm=LogNorm())
plt.hist2d(np.sin(dataTh[:,1])*np.cos(dataTh[:,2]),np.sin(dataTh[:,1])*np.sin(dataTh[:,2]),bins=(100,100),range=[[-1,1],[-1,1]],cmap = plt.cm.jet,norm=LogNorm())



figK,ax22 = plt.subplots(3,figsize=(5,4),sharex=True)
ax22[0].hist2d(TH_PH[:th_ph_c,3],TH_PH[:th_ph_c,5],bins=(250,180),range=[[0,500],[0,180]],cmap = plt.cm.jet,norm=LogNorm())
ax22[1].hist2d(TH_PH[:th_ph_c,4],TH_PH[:th_ph_c,5],bins=(250,180),range=[[0,500],[0,180]],cmap = plt.cm.jet,norm=LogNorm())
ax22[2].hist2d(TH_PH[:th_ph_c,3]-TH_PH[:th_ph_c,4],TH_PH[:th_ph_c,5],bins=(250,180),range=[[0,500],[0,180]],cmap = plt.cm.jet,norm=LogNorm())
#plt.hist2d(np.sin(dataTh[:,1])*np.cos(dataTh[:,2]),np.sin(dataTh[:,1])*np.sin(dataTh[:,2]),bins=(100,100),range=[[-1,1],[-1,1]],cmap = plt.cm.jet,norm=LogNorm())



figBlub,axB = plt.subplots(5,4,figsize=(25,16))
s = 0
for i in range(5):
    for j in range(4):
        axB[i,j].hist2d(AngleArrays[s,:AngleIters[s],0],AngleArrays[s,:AngleIters[s],1],bins=(100,100),range=[[-1,1],[-1,1]],cmap = plt.cm.jet,norm=LogNorm())
        axB[i,j].text(-0.8,0.8,"$d \\to $  " + str(int(distranges[s])))
        s += 1
        axB[i,j].axis("equal")