#include "fs/operations.h"
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

void *really_long_func(){


	//int cycle = 0;
	char *path1 = "/f1";
	char *path2 = "/f2";
	char *path3 = "/f3";
	int f1, f2, f3;

	f1 = tfs_open(path1, TFS_O_CREAT);
	//assert(f1 != -1);

	f2 = tfs_open(path2, TFS_O_CREAT);
	//assert(f2 != -1);

	f3 = tfs_open(path3, TFS_O_CREAT);

	char str_long[BLOCK_SIZE * 1 + 1];

	memset(str_long + BLOCK_SIZE*0 , '0', BLOCK_SIZE * sizeof(char));

	str_long[1024] = '\0';

	//while (cycle < 1){


		/*memset(str_long + BLOCK_SIZE*1 , '1', BLOCK_SIZE * sizeof(char));
		memset(str_long + BLOCK_SIZE*2 , '2', BLOCK_SIZE * sizeof(char));
		memset(str_long + BLOCK_SIZE*3 , '3', BLOCK_SIZE * sizeof(char));
		memset(str_long + BLOCK_SIZE*4 , '4', BLOCK_SIZE * sizeof(char));
		memset(str_long + BLOCK_SIZE*5 , '5', BLOCK_SIZE * sizeof(char));
		memset(str_long + BLOCK_SIZE*6 , '6', BLOCK_SIZE * sizeof(char));
		memset(str_long + BLOCK_SIZE*7 , '7', BLOCK_SIZE * sizeof(char));
		memset(str_long + BLOCK_SIZE*8 , '8', BLOCK_SIZE * sizeof(char));
		memset(str_long + BLOCK_SIZE*9 , '9', BLOCK_SIZE * sizeof(char));*/

		//assert(tfs_init() != -1);


		//assert(f3 != -1);

		tfs_write(f3, "AAAA", 4); //4

		tfs_write(f1, "CCCC", 4);

		tfs_write(f2, "DDDD", 4);

		tfs_write(f3, str_long, strlen(str_long));

		tfs_write(f3, "BBBB", 4);

		tfs_write(f2, str_long, strlen(str_long));

		//cycle++;
	//}
	tfs_close(f1);
	tfs_close(f2);
	tfs_close(f3);

	return 0;
}


void loop_thread(int numThreads){
	pthread_t tid[numThreads];
	clock_t begin = clock();

    for (int i=0; i<numThreads; i++){
		if (pthread_create(&tid[i], NULL, really_long_func, NULL) != 0){
			fprintf(stderr, "Error: pthread_create error\n");
            exit(EXIT_FAILURE);
		}
	}


	for (int i=0; i<numThreads; i++){
		if (pthread_join(tid[i], NULL) != 0){
			fprintf(stderr, "Error: pthread_join error\n");
            exit(EXIT_FAILURE);
		}
	}
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("tfs executed in %f seconds\n", time_spent);
}
int main() {

	tfs_init();

	loop_thread(1);

	char *path3 = "/f3";
	tfs_copy_to_external_fs(path3, "test.txt");

    printf("Successful test.\n");


    return 0;
}