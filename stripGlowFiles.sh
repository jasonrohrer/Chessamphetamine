mkdir tgaDump
mkdir tgaDump/settings

for file in settings/traditionalChineseFont.tga_strip*.glow; do
    echo "$file"
	tail -c +29 $file > tgaDump/$file.tga
done
