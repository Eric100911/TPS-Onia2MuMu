#!/bin/bash
files='crab_crab3_*'
for i in $files
do
	#echo -e "\033[32m $i \033[0m"
	#crab status $i | sed -n -e '/jobs failed/p' > tmp_report.out
	cat report.out > tmp_report.out
	cat tmp_report.out | while read rows
	do	
		toresubmit=0
		result=$(echo $rows | grep 'MINIAOD')
		to_resubmit=0
		if [[ $result != '' ]]
		then
			i=$rows
			echo -e "Checking \033[32m $i \033[0m"
		fi
		#result1=$(echo $rows | grep '50664')
		result1=$(echo $rows | grep 'failed')
			num=$(expr $num)
			if [[ $num -lt 100 ]]
				#echo -e "\033[32m $i \033[0m resubmit because $num 50664 failure"
				echo -e "\033[32m $i \033[0m resubmit"
				crab --quiet resubmit $i
			fi
		fi
                result1=$(echo $rows | egrep '8901')
		if [[ $result1 != '' ]]
		then
			num=$(echo $result1 | awk '{print $1}')
			num=$(expr $num)
			if [[ $num -lt 100 ]]
			then
				echo -e "\033[32m $i \033[0m resubmit because $num 8901 failure"
				# crab --quiet --siteblacklist=T2_FR_GRIF resubmit $i
				toresubmit=1
			fi
		fi
		result2=$(echo $rows | grep '50664')
		if [[ $result2 != '' ]]
		then
			num=$(echo $result2 | awk '{print $1}')
			num=$(expr $num)
			if [[ $num -lt 100 ]]
			then
				echo -e "\033[32m $i \033[0m resubmit because $num 50664 failure"
				# crab --quiet --siteblacklist=T2_FR_GRIF resubmit $i
				toresubmit=1
			fi
		fi
		result2=$(echo $rows | grep 'postprocessing')
		if [[ $result2 != '' ]]
		then
			num=$(echo $result2 | awk '{print $1}')
			num=$(expr $num)
				echo -e "\033[32m $i \033[0m resubmit because $num postprocessing failure"
				# crab --quiet --siteblacklist=T2_FR_GRIF resubmit $i
				toresubmit=1
		fi
		if [[ $toresubmit -eq 1 ]]
		then
			crab --quiet resubmit --siteblacklist=T2_FR_GRIF,T2_US_Wisconsin $i
		fi
	done
	rm -f tmp_report.out
done
