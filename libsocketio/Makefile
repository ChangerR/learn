CPP = g++
CPPFLAGS = -I libwebsockets/linux-x64/include -c -g
LINK = g++
OBJS = websocket.o
LKFLAGS = -L libwebsockets/linux-x64/lib64 -lwebsockets -lpthread

test1: $(OBJS) test/test1.o
	$(LINK) -o $@ $(OBJS) test/test1.o $(LKFLAGS)

%.o:%.cpp
	g++ $(CPPFLAGS) -o $@ $<
