reset                                                                           
set ylabel 'time(us)'
set style fill solid
set key center top 
set boxwidth 0.5
set title 'perfomance comparison'
set term png enhanced font 'Verdana,10'
set output 'runtime.png'

plot [0:4][:]'output.txt' every ::0::0 using 1:3:xtic(2) with boxes title '', \
'' every ::1::1 using 1:3:xtic(2) with boxes title '', \
'' every ::2::2 using 1:3:xtic(2) with boxes title ''
