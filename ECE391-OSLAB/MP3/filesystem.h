#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#define BUF_SPACE 1
#define DNUMS_OFFSET 1
#define FS_START_DOT 1
#define FS_START 2
#define INODE_DIV 4
#define BB_DIV 4
#define RESERVED_SIZE 24
#define NAME_SIZE 32
#define INUM_OFFSET 36
#define DIR_SIZE 64
#define	NUM_FILES 64
#define BLOCK_SIZE 4096
#define MAX_FSIZE 4194304


/* include dependencies below */
#include "lib.h"

typedef struct dentry_t {
	char file_name[NAME_SIZE];
	uint32_t file_type; 
	uint32_t inode_num;
	char reserved[RESERVED_SIZE];
} dentry_t;

/*Declaring member vars */
int8_t* boot_block_addr;

/*Functions */

extern void fs_init(int8_t* ptr);

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);


/* FS system specific OCRW Functions */
extern int32_t file_open(const uint8_t* filename);
extern int32_t file_close(int32_t fd);
extern int32_t file_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t file_write();

extern int32_t dir_open(const uint8_t* filename);
extern int32_t dir_close(int32_t fd);
extern int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t dir_write();

/* Public Testing Functions */
extern void print_filesystem();
extern int32_t print_file(const uint8_t* filename, uint32_t offset);



#endif /* _FILESYSTEM_H */
