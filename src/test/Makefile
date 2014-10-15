VPATH= .. ../comm

MY_INC = -I../../include
MY_LIB = -L../../lib -lLiteNet

MYSQL_INC = 
MYSQL_LIB = 

ALL_INC = $(MY_INC) $(MYSQL_INC)
ALL_LIB = $(MY_LIB)  $(MYSQL_LIB)

BIN = Logger_test LiteList_test LiteTimer_test md5_test conf_ini_test conf_parse_test \
		channel_writer_test channel_reader_test
all: $(BIN)

# -g(-ggdb):生成调试信息
# -Wall: 警告信息
CFLAGS = -g -Wall #-lrt 

CC = gcc
CXX = g++

#定义make识别后缀
.SUFFIXES: .c .cpp .o .h

# 双后缀规则
# 自动化变量: $@ -- 所有的.o目标集
# 自动化变量：$< -- 所有的.c依赖目标集
.c.o:
	$(CC) -c $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB)
.cpp.o:
	$(CXX) -c $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB)

LiteList_test: LiteList_test.o
	$(CXX) $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB) 

LiteTimer_test: LiteTimer_test.o
	$(CXX) $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB) 

md5_test: md5_test.o
	$(CXX) $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB) 
Logger_test: Logger_test.o
	$(CXX) $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB) 
	
conf_ini_test: conf_ini_test.o
	$(CXX) $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB) 
	
conf_parse_test: conf_parse_test.o
	$(CXX) $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB) 

channel_writer_test: channel_writer_test.o
	$(CXX) $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB)
	
channel_reader_test: channel_reader_test.o
	$(CXX) $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB)		
	

.PHONY: clean
clean:
	@rm $(BIN) *.o 

