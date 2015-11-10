rm tmp/plot*

ffmpeg -i $1 -ac 2 -filter_complex:a '[0:a]aresample=8000,asplit[l][r]' \
	-map '[l]' -c:a pcm_s16le -f data tmp/plot-waveform-ac1 \
	-map '[r]' -c:a pcm_s16le -f data tmp/plot-waveform-ac2

gnuplot plot.p
