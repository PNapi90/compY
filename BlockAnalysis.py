import numpy as np
import matplotlib.pyplot as plt

plt.close("all")

from matplotlib import rc
rc('text', usetex=True)


dataSO = np.loadtxt("Lens.dat")

nS = dataSO[0]
nOFT = dataSO[1]
nMC = dataSO[2]

bar1 = 100
bar2 = nOFT/nS*100
bar3 = (nOFT + nMC)/nS*100

x1 = 0.75
x2 = 1.25
x3 = 1.75

print(bar1,bar2,bar3)

#nMC = nOFT + len(dataMC)

names = ["Singles", "OFT", "OFT + MC"]
values = [x1+0.2,x2+0.2,x3+0.2]

ALPHA = 1

plt.figure(1,figsize=(5,4))
plt.clf()
plt.bar(x1,bar1,width=0.4,color="silver",label="Singles",align="edge",hatch="/",alpha=ALPHA,edgecolor="k")
plt.bar(x2,bar2,width=0.4,color="crimson",align="edge",label="OFT",hatch="\\",alpha=ALPHA,edgecolor="k")
plt.bar(x3,bar3,width=0.4,color="dodgerblue",align="edge",label="OFT + MC",hatch="x",alpha=ALPHA,edgecolor="k")

plt.tick_params(axis='both', which='major', labelsize=15)
plt.tick_params(axis='both', which='minor', labelsize=10)
plt.xticks(values,names)
#plt.axhline(100,color="k",ls='-.',lw=2)
#plt.xlim([0,2])
#plt.xticks(1, ('')) 
#plt.xlabel("",fontsize=20)
plt.ylabel("Tracking efficiency rel. to Singles $\\varepsilon'$ in $\\%$",fontsize = 16)
#plt.legend(loc=2,fontsize=12,)
plt.ylim([0,300])
label = ["$\\varepsilon' = 100\\,\\%$", "$\\varepsilon' = 226\\,\\%$", "$\\varepsilon' = 262\\,\\%$"]

plt.text(x = x1 + 0.2 , y = bar1+10, s = label[0], size = 14,ha='center', va='center')
plt.text(x = x2 + 0.2 , y = bar2+10, s = label[1], size = 14,ha='center', va='center')
plt.text(x = x3 + 0.2 , y = bar3+10, s = label[2], size = 14,ha='center', va='center')


plt.savefig("plotX_Range.pdf",bbox_inches="tight")




# MULTS

dataM = np.loadtxt("Mults.dat")


nI = np.array([i for i in range(len(dataM))])

summe = sum(dataM)

dataM /= summe

checkSum = 0
for i in range(7):
    checkSum += dataM[i]

print(checkSum)

plt.figure(2,figsize=(5,4))
plt.clf()
plt.plot(nI,dataM,color="dodgerblue",ls="None",lw=0.5,ms=3,marker="o")
plt.vlines(nI,0,dataM,colors="dodgerblue",linestyles="dashed",lw=0.8)
plt.tick_params(axis='both', which='major', labelsize=15)
plt.tick_params(axis='both', which='minor', labelsize=10)
plt.xlabel("Amount of Hits",fontsize = 20)
plt.xlim([-0.5,10.5])
plt.ylabel("Probability density",fontsize = 20)
#plt.yscale("log")
plt.savefig("plotMult.pdf",bbox_inches="tight")
