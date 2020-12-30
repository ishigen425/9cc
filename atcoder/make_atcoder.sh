#!/bin/bash
echo "#!/bin/bash" > atcoder.sh
echo "echo -e \"" >> atcoder.sh
../9cc main.c >> atcoder.sh
echo "\" > tmp1.s" >> atcoder.sh
echo "cc -no-pie tmp1.s" >> atcoder.sh
echo "./a.out" >> atcoder.sh
