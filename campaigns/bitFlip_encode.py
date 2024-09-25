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
slots = 2**(logN-1)
if n>4:
    slots = slots >> int(sys.argv[4])


ringDim = 2**logN
batchSize = ringDim//2
num_bitsPerCoeff = 66
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
dir     = "logs/log_encode/"

fileN2     =  f"HEAAN_N2_{logN}_{logQ}_{logP}_{slots}.txt"
fileN2_real = f"HEAAN_N2_real_{logN}_{logQ}_{logP}_{slots}.txt"
fileDiff    = f"HEAAN_Diff_{logN}_{logQ}_{logP}_{slots}.txt"


extra   = ""
prolog = "img/HEAAN"
show = True

loops = seeds * input_seeds

num_coeff = int(ringDim*RNS_size)

num_bits = num_coeff * num_bitsPerCoeff


width = 5


##############################################################################
print("N2")
savename = f"N2_{logN}_{num_bitsPerCoeff}_{slots}"

df_N2 = pd.read_csv(dir+fileN2_real,  skiprows=1, header=None, skip_blank_lines=False)
params =  pd.read_csv(dir+fileN2_real, nrows=1, header=None, skip_blank_lines=False).values.flatten().tolist()
logN = params[0]
logQ = params[1]
logP = params[2]
delta = params[3]
loops = params[4]
ringDim = 2**logN
batchSize = ringDim//2
num_bitsPerCoeff = params[4]
labels = [str(0), str(num_bitsPerCoeff)]
for i in range(2, num_coeff+1):
    labels.append(str(i)+f"x{num_bitsPerCoeff}")
xticks_label = np.arange(0, (ringDim+1)*num_bitsPerCoeff, num_bitsPerCoeff)

dataN2 = df_N2.to_numpy(dtype='float64')
stdN2 = np.std(dataN2, axis=0)
dataN2_mean = np.mean(dataN2, axis=0)
x = np.arange(0, num_bitsPerCoeff*ringDim,1)
if(num_bitsPerCoeff>64):
    print("Reshape")
    print(len(x))
    x = np.arange(0, 64*ringDim,1)
    labels = [str(0), str(64)]
    for i in range(2, num_coeff+1):
        labels.append(str(i)+f"x{64}")
    xticks_label = np.arange(0, (ringDim+1)*64, 64)
    print(len(x))

    data_reshaped = dataN2_mean.reshape(-1, num_bitsPerCoeff)
    new_arr = data_reshaped[:, :64]
    dataN2_mean= new_arr.flatten()

    std_reshaped = stdN2.reshape(-1, num_bitsPerCoeff)
    newstd_arr = std_reshaped[:, :64]
    stdN2= newstd_arr.flatten()

plt.plot(x,  dataN2_mean, linewidth=width, label=f"Delta = 2^{delta}")
#plt.scatter(x,  dataN2_mean_30, linewidth=width, label="Delta = 25")
plt.errorbar(x, dataN2_mean, stdN2, linestyle='None', marker='^', color="orange")
plt.ylabel('Norm 2')
plt.xlabel('Bit changed')
plt.xticks(xticks_label , labels, rotation=45)
plt.legend()
plt.savefig(prolog+f"{savename}.pdf", bbox_inches='tight')
plt.savefig(prolog+f"{savename}.jpg", bbox_inches='tight')
plt.show()
plt.clf()

if n<5:
    print("Diff")
    df_DIFF = pd.read_csv(dir+fileDiff, header=None, skip_blank_lines=False, dtype='float64')
    encodingDIFF = df_DIFF.to_numpy(dtype='int64')

    encodingDiff_mean = np.mean(encodingDIFF, axis=0)
    print(encodingDiff_mean)
    encodingDiff_mean_split = np.reshape(encodingDiff_mean, (ringDim*num_bitsPerCoeff, batchSize))
# Define los múltiplos permitidos
    print(encodingDiff_mean_split)
    print("Matriz", num_bitsPerCoeff, batchSize )
# Encuentra todos los índices de columna que son múltiplos de los valores permitidos
    matriz_filtrada = np.zeros((ringDim*(num_bitsPerCoeff-50), batchSize))

#print(matriz_filtrada)
    indices_permitidos = []
    for i in range(num_coeff):
        col = 0
        for j in range(50, num_bitsPerCoeff, 1):
            matriz_filtrada[i][col] = encodingDiff_mean[j*(i+1)]
            col+=1



    print(matriz_filtrada)
    savename = "diff"
    sns.heatmap(np.transpose(matriz_filtrada),  cmap="gray_r", vmin=0)
    plt.ylabel('Input element')
    plt.xlabel('Bit Changed')
    plt.savefig(prolog+f"{savename}.pdf", bbox_inches='tight')
    plt.savefig(prolog+f"{savename}.jpg", bbox_inches='tight')
    if show:
        plt.show()
    plt.clf()

