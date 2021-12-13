#include "fs/operations.h"
#include <assert.h>
#include <string.h>

int main() {

    char *str1 = "AAA!";
	char *str2 = "BBB!";
    char *path1 = "/f1";
	char *path2 = "/f2";
	char *path3 = "/f3";
	char str_long[1026];
    char buffer[40];

	for(int i=0;i < 1024; i++)
		strcat(str_long, "x");

    assert(tfs_init() != -1);

    int f1, f2, f3;
    ssize_t r;

    f1 = tfs_open(path1, TFS_O_CREAT);
    assert(f1 != -1);

	f2 = tfs_open(path2, TFS_O_CREAT);
    assert(f2 != -1);

	f3 = tfs_open(path3, TFS_O_CREAT);
    assert(f3 != -1);

    r = tfs_write(f1, str1, strlen(str1));
	r = tfs_write(f2, str2, strlen(str2));
	r = tfs_write(f2, str_long, strlen(str_long));
	r = tfs_write(f3, str1, strlen(str1));
	r = tfs_write(f3, str1, strlen(str1));

    assert(tfs_close(f1) != -1);
	assert(tfs_close(f2) != -1);
	assert(tfs_close(f3) != -1);


    f1 = tfs_open(path1, 0);
    assert(f1 != -1);

	f2 = tfs_open(path2, 0);
    assert(f2 != -1);

    r = tfs_read(f2, buffer, sizeof(buffer) - 1);

    buffer[r] = '\0';

    assert(tfs_close(f1) != -1);
	assert(tfs_close(f2) != -1);


    printf("Successful test.\n");

    return 0;
}
