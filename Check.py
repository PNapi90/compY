import numpy as np
import matplotlib.pyplot as plt

s = np.random.normal(0,10,10000)

plt.close("all")

from matplotlib import rc
rc('text', usetex=True)


dataN = np.loadtxt("OutputFolder/ONon")
data = np.loadtxt("OutputFolder/Output_FWHM_5.000000_0_1_2inter")
data2 = np.loadtxt("Gamma_GANIL/OFT_2Ints.dat")
data3 = np.loadtxt("Gamma_GANIL/All_2Ints.dat")

print(len(data),len(data2),len(data3),len(dataN))
print("oft+mc",1-len(data)/len(data3))
print("mc",1-len(dataN)/len(data3))
print("oft",1-len(data2)/len(data3))

x1 = 0.8
x2 = 1.0
x3 = 1.2

widthB = 0.15


bar3 = len(data)/len(data3)*100
bar2 = len(data2)/len(data3)*100
bar1 = len(dataN)/len(data3)*100

yl = [i*25 for i in range(5)]
ylS = ["$"+str(y)+"$" for y in yl]

names = ["MC", "OFT", "OFT+MC"]
values = [x1+widthB/2,x2+widthB/2,x3+widthB/2]


nbins = 700

fig = plt.figure(1,figsize=(5,3))
plt.clf()
plt.hist(data2,bins=nbins,range=[0,700],histtype="step",color="r",label="OFT")
plt.hist(data[:,2],bins=nbins,range=[0,700],histtype="step",color="royalblue",label="OFT + MC")
plt.hist(dataN[:,2],bins=nbins,range=[0,700],histtype="step",color="orange",label="OFT + MC")
#plt.hist(data3,bins=nbins,range=[0,700],histtype="step",color="k",label="OFT")


plt.tick_params(axis='both', which='major', labelsize=15)
plt.tick_params(axis='both', which='minor', labelsize=10) 
plt.xlabel("$E_{\\mathrm{dep},1}$ (keV)",fontsize=17)

plt.ylabel("Counts per 4 keV",fontsize = 17)

plt.ylim([0,25000])
plt.xlim([0,660])


a = plt.axes([.194, .45, .31, .33], facecolor='w')
plt.title("Rel. Reconstructed Photons ($\\%$)",fontsize=10)
plt.bar(x1,100-bar1,width=widthB,color="orange",align="edge",alpha=1,edgecolor="k")
plt.bar(x2,100-bar2,width=widthB,color="r",align="edge",alpha=1,edgecolor="k")
plt.bar(x3,100-bar3,width=widthB,color="royalblue",align="edge",alpha=1,edgecolor="k")
plt.bar(x1,bar1,width=widthB,color="orange",align="edge",hatch="/",alpha = 0.4,bottom=100-bar1,edgecolor="k")
plt.bar(x2,bar2,width=widthB,color="r",align="edge",hatch="/",alpha = 0.4,bottom=100-bar2,edgecolor="k")
plt.bar(x3,bar3,width=widthB,color="royalblue",align="edge",hatch="/",alpha = 0.4,bottom=100-bar3,edgecolor="k")

delta = 15

plt.text(x = values[0] , y = 100-bar1-delta, s = "$81\\,\\%$", size = 18,ha='center', va='center',color="w",rotation=90)
plt.text(x = values[1] , y = 100-bar2-delta, s = "$79\\,\\%$", size = 18,ha='center', va='center',color="w",rotation=90)
plt.text(x = values[2] , y = 100-bar3-delta, s = "$96\\,\\%$", size = 18,ha='center', va='center',color="w",rotation=90)
#plt.axhline(100,color="k",ls="--",lw=1.2)

plt.xticks(values,names,fontsize=10)
plt.yticks(yl,ylS)
plt.ylim([40,105])

#plt.ylabel("Rel. Reconst. $\\gamma$s in $\\%$",fontsize = 6)


plt.tick_params(
    axis='x',          # changes apply to the x-axis
    bottom=False)      # ticks along the bottom edge are off
#plt.hist(data3,bins=nbins,range=[0,700],histtype="step",color="orange",label="All Events")

#plt.legend(loc=2,fontsize=12)

# this is an inset axes over the main axes
#a = plt.axes([0.15, 0.5, 0.35, 0.35], facecolor='w')
#plt.hist(data2,bins=nbins,range=[0,700],histtype="step",color="r",label="OFT")
#plt.hist(data[:,2],bins=nbins,range=[0,700],histtype="step",color="royalblue",label="OFT + MC")
#plt.hist(data3,bins=nbins,range=[0,700],histtype="step",color="orange",label="All Events")
#plt.xticks([])
#plt.yticks([])





plt.savefig("plotE_abs.pdf",bbox_inches="tight")


fig = plt.figure(2,figsize=(5,3))
plt.clf()
plt.hist(data3,bins=nbins,range=[0,700],histtype="step",color="k",label="OFT")
plt.tick_params(axis='both', which='major', labelsize=15)
plt.tick_params(axis='both', which='minor', labelsize=10) 
plt.xlabel("$E_{\\mathrm{dep},1}$ (keV)",fontsize=17)

plt.ylabel("Counts per 4 keV",fontsize = 17)
plt.xlim([0,660])

plt.savefig("plotEall.pdf",bbox_inches="tight")


x,e = np.histogram(data[:,2],bins=nbins,range=[0,700],normed=True)

dist = data[:,0]-data[:,1]

h2 = np.zeros(len(data))

rel = 0
rels = 0

iter2 = 0

for i in range(len(dist)):
    if data[i,2] >= 200 and data[i,2] < 400:
        h2[iter2] = data[i,0] - data[i,1]
        iter2 += 1
        if dist[i] > 100:
            rel += 1

bb = np.linspace(0,700,nbins)
sss = 0
for i in range(len(x)):
    if bb[i] >= 200 and bb[i] < 400:
        sss += x[i]*700/nbins

print(sss,rel/len(data3),rels,rels/len(data3))


fig = plt.figure(3,figsize=(5,3))
plt.clf()
plt.hist(data[:,0]-data[:,1],bins=125,range=[0,500],histtype="step",color="royalblue",label="OFT + MC")
plt.hist(h2[0:iter2],bins=125,range=[0,500],histtype="step",color="crimson",label="OFT + MC")

plt.tick_params(axis='both', which='major', labelsize=15)
plt.tick_params(axis='both', which='minor', labelsize=10) 
#plt.xlabel("$E_{\\mathrm{dep},1}$ in keV",fontsize=17)