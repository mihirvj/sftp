START=$(date +%s)
# do something
# start your script work here
ls -l
ping -c 4 www.google.com
# your logic ends here
END=$(date +%s)
DIFF=$(( $END - $START ))
echo "$START - $END = $DIFF" >> time.out
