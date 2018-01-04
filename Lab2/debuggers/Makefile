STUDENT_ID=2819130
FILES=Makefile bugs.c
ARCHIVE_FOLDER=$(STUDENT_ID)_eecs678_lab2
all: bugs valgrind_test

bugs: bugs.c
	gcc -g $^ -o $@

valgrind_test: valgrind_test.c
	gcc -g $^ -o $@

test: bugs
	-valgrind --leak-check=full --track-origins=yes ./bugs > bugs_output.txt 2> valgrind_output.txt
	@sed -i 's/^==[0-9]*==/==#==/' valgrind_output.txt
	@echo ""
	@echo "======================================="
	@echo "VALGRIND DIFF"
	-@diff -a valgrind_desired.txt valgrind_output.txt
	@echo "---------------------------------------"
	@echo ""
	@echo "======================================="
	@echo "PROGRAM DIFF"
	-@diff -a bugs_desired.txt bugs_output.txt

clean:
	#-rm bugs valgrind_test bugs_output.txt valgrind_output.txt
	rm -rf main *.o rm $(ARCHIVE_FOLDER) $(ARCHIVE_FOLDER).tar.gz

tar: clean
	mkdir $(STUDENT_ID)_eecs678_lab2
	cp -r $(FILES) $(ARCHIVE_FOLDER)
	tar cfzv $(ARCHIVE_FOLDER).tar.gz $(ARCHIVE_FOLDER)
	#-rm -rf $(STUDENT_ID)_eecs678_lab2

.PHONY: clean test
