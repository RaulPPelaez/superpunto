ffmpeg -f gif -r 60  -i movie.gif -c:v libx264 -crf 4 -preset slow -c:a libfaac -b:a 192k -ac 2 -r 20 out.mp4

