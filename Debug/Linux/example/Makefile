CC=gcc
CFLAGS += -funwind-tables -rdynamic -g
#LDFLAGS += -ldl -lrt -lpthread

.PHONY: all
#要放第一行,makefile默认执行第一个target

all:test test_dynamic test_static libcalc.so libcalc.a mapstest signal backtrace
##############################################
mapstest:
	$(CC) maps_test.c -o mapstest
signal:
	$(CC) signal.c -o signal
backtrace:
	$(CC) backtrace.c -rdynamic -g -o backtrace
##############################################
calc.o:
	$(CC) -c calc.c $(CFLAGS) -I . -o calc.o

test.o:
	$(CC) -c test.c $(CFLAGS) -I . -o test.o
	
test:test.o calc.o
	$(CC) test.o  calc.o -o test

##############################################	
libcalc.so:
	$(CC) -shared -fPIC calc.c -o libcalc.so
	
test_dynamic.o:
	$(CC) -c test_dynamic.c $(CFLAGS) -I . -o test_dynamic.o

test_dynamic:test_dynamic.o libcalc.so
	$(CC) test_dynamic.o -L . -lcalc -o test_dynamic

##############################################
#静态库只是把一堆.o文件打了个包
libcalc.a:calc.o
	ar rcs libcalc.a calc.o
	
test_static.o:
	$(CC) -c test_static.c $(CFLAGS) -I . -o test_static.o
	
#如果系统的搜索路径下同时存在静态链接库和动态链接库,默认情况下会链接动态链接库,如果需要强制链接静态链接库,需要加上“-static”选项
test_static:test_static.o libcalc.a
	$(CC) test_static.o -L . -static -lcalc -o test_static
	
cleantmp:
	rm -rf *.o

clean:
	rm -rf calc.o test.o test \
	       test_dynamic.o  libcalc.so  test_dynamic \
	       test_static.o libcalc.a test_static \
	       mapstest \
           signal  \
           backtrace


