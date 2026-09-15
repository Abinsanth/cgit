#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#include "cli.h"
#include "test.h"
#include "repository.h"
#include "object.h"
#include "commit.h"
#include "index.h"

static int test_assertion(void)
{
    TEST_ASSERT(1 == 1);

    return 0;
}

static int test_unknown_command(void)
{
    char *argv[] = {
        "cgit",
        "unknown-command"};

    int result = cli_run(2, argv);

    TEST_ASSERT(result != 0);

    return 0;
}
static int test_missing_command_argument(void)
{
    char *argv[] = {
        "cgit"};

    int result = cli_run(1, argv);

    TEST_ASSERT(result != 0);

    return 0;
}

static int test_invalid_commit_arguments(void)
{
    char *argv[] = {
        "cgit",
        "commit"};

    int result = cli_run(2, argv);

    TEST_ASSERT(result != 0);

    return 0;
}

static int test_invalid_checkout_arguments(void)
{
    char *argv[] = {
        "cgit",
        "checkout"};

    int result = cli_run(2, argv);

    TEST_ASSERT(result != 0);

    return 0;
}

static int test_invalid_branch_name(void)
{
    char *argv[] = {
        "cgit",
        "branch",
        "bad name"};

    int result = cli_run(3, argv);

    TEST_ASSERT(result != 0);

    return 0;
}
static int enter_temp_directory(char *path, size_t path_size)
{
    snprintf(path, path_size, "/tmp/cgit-test-XXXXXX");

    if (mkdtemp(path) == NULL)
    {
        return 1;
    }

    if (chdir(path) != 0)
    {
        return 1;
    }

    return 0;
}

static int test_missing_head(void)
{
    char path[64];
    char branch[256];

    TEST_ASSERT(enter_temp_directory(path, sizeof(path)) == 0);

    TEST_ASSERT(repository_read_head(branch, sizeof(branch)) != 0);

    return 0;
}

static int test_missing_branch_reference(void)
{
    char path[64];
    char commit_id[CGIT_OBJECT_ID_SIZE];

    TEST_ASSERT(enter_temp_directory(path, sizeof(path)) == 0);

    TEST_ASSERT(mkdir(".cgit", 0755) == 0);
    TEST_ASSERT(mkdir(".cgit/refs", 0755) == 0);
    TEST_ASSERT(mkdir(".cgit/refs/heads", 0755) == 0);

    TEST_ASSERT(repository_read_branch(
                    "main",
                    commit_id,
                    sizeof(commit_id)) != 0);

    return 0;
}

static int test_missing_object(void)
{
    char path[64];
    unsigned char *data = NULL;
    size_t length = 0;

    TEST_ASSERT(enter_temp_directory(path, sizeof(path)) == 0);

    TEST_ASSERT(object_read_blob(
                    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
                    &data,
                    &length) != 0);

    free(data);

    return 0;
}

static int test_corrupted_head(void)
{
    char path[64];
    char branch[256];

    TEST_ASSERT(enter_temp_directory(path, sizeof(path)) == 0);

    TEST_ASSERT(mkdir(".cgit", 0755) == 0);

    FILE *file = fopen(".cgit/HEAD", "w");
    TEST_ASSERT(file != NULL);

    TEST_ASSERT(fprintf(file, "this is not a valid HEAD\n") > 0);

    fclose(file);

    TEST_ASSERT(repository_read_head(
                    branch,
                    sizeof(branch)) != 0);

    return 0;
}

static int test_corrupted_branch_reference(void)
{
    char path[64];
    char commit_id[CGIT_OBJECT_ID_SIZE];

    TEST_ASSERT(enter_temp_directory(path, sizeof(path)) == 0);

    TEST_ASSERT(mkdir(".cgit", 0755) == 0);
    TEST_ASSERT(mkdir(".cgit/refs", 0755) == 0);
    TEST_ASSERT(mkdir(".cgit/refs/heads", 0755) == 0);

    FILE *file = fopen(".cgit/refs/heads/main", "w");
    TEST_ASSERT(file != NULL);

    TEST_ASSERT(fprintf(file, "not-a-valid-commit-id\n") > 0);

    fclose(file);

    TEST_ASSERT(repository_read_branch(
                    "main",
                    commit_id,
                    sizeof(commit_id)) == 0);

    TEST_ASSERT(strcmp(commit_id, "not-a-valid-commit-id") == 0);

    return 0;
}
static int test_missing_commit_object(void)
{
    char path[64];
    char commit_id[CGIT_OBJECT_ID_SIZE];
    char tree_id[CGIT_OBJECT_ID_SIZE];

    TEST_ASSERT(enter_temp_directory(path, sizeof(path)) == 0);

    TEST_ASSERT(mkdir(".cgit", 0755) == 0);
    TEST_ASSERT(mkdir(".cgit/objects", 0755) == 0);
    TEST_ASSERT(mkdir(".cgit/refs", 0755) == 0);
    TEST_ASSERT(mkdir(".cgit/refs/heads", 0755) == 0);

    FILE *file = fopen(".cgit/refs/heads/main", "w");
    TEST_ASSERT(file != NULL);

    TEST_ASSERT(fprintf(
                    file,
                    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n") > 0);

    fclose(file);

    TEST_ASSERT(repository_read_branch(
                    "main",
                    commit_id,
                    sizeof(commit_id)) == 0);

    TEST_ASSERT(commit_read_tree(
                    commit_id,
                    tree_id,
                    sizeof(tree_id)) != 0);

    return 0;
}

static int test_empty_commit_message(void)
{
    char *argv[] = {
        "cgit",
        "commit",
        "-m",
        ""};

    int result = cli_run(4, argv);

    TEST_ASSERT(result != 0);

    return 0;
}
static int test_empty_file(void)
{
    char path[64];

    TEST_ASSERT(enter_temp_directory(path, sizeof(path)) == 0);

    char *init_argv[] = {
        "cgit",
        "init"};

    TEST_ASSERT(cli_run(2, init_argv) == 0);

    FILE *file = fopen("empty.txt", "w");
    TEST_ASSERT(file != NULL);
    fclose(file);

    char *argv[] = {
        "cgit",
        "add",
        "empty.txt"};

    int result = cli_run(3, argv);

    TEST_ASSERT(result == 0);

    return 0;
}
static int test_duplicate_index_entry(void)
{
    char path[64];

    TEST_ASSERT(enter_temp_directory(path, sizeof(path)) == 0);

    char *init_argv[] = {
        "cgit",
        "init"};

    TEST_ASSERT(cli_run(2, init_argv) == 0);

    FILE *file = fopen("test.txt", "w");
    TEST_ASSERT(file != NULL);

    fprintf(file, "first\n");
    fclose(file);

    char *add_argv[] = {
        "cgit",
        "add",
        "test.txt"};

    TEST_ASSERT(cli_run(3, add_argv) == 0);

    file = fopen("test.txt", "w");
    TEST_ASSERT(file != NULL);

    fprintf(file, "second\n");
    fclose(file);

    TEST_ASSERT(cli_run(3, add_argv) == 0);

    IndexEntry entries[100];

    int count = index_read_entries(entries, 100);

    TEST_ASSERT(count == 1);
    TEST_ASSERT(strcmp(entries[0].path, "test.txt") == 0);

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

    if (test_unknown_command() != 0)
    {
        failed++;
    }

    if (test_missing_command_argument() != 0)
    {
        failed++;
    }

    if (test_invalid_commit_arguments() != 0)
    {
        failed++;
    }

    if (test_invalid_checkout_arguments() != 0)
    {
        failed++;
    }

    if (test_invalid_branch_name() != 0)
    {
        failed++;
    }

    if (test_missing_head() != 0)
    {
        failed++;
    }

    if (test_missing_branch_reference() != 0)
    {
        failed++;
    }

    if (test_missing_object() != 0)
    {
        failed++;
    }
    if (test_corrupted_head() != 0)
    {
        failed++;
    }
    if (test_corrupted_branch_reference() != 0)
    {
        failed++;
    }
    if (test_missing_commit_object() != 0)
    {
        failed++;
    }
    if (test_empty_commit_message() != 0)
    {
        failed++;
    }
    if (test_empty_file() != 0)
    {
        failed++;
    }
    if (test_duplicate_index_entry() != 0)
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