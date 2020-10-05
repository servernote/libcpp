CXX		=	g++
CC_DBG	+=	-g -O2
#STRIP	+=	-s
#CC_OPT	+=	-D_GNU_SOURCE -D_USE_PGSQL -D_USE_MYSQL
CC_OPT	+=	-D_GNU_SOURCE -D_USE_MYSQL
CC_OPT	+=	-DLINUX -D_REENTRANT -D_FORTIFY_SOURCE=2 -fPIC -DPIC -fstack-protector-strong
CC_OPT	+=	-Wformat -Werror=format-security -Wdate-time
CC_INC	+=	-I/usr/local/include
CXXFLAGS	+=	$(CC_DBG) $(CC_OPT) $(CC_INC)
SRCS	=	LFile.cpp LSocket.cpp LPgSQL.cpp LMySQL.cpp G.cpp LCgi.cpp
OBJS	=	${SRCS:.cpp=.o}
DESTLIB	=	/usr/local/lib
DESTINC	=	/usr/local/include

all:	.deps libcpp.a libcpp.so main.x

.deps:
	$(CXX) -M $(CXXFLAGS) $(SRCS) main.cpp > $@

libcpp.a:	$(OBJS)
	ar rv $@ $?
	ranlib $@

libcpp.so:	$(OBJS)
	$(CXX) -pthread -shared -Wl,--as-needed -Wl,-z -Wl,relro -Wl,-z -Wl,now -Wl,-soname -Wl,$@.1 -o $@.1.0 $^

# env LD_LIBRARY_PATH=`pwd` ./main.x
main.x:	main.cpp libcpp.so.1.0
	$(CXX) $(CXXFLAGS) -o $@ $^ -pthread -lm -lssl -lcrypto -lpq

clean:
	rm -f *.o *.a *.so* *.x .deps

install:  all
	install libcpp.a $(DESTLIB)
	install libcpp.so.1.0 $(DESTLIB)
	ldconfig $(DESTLIB)
	ln -f -s -r $(DESTLIB)/libcpp.so.1 $(DESTLIB)/libcpp.so
	install -m 0644 libcpp.h $(DESTINC)

-include .deps
