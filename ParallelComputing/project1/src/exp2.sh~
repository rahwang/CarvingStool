touch time.txt
make
# run parallel overhead experiments
for x in 16 32 64 128 256 512 1024
do
    echo $x
    ./sample_m $x
    rm time.txt
    for i in `seq 0 9`
    do 
	./floyd_serial scratch.txt >> time.txt
    done
    python std_dev.py
    rm time.txt
    for i in `seq 0 9`
    do 
	./floyd_parallel 1 scratch.txt >> time.txt
    done
    python std_dev.py
done
