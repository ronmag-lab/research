VNCPORT=36
if [ -z $1 ];then
	website=http://ec2-54-236-23-225.compute-1.amazonaws.com/full_attack_RAF.html
else
	website=$1
fi

DISPLAY=:$VNCPORT ~/mozilla-central/mach run $website >table_address.txt & sleep 10 && sudo ./L3_check_pid `head table_address.txt | sed -n '3p'` `head table_address.txt | sed -n '4p'` `head table_address.txt | sed -n '5p'`

