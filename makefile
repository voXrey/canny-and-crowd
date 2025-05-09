CXX = g++
CXXFLAGS = -I/usr/include/opencv4 -I./libs
LDFLAGS = -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lm

TARGET = image_program.out
SRCS = image.c libs/priority_queue.c libs/queue.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)