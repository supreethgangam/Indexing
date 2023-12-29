import matplotlib.pyplot as plt
import seaborn as sns
import math
f=open("./data/all_ranges_7.txt")
l=[]
for i in f.readlines():
    l.append(i[:-1])
l=list(map(lambda x: x.split(),l))
l=list(map(lambda x: list(map(lambda y : int(y),x)),l))
bptree=[]
uheap=[]
for i in range(len(l)):
    bptree.append(l[i][0])
    uheap.append(l[i][1])
print(bptree,uheap)
plt.title("Histplot for FANOUT = 7")
plt.hist(bptree,label="B+ tree")
plt.xlabel("Number of block accesses")
yint = range(min(bptree), math.ceil(max(bptree))+1)
plt.xticks(yint)
plt.ylabel("Number of queries")
plt.hist(uheap,label="Unordered Heap")
yint = range(min(bptree), math.ceil(max(uheap))+1)
plt.xticks(yint)
plt.legend()
plt.show()