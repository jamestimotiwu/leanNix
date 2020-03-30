#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#include "i8259.h"
#include "drivers/rtc.h"
#include "page.h"
#include "drivers/fs.h"
// #include "drivers/rtc.h"
#include "drivers/terminal.h"
#include "drivers/keyboard.h"

#define PASS 1
#define FAIL 0

#define READ_BUFSIZE 1024

/* format these macros as you see fit */
#define TEST_HEADER     \
    printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)   \
    printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure() {
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test() {
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i) {
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)) {
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* Exception Handling Test
 *
 * Asserts if divide error exception is handled when dividing by 0
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Exception handling, IDT
 * Files: x86_desc.h/S, interrupts.c/h
 */
int test_divide_error() {
	TEST_HEADER;

	int result = PASS;
	int a = 1;
	int b = 0;
	a = a / b;
	assertion_failure();
	return result;
}

/* System call test
 *
 * Check that system calls work
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: IDT, system call
 */
int syscall_test() {
	TEST_HEADER;

	int result = PASS;

	/* should cause SYSTEM CALL! to print out */
	asm volatile ("int $0x80");

	return result;
}

/* Test paging
 *
 * Test dereference bad address ranges
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: paging
 */
int test_paging_null() {
	/* Test dereference null */
	int* null_test = NULL;
	int lvalue;
	lvalue = *null_test; /* Dereference pointer to null */

	return 0;
}

/* Test paging
 *
 * Test dereference good address ranges
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: paging
 */
int test_paging_kernel() {
	/* Test present page address range */
	int* present;
	int lvalue;

	/* Test lower bound address at 4MB */
	present = (int*)(MB_PAGE_SIZE);
	lvalue = *present;

	/* Test upper bound address of kernel at 8MB - 4*/
	present = (int*)(MB_PAGE_SIZE + MB_PAGE_SIZE - 4);
	lvalue = *present;

	return 0;
}

/* Test paging
 *
 * Test dereference bad address ranges
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: paging
 */
int test_paging_out_kernel() {
	/* Test dereference address range not present */
	int* not_present;
	int lvalue;

	/* Test lower bound address before 4MB */
	not_present = (int*)(MB_PAGE_SIZE - 4);
	lvalue = *not_present;

	/* Test upper bound address after 8MB */
	not_present = (int*)(MB_PAGE_SIZE + MB_PAGE_SIZE);
	lvalue = *not_present;

	return 0;
}

/* Checkpoint 2 tests */


/* RTC frequency test
 *
 * Instructions: enable test and write desired value in testFreq
 * Compares rtc at 1024HZ vs 2HZ
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: IDT, system call
 */
int rtc_frequency_test() {
	TEST_HEADER;
	int result = PASS;
	/* Test each frequency */
	int testFreq = 2;

	int out = rtc_write(0, &testFreq, 0);
	if (out == -1)
		result = FAIL;
	return result;
}

/* RTC read test
 *
 * Makes sure rtc_read blocks till next interrupt
 * Instructions: enable test and comment out flag setting in rtc_int
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: IDT, system call
 */
int rtc_read_test() {
	TEST_HEADER;
	int result = PASS;
	int x = rtc_read(0, 0, 0);
	if (x != 0) {
		result = FAIL;
	}
	return result;
}

/* RTC read test
 *
 * See if frequency is set to 2HZ when rtc_open is called
 * Instructions: set init frequency to something like 1024 and compare difference to test
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: IDT, system call
 */
int rtc_open_test() {
	TEST_HEADER;
	int result = PASS;
	int x = rtc_open(0);
	if (x != 0) {
		result = FAIL;
	}
	return result;
}

/* Test open file in file system
 *
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: paging
 */
int test_fs_open_good_file() {
	TEST_HEADER;

	if (fs_open((const uint8_t*)"frame0.txt") == 0)
		return PASS;
	return FAIL;
}

/* Test open file in file system
 *
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: paging
 */
int test_fs_open_bad_file() {
	TEST_HEADER;

	if (fs_open((const uint8_t*)"thisfiledoesn'texist.txt") == -1)
		return PASS;
	return FAIL;
}

/* Test reading from the file system
 *
 * Description: use a path that exists, print out all characters of file
 * Inputs: the file path
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: fs_read in fs.c
 */
int test_fs_read(char* fname) {
	TEST_HEADER;
	/* Very similar to ece391cat code */
	uint8_t buf[READ_BUFSIZE];
	int cnt, i;

	if (fs_open((const uint8_t*)fname) == -1)
		return FAIL;

	while (0 != (cnt = fs_read(0, buf, READ_BUFSIZE))) {
		if (-1 == cnt) {
			return FAIL;
		}

		// TODO: this doesn't work quite right because it isn't
		// merged with terminal drivers, so it sometimes prints over itself

		/* print all characters except null characters */
		for (i = 0; i < cnt; i++) {
			//if (buf[i] != '\0' && buf[i] != '\b')
			putc(buf[i]);
		}
	}

	printf("\n");

	return PASS;
}

/* Testing file name that isn't in directory
 *
 * Description: use a path that doesn't exist, make sure -1 is returned
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: read_dentry_by_name
 */
int test_dir_path_dne() {
	TEST_HEADER;
	dir_entry_t dentry;

	/* Check that this function works (it should return -1 */
	if (read_dentry_by_name((uint8_t*)"thisfiledoesn'texist", &dentry) == -1)
		return PASS;

	return FAIL;
}

#define LS_BUFSIZE FILENAME_CHAR_LIMIT

/* Test reading from "." ie. listing the directory
 *
 * Description: list files in the directory
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: prints to screen
 * Coverage: fs_read functionality for dir type
 */
int test_ls_dir() {
	TEST_HEADER;
	/* Very similar to ece391ls code */
	uint8_t buf[LS_BUFSIZE];
	int cnt, i;

	if (fs_open((const uint8_t*)".") == -1)
		return FAIL;

	// TODO: potential issue: verylargename file gets cut off one char

	while (0 != (cnt = fs_read(0, buf, LS_BUFSIZE))) {
		if (-1 == cnt) {
			return FAIL;
		}
		for (i = 0; i < cnt; i++) {
			putc(buf[i]);
		}
		/* print out newline */
		putc('\n');
	}

	return PASS;

/* test_terminal_scrolling
 *
 * Check that terminal driver will scroll screen
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: prints to screen
 * Coverage: terminal driver
 */
int test_terminal_scrolling() {
    TEST_HEADER;

    int i;
    /* test screen scrolling */
    for (i = 100; i < 105; i++) {
        printf("%d\n", i);
    }

    /* also test wrap around */
    for (i = 0; i < NUM_COLS*1.5; i++) {
        printf("%d", i%10);
    }
    printf("\n");

    return PASS;

}

/* test_terminal_read
 *
 * Check that terminal driver read function works
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: prints to screen
 * Coverage: terminal driver
 */
int test_terminal_read() {
    TEST_HEADER;

    uint8_t buf[KB_BUF_SIZE+1];
    printf("Enter a string: ");
    int i = terminal_read(0, buf, KB_BUF_SIZE);
    buf[i] = '\0';
    printf("you entered: %s", buf);

    return PASS;
}

/* test_terminal_write
 *
 * Check that terminal driver write function works
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: prints to screen
 * Coverage: terminal driver write
 */
int test_terminal_write() {
    TEST_HEADER;

    char str[] = "Hello, world!\n";
    printf("writing hello: ");
    terminal_write(0, str, sizeof(str));

    return PASS;

}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests() {
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("test_divide_error", test_divide_error());

	/* Paging tests */
	//test_interrupts();
	//test_paging_null();
	//test_paging_kernel();
	//test_paging_out_kernel();

	//TEST_OUTPUT("syscall_test", syscall_test());

	/* RTC CP2 Tests */
	//TEST_OUTPUT("rtc frequency test", rtc_frequency_test());
	//TEST_OUTPUT("rtc read test", rtc_read_test());
	//TEST_OUTPUT("rtc open test", rtc_open_test());

	/* Filesystem CP2 tests */
	//TEST_OUTPUT("test_fs_open_good_file", test_fs_open_good_file());
	//TEST_OUTPUT("test_fs_open_bad_file", test_fs_open_bad_file());
	//TEST_OUTPUT("test_ls_dir", test_ls_dir());
	//TEST_OUTPUT("test_dir_path_dne", test_dir_path_dne());
	//TEST_OUTPUT("test_fs_read", test_fs_read("frame0.txt"));
	//TEST_OUTPUT("test_fs_read", test_fs_read("grep"));
	//TEST_OUTPUT("test_ls_dir", test_ls_dir());
	// launch your tests here
	//test_interrupts();
	TEST_OUTPUT("syscall_test", syscall_test());

	TEST_OUTPUT("test_terminal_scrolling", test_terminal_scrolling());
    TEST_OUTPUT("test_terminal_write", test_terminal_write());
    TEST_OUTPUT("test_terminal_read", test_terminal_read());
}
