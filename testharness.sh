#!/usr/bin/env bash

test_result() {
	if [ "$1" -eq "0" ]; then
		echo "$2 (PASSED)"
	else
		echo "$2 (FAILED)"
	fi
}

# -------------------------------
# Number of test cases.
# update this number as needed.
# -------------------------------
N=12

for ((i = 1; i < N+1; i++)); do
    echo "----------------------------"
  	./testcase "tc$i"
  	ec=$?
  	test_result "$ec" "tc$i"
 	sleep 1
done

# ----------------------------------------------
# Honors section will included test cases below
# ---------------------------------------------- 

#testing our shell program behavior 

echo "--------------------------"

rm -f err.txt stdout.txt 

printf "ls faketestfile 2> err.txt\nexit\n" | ./lab02 > stdout.txt 

if [ -s err.txt ] && grep -q "cannot access" err.txt && ! grep -q "cannot access" stdout.txt; then 

    echo "honors stderr redirection (PASSED)"
else 
    echo "honors stderr redirection (FAILED)"
fi

rm -f err.txt stdout.txt 


# ---------------------------------------------- 
# Memory Leak Check 
# ----------------------------------------------
echo "--------------------------"

valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite --error-exitcode=1 ./testcase tc1  >/dev/null 2> vg_tc1.log
ec1=$?
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite --error-exitcode=1 ./testcase tc2  >/dev/null 2> vg_tc2.log
ec2=$?
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite --error-exitcode=1 ./testcase tc3  >/dev/null 2> vg_tc3.log
ec3=$?
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite --error-exitcode=1 ./testcase tc4  >/dev/null 2> vg_tc4.log
ec4=$?
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite --error-exitcode=1 ./testcase tc5  >/dev/null 2> vg_tc5.log
ec5=$?
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite --error-exitcode=1 ./testcase tc6  >/dev/null 2> vg_tc6.log
ec6=$?
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite --error-exitcode=1 ./testcase tc7  >/dev/null 2> vg_tc7.log
ec7=$?
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite --error-exitcode=1 ./testcase tc8  >/dev/null 2> vg_tc8.log
ec8=$?
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite --error-exitcode=1 ./testcase tc9  >/dev/null 2> vg_tc9.log
ec9=$?
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite --error-exitcode=1 ./testcase tc10 >/dev/null 2> vg_tc10.log
ec10=$?
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite --error-exitcode=1 ./testcase tc11 >/dev/null 2> vg_tc11.log
ec11=$?
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite --error-exitcode=1 ./testcase tc12 >/dev/null 2> vg_tc12.log
ec12=$?

if [ "$ec1" -eq 0 ] && [ "$ec2" -eq 0 ] && [ "$ec3" -eq 0 ] && [ "$ec4" -eq 0 ] && \
   [ "$ec5" -eq 0 ] && [ "$ec6" -eq 0 ] && [ "$ec7" -eq 0 ] && [ "$ec8" -eq 0 ] && \
   [ "$ec9" -eq 0 ] && [ "$ec10" -eq 0 ] && [ "$ec11" -eq 0 ] && [ "$ec12" -eq 0 ]; then
    echo "valgrind memory check (PASSED)"
else
    echo "valgrind memory check (FAILED)"
fi


# ---------------------------------------------- 
# Child Reap Checking 
# ----------------------------------------------

echo "--------------------------"

( printf "yes x | head -n 1 | wc -c\nexit\n" | ./lab02 ) &
shell_pid=$!
wait "$shell_pid"

if [ -d "/proc/$shell_pid/task/$shell_pid" ]; then
    children=$(cat /proc/$shell_pid/task/$shell_pid/children)
    if [ -z "$children" ]; then
        echo "child reap check (PASSED)"
    else
        echo "child reap check (FAILED)"
    fi
else
    echo "child reap check (PASSED)"
fi


# ---------------------------------------------- 
# File Descriptor Leak Check 
# ----------------------------------------------

echo "--------------------------"

rm -f fake_rep_out.txt fake_rep_in.txt
echo "hello" > fake_rep_in.txt

( printf "cat < fake_rep_in.txt | wc > fake_rep_out.txt\nsleep 1\nexit\n" | ./lab02 ) & shell_pid=$!

sleep 1

if [ -d "/proc/$shell_pid/fd" ]; then
    fd_count=$(ls "/proc/$shell_pid/fd" 2>/dev/null | wc -l)
    if [ "$fd_count" -le 3 ]; then
        echo "fd leak check (PASSED)"
    else
        echo "fd leak check (FAILED) open_fds=$fd_count"
    fi
else
    echo "fd leak check (PASSED)"
fi

wait "$shell_pid" 2>/dev/null

rm -f fake_rep_out.txt fake_rep_in.txt
