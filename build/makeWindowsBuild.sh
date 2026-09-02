if [ $# -ne 1 ]
then
	echo ""
	
	echo "Usage:"

	echo ""

	echo "./makeWindowsBuild.sh   version_number"

	echo ""

	echo "Example:"

	echo ""

	echo "./makeWindowsBuild.sh   17"

	echo ""

   exit
fi

buildLocation=VisualStudio/x64/Release/Chessamphetamine.exe

echo "Make sure latest build has been made with Visual studio."

echo "Expecting EXE to be here:  $buildLocation"

echo ""

echo "Logging into steamcmd as a sanity check..."

steamcmd +login "jasonrohrergames" +quit


folderName="Chessamphetamine_v${1}_windows"
zipName=${folderName}.zip

rm -rf $folderName

mkdir $folderName

rm -rf tempCheckout

mkdir tempCheckout


echo "Cloning..."

git clone .. tempCheckout/

cd tempCheckout




echo "Generating spriteCacheFingerprint.txt ..."

find data/*.tga *.h *.c -type f -print0 | sort -z | xargs -0 md5sum | md5sum | sed "s/ .*//" > data/spriteCacheFingerprint.txt


cd ..

echo "Gathering files..."

cp $buildLocation $folderName

cp -r tempCheckout/data $folderName

cp -r tempCheckout/settings $folderName



echo "Gathering files for Steam build..."

rm -rf steamWindowsLatest

mkdir steamWindowsLatest

cp -r $folderName/* steamWindowsLatest

echo "Pushing build with steamcmd..."

steamcmd +login "jasonrohrergames" +run_app_build -desc Chessamphetamine_windows_v$1 /home/jasonrohrer/cpp/Chessamphetamine/build/app_build_windows_4109570.vdf +quit

