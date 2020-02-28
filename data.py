import subprocess
import csv
import sys
import numpy as np
name=sys.argv[1]
with open('data_'+name+'.csv', 'w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["n=?", "Parallel", "Comm", "Serial"])
    for i in ['10','50', '100','200', '500', '1000', '2000', '4000', '10000', '20000']: #,'1200', '1400', '2000', '2500', '5000', '10000']:
        s=subprocess.check_output(['./run.sh', name, '2', i])
        splitt=s.decode().split('\n')
       # paralleltime=[]
        times_p=[]
        for j in range(len(splitt)):
            #paralleltime=paralleltime.append(splitt[j].split(' '))
            x=splitt[j].split(' ')
            if x[0]=="mult1":
                mult_time=int(x[1])
            elif (x[0]=="parallel_time:1") or (x[0]=="parallel_time:0"):
                times_p.append( int(x[1]) )
            elif (x[0]=="serial_time:"):
                serial=int(x[1])
        parallel = max(times_p)
        com=parallel-mult_time
        #paralleltime=(splitt[0].split(' '))[1]
        #serial=(splitt[2].split(':'))[1]
        #isequal=(splitt[3].split(':'))[1]
        writer.writerow([i,str(parallel), str(com), str(serial)])
