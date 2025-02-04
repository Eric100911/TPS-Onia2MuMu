#!/bin/bash
# A script to generate crab config file automaticly

# project config
dataList='Run2023dataList.txt'
template='crab3_template.py'
fileName='crab3'
scriptName='submit.sh'

# Allow parsing from user input in command line
# Use -l to specify the data list file
# Use -t to specify the template file
# Use -n to specify the file name header
# Use -s to specify the output script file for mass submission

while getopts "l:t:n:s:" opt; do
  case $opt in
    l)
      dataList=$OPTARG
      ;;
    t)
      template=$OPTARG
      ;;
    n)
      fileName=$OPTARG
      ;;
    s)
      scriptName=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done

rm -f ${scriptName}

cat $dataList | while read rows
do
	# tag1,2,3 are tags from CMS data naming standard
	# tag1: physics target, tag2: data set, tag3: data format
	tag1=$(echo $rows | awk 'BEGIN{FS="/"} {print $2}')
	tag2=$(echo $rows | awk 'BEGIN{FS="/"} {print $3}')
	tag3=$(echo $rows | awk 'BEGIN{FS="/"} {print $4}')
	# define tag format with varibale tag, it will show up at TaskTag in the config .py files and also in their filenames 
	tag1_=$(echo $tag1 | grep -o [0-9]) 
	tag2_=$(echo $tag2 | awk 'BEGIN{FS="-"} {print $1$3}')
	tag=${tag1_}_${tag2_}_$tag3

	sed -e 's:DataSet:'"$rows:" $template |\
	sed -e 's:TaskTag:'"${fileName}_${tag}:" \
    > ${fileName}_${tag}.py
	#mv ${fileName}_${tag}.py result/
    # Produce a shell script to submit the crab jobs
    echo "crab submit -c ${fileName}_${tag}.py &" >> ${scriptName}
done

