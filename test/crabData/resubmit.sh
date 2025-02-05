#!/bin/bash
files='crab_crab3_JJU_19Jan2025_*'
status_to_resubmit='7002 8001 8003 8020 10040 '
for i in $files
do
	echo -e "\033[32m $i \033[0m"
	crab status $i | sed -n -e '/jobs failed/p' > tmp_report.out
	cat report.out > tmp_report.out
	cat tmp_report.out | while read rows
	do	
		result=$(echo $rows | grep 'MINIAOD')
        isToResubmit=0
		if [[ $result != '' ]]
		then
			i=$rows
			echo -e "Checking \033[32m $i \033[0m"
		fi
		result1=$(echo $rows | grep '8020')
		if [[ $result1 != '' ]]
		then
			num=$(echo $result1 | awk '{print $1}')
			num=$(expr $num)
			if [[ $num -lt 100 ]]
			then
				echo -e "\033[32m $i \033[0m resubmit because $num 8020 failure"
				isToResubmit=1
			fi
		fi
        result2=$(echo $rows | grep '8001')
        if [[ $result2 != '' ]]
        then
            num=$(echo $result2 | awk '{print $1}')
            num=$(expr $num)
            if [[ $num -lt 100 ]]
            then
                echo -e "\033[32m $i \033[0m resubmit because $num 8001 failure"
                isToResubmit=1
            fi
        fi
        if [[ $isToResubmit -eq 1 ]]
        then
            crab resubmit --siteblacklist=T2_US_Purdue $i
        fi

	done 
	rm -f tmp_report.out
done
