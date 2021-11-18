procSync: procSync.c
	  gcc -o procSync procSync.c -lpthread -lrt
clean:
	  rm procSync
run1:
	  ./procSync 1 1 1
run2:
	  ./procSync 0 0 0
run3:
	  ./procSync 10 10 10
run4:
	  ./procSync
run5:
	  ./procSync 1 2 3 4 5
