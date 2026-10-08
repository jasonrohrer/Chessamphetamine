ls -1 data/*.wav | grep _sd_ | sed "s/.*_sd_//" | sed "s/.wav//" | sort -h
