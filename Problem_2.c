#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>

//global for data segment
int data_var = 11;
int bss_var;

void third_function_to_call_and_expand_stack(int param) {
    int func_var = param + 10;
    printf("The address of the first variable in the twice nested function call is: %p\n", &func_var);
    int func_var2 = func_var + 10;
    printf("The address of the second variable in the twice nested function call is: %p\n", &func_var2);
    return;
}

void another_function_to_call_and_expand_stack(int param) {
    int func_var = param + 10;
    printf("The address of a variable in the nested function call is: %p\n", &func_var);
    third_function_to_call_and_expand_stack(func_var);
    return;
}

void some_function_to_call_and_expand_stack() {
    int function_variable = 10;
    printf("The address of a variable in the first function call is: %p\n", &function_variable);
    another_function_to_call_and_expand_stack(function_variable);
    return;
}

int main(int argc, char **argv) {
    printf("\n----------TEXT SEGMENT--------------------\n");
    printf("The address of the main functin in the text segment is: %p\n", main);
    printf("The address of the auxiliary function (that's declared above main) in the text segment is: %p\n",third_function_to_call_and_expand_stack);

    printf("\n----------INITILIAZED DATA SEGMENT-------\n");
    printf("The address of the only variable in the initilized data segment is: %p\n", &data_var);

    printf("\n----------UNINITILIAZED DATA (BSS) SEGMENT-------\n");
    printf("The address of the only uninitilized data (bss) variable is: %p\n", &bss_var);

    printf("\n----------HEAP-----------------------------\n");
    char *test;
    test = malloc(100);
    printf("The address of the first dynamically allocated memory segment is: %p\n", test);
    //100mb allcation
    char* bigTest = malloc(100000000);
    printf("The starting address of the second and much bigger dynamically allocated memory segment is: %p\n", bigTest);
    printf("Now freeing the first allocation...\n");
    free(test);
    printf("Now allocating a smaller bit of memory than first allocation...\n");
    test = malloc(1);
    printf("The address of the small allocation is: %p\n", test);
    void* mmap_addr;
    mmap_addr = mmap(
                    NULL, 
                    1000, 
                    PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS,
                    0, 
                    0 );
    printf("The address of a small mmaped segment is: %p\n", mmap_addr);

    void* mmap_addr1;
    mmap_addr1 = mmap(
                    &test, 
                    1000, 
                    PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS,
                    0, 
                    0 );
    printf("The address of a small mmaped segment that I tried to map over the top of our earlier small allocation is: %p\n", mmap_addr1);
    // clean up
    munmap(mmap_addr, 1000);
    munmap(mmap_addr1, 1000);
    free(test);
    free(bigTest);

    printf("\n----------LINKED LIBRARIES-----------------\n");
    printf("The address of the sem_open functin linked from pthread is: %p\n", sem_open);
    printf("The address of the printf functin linked from the standard lib is: %p\n", printf);

    printf("\n----------ARGC AND ARGV-------\n");
    printf("The address of argc is: %p\n", &argc);
    printf("The address of the start of argv is: %p\n", argv);

    printf("\n----------STACK-------\n");
    int stack_var= -1;
    printf("The address of the first variable allocated (not argv or argc) on the stack is: %p\n", &stack_var);
    int stack_var2 = 25;
    printf("The address of the second variable allocated (not argv or argc) on the stack is: %p\n", &stack_var2);
    int stack_var3;
    printf("The address of the third variable allocated (not argv or argc) on the stack is: %p\n", &stack_var3);
    some_function_to_call_and_expand_stack();
    int stack_var4;
    stack_var4 = stack_var + stack_var2 * 20;
    printf("The address of the fourth (and post function call) variable allocated (not argv or argc) on the stack is: %p\n", &stack_var4);
    return 0;
}