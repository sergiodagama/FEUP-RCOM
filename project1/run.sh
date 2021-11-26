echo './receiver /dev/ttyS11'
echo './transmitter /dev/ttyS10' $1

xterm -hold -geometry 83x24-0+0 -fa 'Monospace' -fs 14 -title "Receiver" -e "echo 'Issuing command ./receiver /dev/ttyS11'; ./receiver /dev/ttyS11" &
sleep 1 && 
xterm -hold -geometry 84x24+0+0 -bg 'light grey' -fg black -fa 'Monospace' -fs 14 -title "Transmitter" -e "echo 'Issuing command ./transmitter /dev/ttyS10 $1'; ./transmitter /dev/ttyS10 $1" 

