#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#include "i8259.h"
#include "drivers/rtc.h"

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

/* RTC frequency test
 *
 * Instructions: enable test and write desired value in testFreq
 * Compares rtc at 1024HZ vs 2HZ
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: IDT, system call
 */
int rtc_frequency_test(){
	TEST_HEADER;
	int result = PASS;
	/* Test each frequency */
	int testFreq = 2;

	int out = rtc_write(0, &testFreq, 0);
	if(out == -1)
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
int rtc_read_test(){
	TEST_HEADER;
	int result = PASS;
	int x = rtc_read(0,0,0);
	if(x != 0){
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
int rtc_open_test(){
	TEST_HEADER;
	int result = PASS;
	int x = rtc_open(0);
	if(x != 0){
		result = FAIL;
	}
	return result;
}



/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("test_divide_error", test_divide_error());
	// launch your tests here
	//TEST_OUTPUT("syscall_test", syscall_test());
	/* RTC CP2 Tests */
	TEST_OUTPUT("rtc frequency test", rtc_frequency_test());
	//TEST_OUTPUT("rtc read test", rtc_read_test());
	//TEST_OUTPUT("rtc open test", rtc_open_test());
}
