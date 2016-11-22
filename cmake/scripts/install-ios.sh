retcode=1
echo Install to $1 with $2
while [ $retcode -ne 0 ]; do
  sleep 1s
  xcrun simctl install $1 $2
  retcode=$?
done
