import subprocess
import csv
import sys
import numpy as np
name=sys.argv[1]
vals=['10','50', '100','200', '500', '1000', '2000', '4000', '10000', '20000']
val2=['10','50', '100','200', '500', '1000', '2000', '4000', '10000']
val3=['10','50', '100','200', '500', '1000', '2000']
p=sys.argv[2]
with open('data_'+name+p+'.csv', 'w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["n=?", "Parallel", "Comm", "Serial"])
    for i in val3: #,'1200', '1400', '2000', '2500', '5000', '10000']:
        s=subprocess.check_output(['./run.sh', name, p, i])
        splitt=s.decode().split('\n')
       # paralleltime=[]
        times_p=[]
        mult_time={}
        index=[]
        for j in range(len(splitt)):
            #paralleltime=paralleltime.append(splitt[j].split(' '))
            x=splitt[j].split(' ')
            xn=x[0]
            if(xn==''):
                continue
            xname=xn[0:len(xn)-1]
            #print(xname)
            xnum=(xn[len(xn)-1])
            
            if xname=="mult":
                #print(xnum+"Pushed")
                mult_time[int(xnum)]=int(x[1])
                #print(mult_time[int(xnum)])
            elif (xname=="parallel_time:"):
                times_p.append( int(x[1]) )
                index.append(int(xnum))
            elif (xname=="serial_time"):
                serial=int(x[1])
        parallel = max(times_p)
        indices=times_p.index(parallel)
        #print(indices)
        #print(mult_time)
        com=parallel-mult_time[indices]
        #paralleltime=(splitt[0].split(' '))[1]
        #serial=(splitt[2].split(':'))[1]
        #isequal=(splitt[3].split(':'))[1]
        writer.writerow([i,str(parallel), str(com), str(serial)])
