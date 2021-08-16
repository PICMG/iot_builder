ifeq ($(OS),Windows_NT)
CC := cl.exe
LINK := link.exe
EXECUTABLE := builder.exe
LIBINCLUDES := ../../lib/include
LIBPATH := ../../lib
INCLUDES := .

OBJECTS := main.obj builder.obj CSpline.obj Interpolator.obj JsonArray.obj JsonFactory.obj JsonObject.obj JsonValue.obj
CXX_FLAGS := /EHsc /std:c++14 
build : clean $(OBJECTS)
	$(LINK) /OUT:$(EXECUTABLE) /DEBUG:FULL $(OBJECTS)
	rm Json*.*
	rm *.obj

%.obj : %.cpp
	$(CC) -c $(CXX_FLAGS) /Z7 $< /I $(LIBINCLUDES)

%.o : %.c
	$(CC) $(CXX_FLAGS) -c $< -I$(INCLUDES) -I$(LIBINCLUDES)

clean:
	-rm *.obj
	cp ../../lib/json/*.c* ./
else
CC := g++
LINK := g++
EXECUTABLE := builder
LIBINCLUDES := ../../lib/include
LIBPATH := ../../lib
INCLUDES := .
OBJECTS := main.o builder.o CSpline.o Interpolator.o $(LIBPATH)/json/libjson.a
CXX_FLAGS := -std=c++11 -ggdb
build : $(OBJECTS)
	$(LINK) -o $(EXECUTABLE) $(CXX_FLAGS) $(OBJECTS) -L$(LIBPATH)/json -ljson

%.o : %.cpp
	$(CC) $(CXX_FLAGS) -c $< -I$(INCLUDES) -I$(LIBINCLUDES)

%.o : %.c
	$(CC) $(CXX_FLAGS) -c $< -I$(INCLUDES) -I$(LIBINCLUDES)

%.a : 
	cd $(@D) && make clean
	cd $(@D) && make build

clean:
	-rm *.o
endif



