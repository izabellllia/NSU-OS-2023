guarded_editor: guarded_editor.c
	cc -v guarded_editor.c -o guarded_editor

PATH_TO_TEST_FILE = /tmp/test_file

create_test_file:
	echo "Important data" >> $(PATH_TO_TEST_FILE)

rm_test_file:
	rm $(PATH_TO_TEST_FILE)

set_advisory:
	chmod -l $(PATH_TO_TEST_FILE)

set_mandatory:
	chmod +l $(PATH_TO_TEST_FILE)
