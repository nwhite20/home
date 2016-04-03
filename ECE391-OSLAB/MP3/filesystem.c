#include "filesystem.h"

/*
* extern void fs_init(int8_t* ptr);
*   Inputs: int8_t* ptr = pointer to start of the boot block
*   Return Value: none
*	Function: saves address of the boot block
*/
extern void fs_init(int8_t* ptr){
	boot_block_addr = ptr;
}

/*
* int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
*   Inputs: const uint8_t* fname = pointer to name of the file to read dentry for
*			dentry_t* dentry = dentry struct to read to
*   Return Value: 0 on success, -1 on fail
*	Function: reads directory entry information (by name of file) and places it 
*	into a struct we can use
*/
int32_t 
read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {

	int i = FS_START_DOT;
	int temp_len, f_len;
	f_len = strlen((int8_t*)fname);
	int8_t* temp_dir;
	int8_t temp_buf[NAME_SIZE+BUF_SPACE];

	// iterating through all files to find file with correct name
	while(i<NUM_FILES)
	{
		// getting current file name 
		temp_dir = (int8_t*)boot_block_addr+ i*DIR_SIZE;
		memcpy(temp_buf,temp_dir,NAME_SIZE);

		// file name doesn't have to be null terminated in system,
		// so have an extra space in a buffer to allow for 32-byte names
		// to have compatibility with lib functions that look for '\0'
		temp_buf[NAME_SIZE] = '\0';
		temp_len = strlen(temp_buf);

	// if file names aren't same length, move to next
		if(temp_len != f_len){ i++; continue;}
		if(!strncmp(temp_buf,(int8_t*)fname,temp_len))
		{
			// copy to struct if correct file
			memcpy(dentry,temp_dir,DIR_SIZE);
			return 0;
		}
		i++;
	}
	return -1;
}

/*
* int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
*   Inputs: uint32_t index = index of desired file
*			dentry_t* dentry = dentry struct to read to
*   Return Value: 0 on success, -1 on fail
*	Function: reads directory entry information (by index) and places it 
*	into a struct we can use
*/
int32_t 
read_dentry_by_index (uint32_t index, dentry_t* dentry) {
	int i = FS_START_DOT;
	int8_t num_dirs = *boot_block_addr;
	int8_t* temp_dir;

  if(index < num_dirs) {

    //move temp_dir to point to the proper directory struct
  	temp_dir = (int8_t*)boot_block_addr + (i+index)* DIR_SIZE;
  		
  	// copy to struct if correct index
  	memcpy(dentry,temp_dir,DIR_SIZE);

    return 0;
  }

  return -1; 
}

/*
* int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
*   Inputs: uint32_t inode = inode of file to read from
			uint32_t offset = offset from start of file to start reading at
			uint8_t* buf = buffer to place data read
			uint32_t length = amount of data to read
*   Return Value: Number of bytes read
*	Function: Places information read from the desired file into the given buffer
*/
int32_t 
read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
		// get num_inodes,dnodes from boot_block
		// and initializing remaining variable to argument
		unsigned int num_inodes = *((int8_t*)(boot_block_addr+BB_DIV)),num_dnodes = *((int8_t*)(boot_block_addr+2*BB_DIV)),copied = 0;
		int i,bytesread,leftinfile,dnode,start;
		int remaining = length;

		// setting pointers to start of inodes, dnodes
		int8_t* inode_ptr = boot_block_addr + BLOCK_SIZE;
		int8_t* dnode_ptr, *dnodes_strt = inode_ptr + BLOCK_SIZE*num_inodes;

		// returning error if improper
		if (offset+length > MAX_FSIZE) return -1;
		if ((inode < 0) || (inode >= num_inodes)) return -1;

		// getting to correct inode
		for (i=0;i<inode;i++) inode_ptr += BLOCK_SIZE;
		
		// getting amount left in the file after offset
		leftinfile = (*(int*)(inode_ptr)) - (int)offset;

		if ((leftinfile) < remaining) {

			remaining = leftinfile;

		}

		// saving return value
		bytesread = remaining;

		// moving to correct dnode and getting start offset in first
		inode_ptr += INODE_DIV*(offset/BLOCK_SIZE + DNUMS_OFFSET);
		start = offset % BLOCK_SIZE;

		while (remaining>0) {
				
				// get current dnode number 
				dnode = *((int*)(inode_ptr));

				// return error if invalid
				if ((dnode < 0) || (dnode >= num_dnodes)) return -1;

				dnode_ptr = dnodes_strt;

				// get to dnode
				for (i=0;i<dnode;i++) dnode_ptr += BLOCK_SIZE;

				// take care of starting at a point other than beginning
				if (start) {

						dnode_ptr += start;

						// extends past current data block
						if (start + length >= BLOCK_SIZE) {

								// read remaining in this block and decrement remaining
								memcpy(buf+copied,dnode_ptr,BLOCK_SIZE-start);
								copied += BLOCK_SIZE-start;
								remaining -= BLOCK_SIZE-start;
								start = 0;

						// doesn't extend past current data block
						} else {

								// finish reading
								memcpy(buf+copied,dnode_ptr,remaining);
								copied += remaining;
								remaining = 0;
							
						}

				} else {

						// limited to current data block
						if (remaining < BLOCK_SIZE) {

								// finish reading
								memcpy(buf+copied,dnode_ptr,remaining);
								copied += remaining;
								remaining = 0;

						} else {

								// read entire block
								memcpy(buf+copied,dnode_ptr,BLOCK_SIZE);
								copied += BLOCK_SIZE;
								remaining -= BLOCK_SIZE;

						}

				}

				// advancing to next dnode
				inode_ptr += INODE_DIV;
		}


	return copied;
}

/* Testing fxns */


/*
* int32_t print_filesystem();
*  Function: Prints all the names of the files in the file_sys directory.
*/
extern void 
print_filesystem(){
	int num_dirs = *boot_block_addr;
	
	int i;
	for(i=0; i < num_dirs; i++){
		dentry_t d;
		read_dentry_by_index(i, &d);
		printf("%s\n", d.file_name);
	}
}

/*
* print_file(const uint8_t* filename, uint32_t offset);
*   Inputs: const uint8_t* fname = pointer to name of the file to read dentry for
			uint32_t offset = offset from start of file to start reading at
*   Return Value: The file size in bytes
*	Function: Prints out a given file to the screen using the offset as a starting point.
*/
extern int32_t
print_file(const uint8_t* filename, uint32_t offset) {
	dentry_t d;
	read_dentry_by_name((uint8_t*)filename, &d);
	
	unsigned char buf[10000];
	int len;

	int i;
	for(i = offset; i < 10000; i++){
		if(read_data(d.inode_num,i, buf, 1) == 0) {
			len = i; 
			break;
		}
	}

	unsigned char len_buf[len+BUF_SPACE];
	read_data(d.inode_num, offset, len_buf, len);
	buf[len+BUF_SPACE] = '\0';
	
	printf("%s\n",len_buf); 

	return len;
}


/* OCWR Functions */


/*
* file_open(const uint8_t* filename)
*   Inputs: const uint8_t* filename = pointer to name of the file to read dentry for
*   Return Value: 0 for success and -1 for failure
*	Function: Creates fd struct and set up necessary information to handle file
*/
extern int32_t 
file_open(const uint8_t* filename) {
 	return 0; 
}

/*
* file_close(int32_t fd)
*   Inputs: int32_t fd = filedescriptor for the file we want to close
*   Return Value: 0 for success and -1 for failure
*	Function: Removes information for handling file from the fd struct
*/
extern int32_t 
file_close(int32_t fd){
	return 0; 
}

/*
* file_read(int32_t fd, void* buf, int32_t nbytes)
*   Inputs: int32_t fd = filedescriptor for the file we want to read
			void* buf =  pointer to the buffer we want to copy data to
			int32_t nbytes = number of bytes to read
*   Return Value: 0 for success and -1 for failure
*	Function: Reads data from a file given an fd
*/
extern int32_t 
file_read(int32_t fd, void* buf, int32_t nbytes){
	return 0;
}
/*
* file_write();
*   Return Value: 0 for success and -1 for failure
*	Function: Always returns -1 because our filesystem is read only
*/
extern int32_t 
file_write(){
	return -1;
}

/*
* dir_open(const uint8_t* filename)
*   Inputs: const uint8_t* filename = pointer to name of the file to read dentry for
*   Return Value: 0 for success and -1 for failure
*	Function: Creates fd struct and set up necessary information to handle dir
*/
extern int32_t 
dir_open(const uint8_t* filename){
	return 0;
}

/*
* dir_close(int32_t fd)
*   Inputs: int32_t fd = filedescriptor for the dir we want to close
*   Return Value: 0 for success and -1 for failure
*	Function: Removes information for handling dir from the fd struct
*/
extern int32_t 
dir_close(int32_t fd){
	return 0;
}

/*
* dir_read(int32_t fd, void* buf, int32_t nbytes)
*   Inputs: int32_t fd = filedescriptor for the dir we want to read
			void* buf =  pointer to the buffer we want to copy data to
			int32_t nbytes = number of bytes to read
*   Return Value: 0 for success and -1 for failure
*	Function: Reads data from a dir given an fd
*/
extern int32_t 
dir_read(int32_t fd, void* buf, int32_t nbytes){
	return 0;
}

/*
* dir_write();
*   Return Value: 0 for success and -1 for failure
*	Function: Always returns -1 because our filesystem is read only
*/
extern int32_t 
dir_write(){
	return -1;
}

