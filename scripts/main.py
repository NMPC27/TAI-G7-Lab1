import subprocess


k_list = range(15,3,-1)


for k in k_list:
    command = 'python3 generate_results.py --fname chry -k'+str(k)+' -pf -a1 -rm -tf:6'
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
    process.wait()

    if process.returncode == 0:
        print('success:'+ command)
    else:
        print('ERROR:'+ command)

tf_list =[3,6,9]
tc_list =[0.01,0.05]
tn_list =[0.3,0.4,0.5]


for tf in tf_list:
    command = 'python3 generate_results.py --fname chry -k12 -pf -a1 -rm -tf:'+str(tf)
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
    process.wait()

    if process.returncode == 0:
        print('success:'+ command)
    else:
        print('ERROR:'+ command)

for tc in tc_list:
    command = 'python3 generate_results.py --fname chry -k12 -pf -a1 -rm -tc:'+str(tc)
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
    process.wait()

    if process.returncode == 0:
        print('success:'+ command)
    else:
        print('ERROR:'+ command)

for tn in tn_list:
    command = 'python3 generate_results.py --fname chry -k12 -pf -a1 -rm -tn:'+str(tn)
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
    process.wait()

    if process.returncode == 0:
        print('success:'+ command)
    else:
        print('ERROR:'+ command)
