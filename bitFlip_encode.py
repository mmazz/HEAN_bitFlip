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
num_bitsPerCoeff = (logQ+logP+6)
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

fileN2     = f"smallEncodeN2_{logN}_{logQ}_{logP}.txt"
fileDiff     = f"smallEncodeDiff_{logN}_{logQ}_{logP}.txt"
extra   = ""
prolog = "img/smallEncode"
show = True

loops = seeds * input_seeds

num_coeff = int(ringDim*RNS_size)

num_bits = num_coeff * num_bitsPerCoeff

labels = [str(0), str(num_bitsPerCoeff)]
for i in range(2, num_coeff+1):
    labels.append(str(i)+f"x{num_bitsPerCoeff}")

xticks_label = np.arange(0, (ringDim+1)*num_bitsPerCoeff, num_bitsPerCoeff)
print(xticks_label)
width = 5
##############################################################################
print("N2")
savename = f"N2_{logN}_{num_bitsPerCoeff}"
df_N2 = pd.read_csv(dir+fileN2, header=None, skip_blank_lines=False)
dataN2 = df_N2.to_numpy(dtype='float64')
stdN2 = np.std(dataN2, axis=0)
dataN2_mean = np.mean(dataN2, axis=0)
print(len(dataN2_mean))
x = np.arange(0, num_bitsPerCoeff*ringDim,1)
plt.plot(x,  dataN2_mean, linewidth=width, label="Delta = 25")
#plt.scatter(x,  dataN2_mean_30, linewidth=width, label="Delta = 25")
plt.errorbar(x, dataN2_mean, stdN2, linestyle='None', marker='^', color="orange")
plt.ylabel('Norm2 ')
plt.xlabel('Bit changed')
plt.xticks(xticks_label , labels, rotation=45)
plt.savefig(prolog+f"{savename}.pdf", bbox_inches='tight')
plt.savefig(prolog+f"{savename}.jpg", bbox_inches='tight')
plt.legend()
plt.show()
plt.clf()
a

print("Diff")
df_DIFF = pd.read_csv(dir+fileDiff, header=None, skip_blank_lines=False, dtype='float64')
encodingDIFF = df_DIFF.to_numpy(dtype='int64')

encodingDiff_mean = np.mean(encodingDIFF, axis=0)
encodingDiff_mean_split = np.reshape(encodingDiff_mean, (ringDim*num_bitsPerCoeff, batchSize))
# Define los múltiplos permitidos
multiples_permitidos = set(range(50, 65))
print(multiples_permitidos)
# Encuentra todos los índices de columna que son múltiplos de los valores permitidos
indices_permitidos = set()
for valor in multiples_permitidos:
    indices_permitidos.update(range(valor, batchSize + 1, valor))
print(indices_permitidos)
# Convierte a lista y ordena
indices_permitidos = sorted(indices_permitidos)

# Filtra la matriz para conservar solo las columnas permitidas
matriz_filtrada = encodingDiff_mean_split[:, indices_permitidos]
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

