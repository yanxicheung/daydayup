TEST_TARGET=test
TEST_DYNAMIC_TARGET=test_dynamic
TEST_STATIC_TARGET=test_static
LIB_SO_TARGET=libadd.so
LIB_TARGET=libadd.a

CC=gcc
CFLAGS += -funwind-tables -rdynamic
LDFLAGS += -ldl -lrt -lpthread

all: $(TEST_TARGET) $(TEST_DYNAMIC_TARGET) $(TEST_STATIC_TARGET)

$(TEST_TARGET):test.c
	$(CC) $(CFLAGS)
	@echo "build test finish"

clean:
	rm -rf $(TEST_TARGET) $(TEST_DYNAMIC_TARGET) $(TEST_STATIC_TARGET)


