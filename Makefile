CPP_SRCS += \
src/client.cpp \
src/connection.cpp \
src/message.cpp \
src/message_queue.cpp \
src/server.cpp \
src/tcp_client.cpp \
src/tinymq.cpp \
src/durable_store.cpp

OBJS += \
src/client.o \
src/connection.o \
src/message.o \
src/message_queue.o \
src/server.o \
src/tcp_client.o \
src/tinymq.o \
src/durable_store.o

CPP_DEPS += \
src/client.d \
src/connection.d \
src/message.d \
src/message_queue.d \
src/server.d \
src/tcp_client.d \
src/tinymq.d \
src/durable_store.d

BOOST_PREFIX=/opt/local

LIBS := -L$(BOOST_PREFIX)/lib -lboost_thread-mt -lboost_system-mt -lboost_program_options-mt -lboost_filesystem-mt 
CFLAGS := -I$(BOOST_PREFIX)/include -Iextlib -O0 -gdwarf-2 -Wall 
all: tinymq

tinymq: $(OBJS)
	g++  -o "tinymq" $(OBJS) $(USER_OBJS) $(LIBS)

src/%.o: src/%.cpp
	@echo 'Building file: $<'
	g++ $(CFLAGS) -c  -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

clean:
	rm -Rf  $(OBJS)$(C++_DEPS)$(EXECUTABLES)$(CC_DEPS)$(C_DEPS)$(CPP_DEPS)$(CXX_DEPS)$(C_UPPER_DEPS) tinymq
	-@echo ' '

.PHONY: all clean dependents