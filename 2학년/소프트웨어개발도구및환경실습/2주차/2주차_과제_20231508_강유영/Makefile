cc=gcc
target = file
objects = main.o Count_Page.o Print_Page.o

$(target) : $(objects)
	$(cc) -o $(target) $(objects)

main.o Count_Page.o Print_Page.o : header.h

.PHONY : clean
clean :
	rm -f $(target) $(objects)

