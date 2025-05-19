CXX = g++
CXXFLAGS = -I/usr/include/opencv4 -I./libs
LDFLAGS = -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lm

TARGET = output.out
SRCS = main.c libs/common.c libs/priority_queue.c libs/queue.c libs/image.c libs/image_usage.c libs/circular_list.c libs/crowd.c libs/config.c libs/logging.c libs/csv.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

safe: CXXFLAGS += -fsanitize=address -g
safe: LDFLAGS += -fsanitize=address
safe: clean $(TARGET)