import numpy as np
import matplotlib.pyplot as plt

plt.close("all")

from matplotlib import rc
rc('text', usetex=True)


dataSO = np.loadtxt("Lens.dat")

nS = dataSO[0]
nOFT = dataSO[1]
nMC = dataSO[2]

Nall = 10559901*0.996
N7 = 10557454

Nall = N7

bar1 = nS/Nall
bar2 = nOFT/Nall
bar3 = (nOFT + nMC)/Nall

rel12 = (1-bar1)/(1-bar2)
rel23 = (1-bar1)/(1-bar3)
#rel23 = 1/((1-bar3)/(1-bar2))

x1 = 0.75
x2 = 1.25
x3 = 1.75


vP = 1 - (1-bar1)/(1-bar2)
vP2 = 1-(1-bar2)/(1-bar3)

print("performance:",(bar2-bar1)/bar1,(bar3-bar2)/bar2)
print("performanceM:",(1-bar2-1+bar1)/(1-bar1),(1-bar3-1+bar2)/(1-bar2))
print("p3",(1-bar1)/(1-bar2),(1-bar2)/(1-bar3))

print("waldi",(bar2-bar1)/(1-bar1),(bar3-bar1)/(1-bar1),(bar3-bar1)/(bar2-bar1))

print(bar1,bar2,bar3)

#nMC = nOFT + len(dataMC)

names = ["Singles", "OFT", "OFT+MCT"]
values = [x1+0.2,x2+0.2,x3+0.2]

xs = np.array([x1,x2,x3])
xs += 0.2

ys = np.array([0,rel12,rel23])

ALPHA = 1


SINGLE_COLOR = "darkgreen"
OFT_COLOR = "r"
OFTMC_COLOR = "royalblue"

bar1 *= 100
bar2 *= 100
bar3 *= 100

fx = 5
fy = 3

fig,ax = plt.subplots(1,figsize=(5,4),sharex=True)
plt.figure(1,figsize=(fx,fy))

yl = [i*20 for i in range(6)]
ylS = ["$"+str(y)+"$" for y in yl]


#plt.setp(ax, xticks=values, xticklabels=names)

plt.bar(x1,bar1,width=0.4,color=SINGLE_COLOR,align="edge",alpha=ALPHA,edgecolor="k")
plt.bar(x2,bar2,width=0.4,color=OFT_COLOR,align="edge",alpha=ALPHA,edgecolor="k")
plt.bar(x3,bar3,width=0.4,color=OFTMC_COLOR,align="edge",alpha=ALPHA,edgecolor="k")
plt.bar(x1,100-bar1,width=0.4,color=SINGLE_COLOR,align="edge",hatch="/",alpha = 0.4,bottom=bar1,edgecolor="k")
plt.bar(x2,100-bar2,width=0.4,color=OFT_COLOR,align="edge",hatch="/",alpha = 0.4,bottom=bar2,edgecolor="k")
plt.bar(x3,100-bar3,width=0.4,color=OFTMC_COLOR,align="edge",hatch="/",alpha = 0.4,bottom=bar3,edgecolor="k")

plt.bar(-1,0.1,color="grey",edgecolor="k",label="Reconstructed")
plt.bar(-1,0.1,color="grey",hatch="/",edgecolor="k",label="Not Reconstructed",alpha=0.4)
#plt.vlines(xs,1,1.2,colors="k",linestyles="dashed",lw=0.8)



plt.axhline(100,color="k",ls="--")

#plt.plot(xs[0:3],ys[0:3],ls="-.",color="k",lw=1)
#plt.plot(x2+0.2,rel12,color=OFT_COLOR,marker="o",ms=8)
#plt.plot(x3+0.2,rel23,color=OFTMC_COLOR,marker="o",ms=8)
#plt.plot(xs[1:3],ys[1:3],ls=":",color=OFTMC_COLOR)
plt.tick_params(axis='both', which='major', labelsize=15)
plt.tick_params(axis='both', which='minor', labelsize=10)
plt.tick_params(axis='both', which='major', labelsize=15)
plt.tick_params(axis='both', which='minor', labelsize=10)

plt.xticks(values,names)
plt.yticks(yl,ylS)
#plt.axhline(100,color="k",ls='-.',lw=2)
plt.xlim([0.65,2.25])
plt.ylim([0,1.5*100])
#plt.xticks(1, ('')) 
#plt.xlabel("",fontsize=20)
plt.ylabel("Rel. Reconstructed Photons ($\\%$)",fontsize = 16)
#plt.set_ylabel("hellp")
plt.legend(loc=2,fontsize=12)
#plt.ylim([0,1.10])
label = ["$\\xi = 0.73$", "$\\xi_1 = 126.5\\,\\%$", "$\\xi_2 = 17.5\\,\\%$"]
label2 = ["$\\delta_F = 0.73$", "$\\delta_F = 0.40$", "$\\varepsilon = 0.70$"]

#plt.text(x = x1 + 0.2 , y = 1+0.08, s = label[0], size = 14,ha='center', va='center')
plt.text(x = (xs[0]+xs[1])/2 , y = 100+0.08*100, s = label[1], size = 14,ha='center', va='center')
plt.text(x = (xs[1]+xs[2])/2 , y = 100+0.08*100, s = label[2], size = 14,ha='center', va='center')

delta = 8

plt.text(x = (xs[0]) , y = 27-delta, s = "$26.6\\,\\%$", size = 17,ha='center', va='center',color="w")
plt.text(x = (xs[1]) , y = 60-delta, s = "$60.3\\,\\%$", size = 17,ha='center', va='center',color="w")
plt.text(x = (xs[2]) , y = 71-delta, s = "$70.9\\,\\%$", size = 17,ha='center', va='center',color="w")



delta = xs[1] - 0.075 -xs[0]-0.075

#plt.arrow(xs[0]+0.075,1.16,delta,0,head_width=0.02,head_length=0.02,fc="k",ec="k")
#plt.arrow(xs[1]+0.075,1.16,delta,0,head_width=0.02,head_length=0.02,fc="k",ec="k")


plt.tick_params(
    axis='x',          # changes apply to the x-axis
    bottom=False)

plt.savefig("plotX_Range.pdf",bbox_inches="tight")




# MULTS

dataM = np.loadtxt("Mults.dat")


nI = np.array([i for i in range(len(dataM))])

summe = sum(dataM)

dataM /= summe

checkSum = 0
for i in range(7):
    checkSum += dataM[i]


print(checkSum,checkSum-dataM[1])

plt.figure(2,figsize=(fx,fy))
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
