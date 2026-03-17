if [ $# -ne 1 ]
then
	echo ""
	
	echo "Usage:"

	echo ""

	echo "./makeLinuxBuild.sh   version_number"

	echo ""

	echo "Example:"

	echo ""

	echo "./makeLinuxBuild.sh   17"

	echo ""

   exit
fi

echo "Logging into steamcmd as a sanity check..."

steamcmd +login "jasonrohrergames" +quit


folderName="Chessamphetamine_v${1}_linux"
zipName=${folderName}.zip

rm -rf $folderName

mkdir $folderName

rm -rf tempCheckout

mkdir tempCheckout

echo "Cloning..."

git clone .. tempCheckout/

cd tempCheckout

echo "Compiling..."

./hardcoreC89OptCompile.sh

cd ..

echo "Gathering files..."

cp tempCheckout/Chessamphetamine $folderName

cp -r tempCheckout/data $folderName

cp -r tempCheckout/settings $folderName

echo "Zipping..."

zip -r $zipName $folderName


echo "Gathering files for Steam build..."

rm -rf steamLinuxLatest

mkdir steamLinuxLatest

cp -r $folderName/* steamLinuxLatest

echo "Pushing build with steamcmd..."

steamcmd +login "jasonrohrergames" +run_app_build -desc Chessamphetamine_linux_v$1 /home/jasonrohrer/cpp/Chessamphetamine/build/app_build_linux_4109570.vdf +quit

