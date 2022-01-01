#include "fs/operations.h"
#include <assert.h>
#include <string.h>
#include <pthread.h>


int main() {

    //char *str1 = "AAAA!";
	//char *str2 = "BBB!";
    char *path1 = "/f1";
	char *path2 = "/f2";
	char *path3 = "/f3";
	char str_long[BLOCK_SIZE * 1 + 1];

	memset(str_long + BLOCK_SIZE*0 , '0', BLOCK_SIZE * sizeof(char));
	/*memset(str_long + BLOCK_SIZE*1 , '1', BLOCK_SIZE * sizeof(char));
	memset(str_long + BLOCK_SIZE*2 , '2', BLOCK_SIZE * sizeof(char));
	memset(str_long + BLOCK_SIZE*3 , '3', BLOCK_SIZE * sizeof(char));
	memset(str_long + BLOCK_SIZE*4 , '4', BLOCK_SIZE * sizeof(char));
	memset(str_long + BLOCK_SIZE*5 , '5', BLOCK_SIZE * sizeof(char));
	memset(str_long + BLOCK_SIZE*6 , '6', BLOCK_SIZE * sizeof(char));
	memset(str_long + BLOCK_SIZE*7 , '7', BLOCK_SIZE * sizeof(char));
	memset(str_long + BLOCK_SIZE*8 , '8', BLOCK_SIZE * sizeof(char));
	memset(str_long + BLOCK_SIZE*9 , '9', BLOCK_SIZE * sizeof(char));*/


	str_long[1024] = '\0';

    assert(tfs_init() != -1);

    int f1, f2, f3;

    f1 = tfs_open(path1, TFS_O_CREAT);
    assert(f1 != -1);

	f2 = tfs_open(path2, TFS_O_CREAT);
    assert(f2 != -1);

	f3 = tfs_open(path3, TFS_O_CREAT);
    assert(f3 != -1);

	tfs_write(f3, "AAAA", 4); //4

	tfs_write(f1, "CCCC", 4);

	tfs_write(f2, "DDDD", 4);

	tfs_write(f3, str_long, strlen(str_long)); //1028

	tfs_write(f3, "BBBB", 4); //1032

	tfs_write(f2, str_long, strlen(str_long));

	tfs_write(f3, str_long, strlen(str_long)); //

	//print_fs_data(200000);

	//print_inode_data_block(3);


    assert(tfs_close(f1) != -1);
	assert(tfs_close(f2) != -1);
	assert(tfs_close(f3) != -1);

    f1 = tfs_open(path1, 0);
    assert(f1 != -1);

	f2 = tfs_open(path2, 0);
    assert(f2 != -1);

	f3 = tfs_open(path3, 0);
    assert(f3 != -1);

	tfs_copy_to_external_fs(path1, "test.txt");



    assert(tfs_close(f1) != -1);
	assert(tfs_close(f2) != -1);
	assert(tfs_close(f3) != -1);

    printf("Successful test.\n");

    return 0;
}
