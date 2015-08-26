LD=clang
CXX=clang++

CXXFLAGS += -Wall -pedantic -Werror -std=c++14 -stdlib=libc++
LDFLAGS += -lc++

CXXFLAGS += -Isrc/
LDFLAGS += -lcurl -lpq

SRC=
LIB=libhugopeixoto-memorel.a

OBJ=$(SRC:.cc=.o)
all: test $(LIB)

$(LIB): $(OBJ)
	$(AR) rcs $@ $^

test: main.o $(LIB)
	$(LD) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf test main.o $(OBJ) $(LIB)
