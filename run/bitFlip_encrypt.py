import sys
import pandas as pd

import matplotlib.pyplot  as plt
import numpy as np
from matplotlib.ticker import PercentFormatter
import seaborn as sns

n = len(sys.argv)
logN = 4
if n >1 :
    logN = int(sys.argv[1])
logQ = 35
if n>2:
    logQ = int(sys.argv[2])
logP = 25
if n>3:
    logP = int(sys.argv[3])
ringDim = 2**logN
batchSize = ringDim//2
num_bitsPerCoeff = 35#logQ+logP+4
RNS_size = 1
seeds = 10
input_seeds = 1

SMALL_SIZE = 12
MEDIUM_SIZE = 16
BIGGER_SIZE = 20

plt.rc('font', size=SMALL_SIZE)          # controls default text sizes
plt.rc('axes', titlesize=SMALL_SIZE)     # fontsize of the axes title
plt.rc('axes', labelsize=MEDIUM_SIZE)    # fontsize of the x and y labels
plt.rc('xtick', labelsize=MEDIUM_SIZE)    # fontsize of the tick labels
plt.rc('ytick', labelsize=MEDIUM_SIZE)    # fontsize of the tick labels
plt.rc('legend', fontsize=SMALL_SIZE)    # legend fontsize
plt.rc('figure', titlesize=BIGGER_SIZE)  # fontsize of the figure title
dir     = "logs/log_encrypt/"

fileN2_C0     = f"smallEncryptN2_{logN}_{logQ}_{logP}_C0.txt"
fileN2_C1     = f"smallEncryptN2_{logN}_{logQ}_{logP}_C1.txt"
extra   = ""
prolog = "img/smallEncrpyt"
show = True

loops = seeds * input_seeds

num_coeff = int(2*ringDim*RNS_size)

num_bits = num_coeff * num_bitsPerCoeff

labels = [str(0), str(num_bitsPerCoeff)]
for i in range(2, num_coeff+1):
    labels.append(str(i)+f"x{num_bitsPerCoeff}")

xticks_label = np.arange(0, num_bits+1, num_bitsPerCoeff)
print(xticks_label)
width = 5
##############################################################################
print("N2")
savename = f"N2_{logN}_{num_bitsPerCoeff}"
x0 = np.arange(0, num_bitsPerCoeff*ringDim,1)
x1 = np.arange(num_bitsPerCoeff*ringDim, 2*num_bitsPerCoeff*ringDim,1)

df_N2_C0 = pd.read_csv(dir+fileN2_C0, header=None, skip_blank_lines=False)
dataN2_C0 = df_N2_C0.to_numpy(dtype='float64')
stdN2_C0 = np.std(dataN2_C0, axis=0)
dataN2_mean_C0 = np.mean(dataN2_C0, axis=0)

df_N2_C1 = pd.read_csv(dir+fileN2_C1, header=None, skip_blank_lines=False)
dataN2_C1 = df_N2_C1.to_numpy(dtype='float64')
stdN2_C1 = np.std(dataN2_C1, axis=0)
dataN2_mean_C1 = np.mean(dataN2_C1, axis=0)

plt.plot(x0,  dataN2_mean_C0, linewidth=width, color='steelblue', label="Delta = 25")
plt.plot(x1,  dataN2_mean_C1, linewidth=width, color='firebrick', label="Delta = 25")
#plt.scatter(x,  dataN2_mean_30, linewidth=width, label="Delta = 25")
plt.errorbar(x0, dataN2_mean_C0, stdN2_C0, linestyle='None', marker='^', color="orange")
plt.errorbar(x1, dataN2_mean_C1, stdN2_C1, linestyle='None', marker='^', color="orange")
plt.ylabel('Norm2')

plt.xlabel('Bit changed')
plt.xticks(xticks_label , labels, rotation=45)
plt.savefig(prolog+f"{savename}.pdf", bbox_inches='tight')
plt.savefig(prolog+f"{savename}.jpg", bbox_inches='tight')
plt.legend()
plt.show()
plt.clf()




