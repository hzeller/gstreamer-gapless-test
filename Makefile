BINARIES=test-loop-0.1 test-loop-1.0

all : $(BINARIES)

test-loop-0.1: test-loop.c
	gcc test-loop.c -o $@ `pkg-config --cflags --libs gstreamer-0.10`

test-loop-1.0: test-loop.c
	gcc test-loop.c -o $@ `pkg-config --cflags --libs gstreamer-1.0`

clean:
	rm -f $(BINARIES)
