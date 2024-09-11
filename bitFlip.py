import sys
import pandas as pd

import matplotlib.pyplot  as plt
import numpy as np
from matplotlib.ticker import PercentFormatter
import seaborn as sns

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
dir     = "logs/log_encode/"
fileN2_30     = "smallEncodeN2_30.txt"
extra   = ""
prolog = "img/smallEncode"
show = True

ringDim = 2**2
batchSize = ringDim//2
num_bitsPerCoeff = 40
RNS_size = 1
seeds = 10
input_seeds = 1
loops = seeds * input_seeds

num_coeff = int(ringDim*RNS_size)

num_bits = num_coeff * num_bitsPerCoeff

labels = [str(0), str(num_bitsPerCoeff)]
for i in range(2, num_coeff+1):
    labels.append(str(i)+f"*{num_bitsPerCoeff}")

xticks_label = np.arange(0, (ringDim+1)*num_bitsPerCoeff, num_bitsPerCoeff)
print(xticks_label)
width = 5
##############################################################################
print("N2")
savename = "N2_25"
df_N2 = pd.read_csv(dir+fileN2_30, header=None, skip_blank_lines=False)
dataN2_30 = df_N2.to_numpy(dtype='float64')
stdN2 = np.std(dataN2_30, axis=0)
dataN2_mean_30 = np.mean(dataN2_30, axis=0)
print(len(dataN2_mean_30))
x = np.arange(0, num_bitsPerCoeff*ringDim,1)
plt.plot(x,  dataN2_mean_30, linewidth=width, label="Delta = 25")
#plt.scatter(x,  dataN2_mean_30, linewidth=width, label="Delta = 25")
#plt.errorbar(x, dataN2_mean_30, stdN2, linestyle='None', marker='^', color="orange")
plt.ylabel('Norm2 ', color='green')
plt.xlabel('Bit changed')
plt.xticks(xticks_label , labels, rotation=45)
plt.savefig(prolog+f"{savename}.pdf", bbox_inches='tight')
plt.savefig(prolog+f"{savename}.jpg", bbox_inches='tight')
plt.legend()
plt.show()
plt.clf()




