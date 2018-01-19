
#############################  GLOBAL  ###############################
LIMIT=100

#############################  FUNCTION  ###############################
enable_uart()
{
	echo -n "launch uart4..."
	devc-seromap -E -F -b115200 -c48000000/16 0x4806e000^2,102 -u4
	if [ -n "`ls /dev/ser2`" ];then
		echo "done"
	else
		echo "failed"
	fi
	echo -n "launch uart8..."
	devc-seromap -E -F -b9600 -c48000000/16 0x48422000^2,175 -u8
	if [ -n "`ls /dev/ser3`" ];then
		echo "done"
	else
		echo "failed"
	fi
}

check_uart()
{
	pidin ar | grep devc
}

send_uart4_data()
{
	echo "echo data to uart4..."
	if [ -e "/dev/ser2" ];then
		COUNT=0
		while [ "$COUNT" -le "$LIMIT" ]
		do
			COUNT=$(($COUNT+1))
			echo "send 1"
			echo 1 > /dev/ser2
			sleep 0.5
			echo "send 2"
			echo 2 > /dev/ser2
			sleep 1
		done
	fi
}

choice()
{
	echo ""
	echo "Please choose action for uart test:"
	echo "1) check_uart"
	echo "2) enable_uart"
	echo "3) send_uart4_data"
	echo "9) exit"
	read ACTION
}

##############################  MAIN  ##################################
:<<NOUSE
clear
while true
do
	choice
	case "$ACTION" in
		1)
			check_uart
			;;
		2)
			enable_uart
			;;
		3)
			send_uart4_data
			;;
		*)
			exit
			;;
	esac
	sleep 1
done
NOUSE
check_uart
enable_uart
/bin/uart_ipc &
exit 0
