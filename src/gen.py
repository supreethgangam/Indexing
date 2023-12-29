import random
f=open("../data/input.txt","w+")
a=list(range(1,101))
random.shuffle(a)
for i in range(len(a)):
    string="INSERT {}".format(a[i])
    f.write(string)
    f.write("\n")
for i in range(1,100):
    string="RANGE {} {}".format(i,i+1)
    f.write(string)
    f.write("\n")
f.close()