import subprocess
import csv
import sys
import numpy as np
name=sys.argv[1]
with open('data_'+name+'.csv', 'w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["n=?", "Parallel", "Serial", "Isequal"])
    for i in ['100', '1000','1200', '1400', '2000', '2500', '5000', '10000']:
        s=subprocess.check_output(['./run.sh', name, '2', i])
        splitt=s.decode().split('\n')
        paralleltime=(splitt[0].split(':'))[1]
        serial=(splitt[2].split(':'))[1]
        isequal=(splitt[3].split(':'))[1]
        writer.writerow([i,str(paralleltime), str(serial), str(isequal)])
