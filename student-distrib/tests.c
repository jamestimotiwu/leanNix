#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#include "i8259.h"
// #include "drivers/rtc.h"
#include "drivers/terminal.h"
#include "drivers/keyboard.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
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
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
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
/* Checkpoint 2 tests */

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

    uint8_t buf[KB_BUF_SIZE];
    printf("Enter a string: ");
    int i = terminal_read(0, buf, KB_BUF_SIZE);
    buf[i] = '\0';
    printf("you entered: %s", buf);

    return PASS;
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){

	TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("test_divide_error", test_divide_error());
	// launch your tests here
	//test_interrupts();
	TEST_OUTPUT("syscall_test", syscall_test());

	TEST_OUTPUT("test_terminal_scrolling", test_terminal_scrolling());
    TEST_OUTPUT("test_terminal_read", test_terminal_read());
}
