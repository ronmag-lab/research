exp_name=multiple_sets
website=http://ec2-18-206-203-46.compute-1.amazonaws.com/full_attack_RAF.html
VNCPORT=36
if [ -z $1 ];then
	num_of_succes=1
else
	num_of_succes=$1
fi
i="3"
while [ $i -lt $num_of_succes ]
do
sleep 3 && ./get_mozila_address.sh > set_and_slice.txt && sudo ~/Mastik-0.02-AyeAyeCapn_scan/demo/L3-scan $(<set_and_slice.txt) > l3_noise_results/$exp_name"_"$i.txt && DISPLAY=:$VNCPORT xdotool key "ctrl+w"
if [ -s l3_noise_results/$exp_name"_"$i.txt  ]
then
i=$[$i+1]
fi
done
