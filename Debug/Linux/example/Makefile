CC=gcc
CFLAGS += -g -I. -rdynamic  # 编译时加入-rdynamic可以防止strip将符号信息优化掉
LDFLAGS += -rdynamic        # 链接时只有加上-rdynamic才能将所有非静态函数加到动态符号表中,否在backtrace无法堆栈回溯
.PHONY: all

#要放第一行,makefile默认执行第一个target
all:test test_dynamic test_static libcalc.so libcalc.a mapstest signal
##############################################
mapstest:
	$(CC) maps_test.c -o mapstest
signal:
	$(CC) signal.c -o signal
##############################################
calc.o:
	$(CC) $(CFLAGS) -c calc.c -o calc.o

test.o:
	$(CC) $(CFLAGS) -c test.c -o test.o
	
test:test.o calc.o
	$(CC) $(LDFLAGS) test.o  calc.o -o test
##############################################	
libcalc.so:
	$(CC) -shared -fPIC -g calc.c -o libcalc.so   #addr2line需要加入-g
	
test_dynamic.o:
	$(CC) $(CFLAGS) -c test_dynamic.c -o test_dynamic.o

#加上-Wl,-rpath=.   后就不用在运行前export LD_LIBRARY_PATH了
test_dynamic:test_dynamic.o libcalc.so
	$(CC) $(LDFLAGS) test_dynamic.o -L. -lcalc  -Wl,-rpath=. -o test_dynamic

##############################################
#静态库只是把一堆.o文件打了个包
libcalc.a:calc.o
	ar rcs libcalc.a calc.o
	
test_static.o:
	$(CC) $(CFLAGS) -c test_static.c -o test_static.o
	
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
           signal


