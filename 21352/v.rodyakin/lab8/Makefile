COMPILER=cc
COMPILE_FLAGS=-v
TEST_FILE_PATH=/tmp/rodyakin_lab8_testfile
text_editor_guard.out: text_editor_guard.c
	$(COMPILER) $(COMPILE_FLAGS) text_editor_guard.c -o text_editor_guard.out

create_test_file:
	echo "test string" >> $(TEST_FILE_PATH)

delete_test_file:
	rm -f $(TEST_FILE_PATH)

set_lock_to_advisory:
	chmod -l $(TEST_FILE_PATH)

set_lock_to_mandatory:
	chmod +l $(TEST_FILE_PATH)

clean:
	rm text_editor_guard.out
