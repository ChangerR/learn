CPP = g++
CPPFLAGS = -I libwebsockets/linux-x64/include -I . -c -g
LINK = g++
OBJS = websocket.o
LKFLAGS = -L libwebsockets/linux-x64/lib64 -lwebsockets -lpthread
TARGET = test1
test1: $(OBJS) test/test1.o
	$(LINK) -o $@ $(OBJS) test/test1.o $(LKFLAGS)

%.o:%.cpp
	g++ $(CPPFLAGS) -o $@ $<

clean:
	-rm *.o
	-rm $(TARGET)
