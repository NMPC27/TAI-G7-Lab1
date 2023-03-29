import subprocess





tc_list =[0.01,0.05]



for tc in tc_list:
    command = 'python3 generate_results.py --fname chry -k12 -pf -a1 -rm -tc:'+str(tc)
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
    process.wait()

    if process.returncode == 0:
        print('success:'+ command)
    else:
        print('ERROR:'+ command)


