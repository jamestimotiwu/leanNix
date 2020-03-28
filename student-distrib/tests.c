#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#include "i8259.h"
#include "page.h"
#include "drivers/fs.h"
// #include "drivers/rtc.h"

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

/* Test open file in file system
 *
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: paging
 */
int test_fs_open_good_file() {
	int err_code;
	err_code = fs_open((const uint8_t*) "frame0.txt");
	printf("Test file system open good file frame0.txt: %d \n", err_code);
	return 0;
}

/* Test open file in file system
 *
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: paging
 */
int test_fs_open_bad_file() {
	int err_code;
	err_code = fs_open((const uint8_t*) "frame10.txt");
	printf("Test file system open bad file frame10.txt: %d \n", err_code);
	return 0;
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
	//test_paging_null();
	//test_paging_kernel();
	//test_paging_out_kernel();
	test_fs_open_good_file();
	test_fs_open_bad_file();
	TEST_OUTPUT("syscall_test", syscall_test());
}
