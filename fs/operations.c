#include "operations.h"
#include "pthread.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


int tfs_init() {
    state_init();

	/*test git pull command*/
    /* create root inode */
    int root = inode_create(T_DIRECTORY);
    if (root != ROOT_DIR_INUM) {
        return -1;
    }

    return 0;
}

int tfs_destroy() {
    state_destroy();
    return 0;
}

static bool valid_pathname(char const *name) {
    return name != NULL && strlen(name) > 1 && name[0] == '/';
}


int tfs_lookup(char const *name) {
    if (!valid_pathname(name)) {
        return -1;
    }

    // skip the initial '/' character
    name++;

    return find_in_dir(ROOT_DIR_INUM, name);
}

int tfs_open(char const *name, int flags) {
    int inum;
    size_t offset;

    /* Checks if the path name is valid */
    if (!valid_pathname(name)) {
        return -1;
    }

    inum = tfs_lookup(name);
    if (inum >= 0) {
        /* The file already exists */
        inode_t *inode = inode_get(inum);
        if (inode == NULL) {
            return -1;
        }

        /* Trucate (if requested) */
        if (flags & TFS_O_TRUNC) {
            if (inode->i_size > 0) {
                if (data_block_free(inode->i_data_block[inode->last_written_index]) == -1) {
                    return -1;
                }
                inode->i_size = 0;
            }
        }
        /* Determine initial offset */
        if (flags & TFS_O_APPEND) {
            offset = inode->i_size;
        } else {
            offset = 0;
        }
    } else if (flags & TFS_O_CREAT) {
        /* The file doesn't exist; the flags specify that it should be created*/
        /* Create inode */
        inum = inode_create(T_FILE);
        if (inum == -1) {
            return -1;
        }
        /* Add entry in the root directory */
        if (add_dir_entry(ROOT_DIR_INUM, inum, name + 1) == -1) {
            inode_delete(inum);
            return -1;
        }
        offset = 0;
    } else {
        return -1;
    }

    /* Finally, add entry to the open file table and
     * return the corresponding handle */
    return add_to_open_file_table(inum, offset);

    /* Note: for simplification, if file was created with TFS_O_CREAT and there
     * is an error adding an entry to the open file table, the file is not
     * opened but it remains created */
}

int tfs_close(int fhandle) { return remove_from_open_file_table(fhandle); }

ssize_t tfs_write(int fhandle, void const *buffer, size_t to_write) {

    open_file_entry_t *file = get_open_file_entry(fhandle);
    if (file == NULL) {
        return -1;
    }

    /* From the open file table entry, we get the inode */
    inode_t *inode = inode_get(file->of_inumber);
    if (inode == NULL) {
        return -1;
    }

	while (to_write > 0)
	{
		long unsigned int written = 0;
		/*If a file is empty or full*/
		if(file->of_offset == 0){
			/*Alloc a new data block for next index in i_data_block*/
			inode->i_data_block[inode->last_written_index] = data_block_alloc();
		}
		if(file->of_offset == BLOCK_SIZE){
			/*Alloc a new data block for next index in i_data_block*/
			inode->last_written_index++;
			inode->i_data_block[inode->last_written_index] = data_block_alloc();
			file->of_offset = 0;
		}

		void *block = data_block_get(inode->i_data_block[inode->last_written_index]);
		if (block == NULL) {
            return -1;
    	}

		if(to_write <= BLOCK_SIZE - file->of_offset){
			memcpy(block + file->of_offset, buffer, to_write);
       		/* The offset associated with the file handle is
         	* incremented accordingly */
        	file->of_offset += to_write;
        	if (file->of_offset > inode->i_size) {
            	inode->i_size = file->of_offset;
        	}else{
				inode->i_size += to_write;
			}
			return (ssize_t)to_write;
		}

		memcpy(block + file->of_offset, buffer + written, BLOCK_SIZE - file->of_offset);
		written += BLOCK_SIZE - file->of_offset;
		to_write -= written;
		/* The offset associated with the file handle is
        * incremented accordingly */
        file->of_offset += written;
        if (file->of_offset > inode->i_size) {
           	inode->i_size = file->of_offset;
        }else{
			inode->i_size += written;
		}
	}
	return (ssize_t)to_write;
}

ssize_t tfs_read(int fhandle, void *buffer, size_t len) {
    open_file_entry_t *file = get_open_file_entry(fhandle);
    if (file == NULL) {
        return -1;
    }

    /* From the open file table entry, we get the inode */
    inode_t *inode = inode_get(file->of_inumber);
    if (inode == NULL) {
        return -1;
    }

    /* Determine how many bytes to read */
    size_t to_read = inode->i_size;
    if (to_read > len) {
        to_read = len;
    }

	int i = 0;
	size_t read = BLOCK_SIZE;
	while(i <= inode->last_written_index){
		if (to_read > 0) {
        	void *block = data_block_get(inode->i_data_block[i]);
        	if (block == NULL) {
            	return -1;
        	}
        	/* Perform the actual read */
        	memcpy(buffer +  file->of_offset, block, read);
        	/* The offset associated with the file handle is
         	* incremented accordingly */
        	file->of_offset += read;
    	}
		i++;
	}
    return (ssize_t)to_read;
}

int tfs_copy_to_external_fs(char const *source_path, char const *dest_path){

	int inum;
	FILE *fp;
	ssize_t r;

    if (!valid_pathname(source_path)) {
        return -1;
    }

    inum = tfs_lookup(source_path);
    if (inum < 0){
        return -1;
	}

	inode_t *inode = inode_get(inum);

	int file = tfs_open(source_path, TFS_O_CREAT);

	char buffer[BLOCK_SIZE * (inode->last_written_index + 1)];

	r = tfs_read(file, buffer, sizeof(buffer) - 1);

    buffer[r] = '\0';

	fp = fopen(dest_path, "w+");
	fprintf(fp, "%s", buffer);

	fclose(fp);
	return 0;

}
