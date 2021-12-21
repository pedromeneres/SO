#include "fs/operations.h"
#include <assert.h>
#include <string.h>

int main() {

    //char *str1 = "AAAA!";
	//char *str2 = "BBB!";
    char *path1 = "/f1";
	char *path2 = "/f2";
	char *path3 = "/f3";
	char str_long[1024 * 3 + 1];
    char buffer[BLOCK_SIZE * 2];

	memset(str_long + BLOCK_SIZE*0 , '0', BLOCK_SIZE * sizeof(char));
	memset(str_long + BLOCK_SIZE*1 , '1', BLOCK_SIZE * sizeof(char));
	memset(str_long + BLOCK_SIZE*2 , '2', BLOCK_SIZE * sizeof(char));
	/*memset(str_long + BLOCK_SIZE*3 , '3', BLOCK_SIZE * sizeof(char));
	memset(str_long + BLOCK_SIZE*4 , '4', BLOCK_SIZE * sizeof(char));
	memset(str_long + BLOCK_SIZE*5 , '5', BLOCK_SIZE * sizeof(char));
	memset(str_long + BLOCK_SIZE*6 , '6', BLOCK_SIZE * sizeof(char));
	memset(str_long + BLOCK_SIZE*7 , '7', BLOCK_SIZE * sizeof(char));*/

	str_long[1024 * 3] = '\0';


	/*
	for(int i=0;i < 2001; i++){
		strcat(str_long, "x");
	}*/

    assert(tfs_init() != -1);

    int f1, f2, f3;
    ssize_t r;

    f1 = tfs_open(path1, TFS_O_CREAT);
    assert(f1 != -1);

	f2 = tfs_open(path2, TFS_O_CREAT);
    assert(f2 != -1);

	f3 = tfs_open(path3, TFS_O_CREAT);
    assert(f3 != -1);

	r = tfs_write(f3, "AAAA", 4);

	r = tfs_write(f1, str_long, strlen(str_long));

	r = tfs_write(f3, str_long, strlen(str_long));


/*
	r = tfs_write(f3, str_long, strlen(str_long));

	r = tfs_write(f3, str2, strlen(str2));
	r = tfs_write(f3, str1, strlen(str1));
	//printf("NOW GETTING IN\n");

	r = tfs_write(f2, str2, strlen(str2));
	r = tfs_write(f1, str1, strlen(str2));
	r = tfs_write(f2, str_long, strlen(str_long));*/

	print_fs_data(200000);

	print_inode_data_block(3);


    assert(tfs_close(f1) != -1);
	assert(tfs_close(f2) != -1);
	assert(tfs_close(f3) != -1);

    f1 = tfs_open(path1, 0);
    assert(f1 != -1);

	f2 = tfs_open(path2, 0);
    assert(f2 != -1);

	f3 = tfs_open(path3, 0);
    assert(f3 != -1);

    r = tfs_read(f3, buffer, sizeof(buffer) - 1);

    buffer[r] = '\0';


    assert(tfs_close(f1) != -1);
	assert(tfs_close(f2) != -1);
	assert(tfs_close(f3) != -1);


	/*TESTING OUT TFS_COPY_TO_EXTERNAL_FILE function*/
	FILE *fp;
	fp = fopen("test.txt", "w+");
	fprintf(fp, "%s", buffer);

	fclose(fp);

    printf("Successful test.\n");

    return 0;
}
