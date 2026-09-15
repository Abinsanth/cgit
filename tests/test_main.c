#include <stdio.h>

#include "test.h"

static int test_assertion(void)
{
    TEST_ASSERT(1 == 1);

    return 0;
}

int main(void)
{
    int failed = 0;

    printf("Running tests...\n");

    if (test_assertion() != 0)
    {
        failed++;
    }

    if (failed == 0)
    {
        printf("All tests passed.\n");
        return 0;
    }

    printf("%d test(s) failed.\n", failed);

    return 1;
}