VPATH=../comm ../conf

MY_INC = -I..
MY_LIB = 

MYSQL_INC = 
MYSQL_LIB = 

ALL_INC = $(MY_INC) $(MYSQL_INC)
ALL_LIB = $(MY_LIB)  $(MYSQL_LIB)

LIB = LiteNet

all: $(LIB)
	cp libLiteNet.a ../../lib/
	cp *.h ../../include/net
	cp ../comm/*.h ../../include/comm
	cp ../conf/*.h ../../include/conf
OBJ_DIR = ./obj

#-----------------------------------------------
# -g(-ggdb):���ɵ�����Ϣ
# -Wall: ������Ϣ
#-----------------------------------------------
CFLAGS = -lrt -g -Wall #-Fo $(OBJ_DIR)

CC = gcc
CXX = g++

#-------------------------------------------------
#����makeʶ���׺
#-------------------------------------------------
.SUFFIXES: .c .cpp .o .h

#-------------------------------------------------
# ˫��׺����
# �Զ�������: $@ -- ���е�.oĿ�꼯
# �Զ���������$< -- ���е�.c����Ŀ�꼯
#-------------------------------------------------
.c.o:
	$(CC) -c $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB)
	#$(CC) -c $(CFLAGS) -o $(OBJ_DIR)/$@ $^ $(ALL_INC) $(ALL_LIB)
	
.cpp.o:
	$(CXX) -c $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB)
	#$(CXX) -c $(CFLAGS) -o $(OBJ_DIR)/$@ $^ $(ALL_INC) $(ALL_LIB)

LiteNet: Timer.o md5.o Logger.o TimeUtil.o Channel.o ShmUtil.o Serializer.o \
		SocketAPI.o SocketInputStream.o SocketOutputStream.o Epoller.o ServerUtil.o \
		iniparser.o dictionary.o 
	ar -cr libLiteNet.a *.o

.PHONY: clean

clean:
	@rm $(BIN) *.o