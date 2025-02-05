#!/bin/bash
files='crab_crab3_*_*_MINIAOD'
#for i in $files
#do
	#echo -e "\033[32m $i \033[0m"
	#crab status $i | sed -n -e '/jobs failed/p' > tmp_report.out
	cat report.out > tmp_report.out
	cat tmp_report.out | while read rows
	do	
		result=$(echo $rows | grep 'MINIAOD')
		to_resubmit=0
		if [[ $result != '' ]]
		then
			i=$rows
			#echo -e "\033[32m $i \033[0m"
		fi
		result1=$(echo $rows | grep '8001')
		if [[ $result1 != '' ]]
		then
			num=$(echo $result1 | awk '{print $1}')
			num=$(expr $num)
			if [[ $num -lt 100 ]]
			then
				echo -e "\033[32m $i \033[0m resubmit because $num 8001 failure"
		#		crab --quiet resubmit $i
				to_resubmit=1
			fi
		fi
		result2=$(echo $rows | grep '50664')
		if [[ $result1 != '' ]]
		then
			num=$(echo $result1 | awk '{print $1}')
			num=$(expr $num)
			if [[ $num -lt 100 ]]
			then
				echo -e "\033[32m $i \033[0m resubmit because $num 50664 failure"
		#		crab --quiet resubmit $i
				to_resubmit=1
			fi
		fi
		result3=$(echo $rows | grep '8002')
		if [[ $result1 != '' ]]
		then
			num=$(echo $result1 | awk '{print $1}')
			num=$(expr $num)
			if [[ $num -lt 100 ]]
			then
				echo -e "\033[32m $i \033[0m resubmit because $num 8002 failure"
		#		crab --quiet resubmit $i
				to_resubmit=1
			fi
		fi
		if [[ $to_resubmit -eq 1 ]]
		then
			crab resubmit $i
		fi
	done 
	rm -f tmp_report.out
#done
