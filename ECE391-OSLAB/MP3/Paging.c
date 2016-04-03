#include "Paging.h"


/*paging*/
#define small_page 1024 /*1 kb*/
#define pageby4 4096 /*1024*4 4 kb*/
/*bits as defined in os dev for enmabling paging*/
#define cr0enable 0x80000000
#define cr4enable 0x00000010

/*various flags all described in ULK*/
#define global_flag 0x100
#define dirty_flag 0x40
#define page_size_flag 0x80
#define accessed_flag 0x20
#define cache_flag 0x10 
#define write_through_flag 0x8
#define checkurprivaledge_flag 0x0
#define rwr_flag 0x2
#define present_flag 0x1
#define four_megabyte 0x00400000
/*global priveldge rwr and present all in one*/
#define allflag 0x183

/*video memory*/
#define vmemme 0xB8000

/*4mb kernel 4kb for video*/

extern void enable_paging(){
	int a=cr0enable;
	int b=cr4enable;
/*set cr4 and cr3 to the corresponding bits using inlined assembly*/
   asm (      "movl %%cr4, %%eax\n\t"
              "orl %1, %%eax\n\t"
              "movl %%eax, %%cr4\n\t"
              "movl %%cr0, %%eax\n\t"
              "orl %0, %%eax\n\t"
              "movl %%eax, %%cr0\n\t"

             : 
             : "r" (a), "r" (b) 
            :"eax"); 


/*first mov into register then into cr4 and so on and so on
cr4 MUST GO FIRST OTHERWISE THIS WILL NOT WORK must have extended paging before paging*/
	
}

extern void  make_page(unsigned int * page_dir, unsigned int  * page_tab){

	int i;
/*set all tabls and directories to read writable incase they must be overwritten (not necessary for those that arent prseent)*/
	for(i = 0; i < small_page; i++)
	{
    	page_dir[i] = rwr_flag;
	}

	for(i = 0; i < small_page; i++)
	{
    	page_tab[i] = rwr_flag;
	}

  /*set all the flags for the video memory we need to set these to kernel priveledge*/
		  int offs=vmemme/(pageby4);
      page_tab[offs]=vmemme; 
    	page_tab[offs] = page_tab[offs]|checkurprivaledge_flag;
    	page_tab[offs] = page_tab[offs] |rwr_flag;
    	page_tab[offs] = page_tab[offs] | present_flag;


/*2 is temporary for 3.1, We just init the vmeme and kernel. Kernel is at 1 and is extended paging.
vmem is in dir 0 at some offset*/
	for(i = 0; i < 2; i++) /*for this checkpoint we only need to init the page dir where vmem and kernel mem live
  thus we only loop through for two. Since I explained this is not a magic num.*/
	{
		if(i==0) { /*video memory lives at this one its less than four mb so in the first page*/
          page_dir[i]=(unsigned int)page_tab;
    		  page_dir[i] = page_dir[i]|checkurprivaledge_flag; 
    		  page_dir[i] = page_dir[i]|rwr_flag;
    		  page_dir[i] = page_dir[i]|present_flag;
  	  	}


  	 if(i==1) { /*lives in the second page does the kernel memory it lives from 4-8mb so the second page*/
  	  	  page_dir[i] = four_megabyte;
          page_dir[i]|=allflag;
          /*remember we index starting from 0 :)*/
  	  }

	}

/*unsigned int to hold our page dir pointer laod it into cr3 so cr3 points to our page directory*/
unsigned int q=(unsigned int)page_dir;
asm (	"movl %0, %%eax \n\t"
      "movl %%eax, %%cr3 \n\t"
             : 
             :"r" (q)        /* input */
             :"eax"); 



}




