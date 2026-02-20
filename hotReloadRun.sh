enableSetting="settings/maxigin_enableAutoQuit.ini"
autoSetting="settings/maxigin_autoQuit.ini"
quitFileName="settings/maxigin_autoQuitDone.ini"

echo "1" > $enableSetting
echo "0" > $autoSetting
echo "0" > $quitFileName

fileList="$quitFileName Chessamphetamine game.c maxigin.h mingin.h"

oldQuitModTime=$(stat -c %Y "$quitFileName")

./Chessamphetamine &
pid=$!

while inotifywait -e modify,create,delete $fileList; do
    echo "Watched files changed"

	newQuitModTime=$(stat -c %Y "$quitFileName")

	if (( newQuitModTime != oldQuitModTime )); then
		echo "Game itself quit, stopping hot-reload loop"
		wait "$pid"

		rm -f $enableSetting $autoSetting $quitFileName
		exit 0
	fi

	compOutput=$(./fastLinuxCompile.sh 2>&1)

	if [[ -n "$compOutput" ]]; then
		echo "compilation failed, not quitting/restarting game"
		echo "compilation output:"
		echo ""
		echo "$compOutput"
		echo ""
	else
		# tell it to quit
		echo "1" > $autoSetting

		# wait for it to quit
		wait "$pid"

		
		# rest quit flags for next run
		echo "0" > $autoSetting
		echo "0" > $quitFileName

		oldQuitModTime=$(stat -c %Y "$quitFileName")	
		
		./Chessamphetamine &
		pid=$!
	fi
 
done

echo "Hot reload loop done"

