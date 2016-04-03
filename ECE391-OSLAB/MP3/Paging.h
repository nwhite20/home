#ifndef _PAGING_H
#define _PAGING_H



/* 
 * enable_paging
 *   DESCRIPTION: Uses inlined assembly code to change cr4 and cr0 to enable paging
 *				  of the correct size and type (global+enabling extended paging)
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: enables paging (MUST BE CALLED AFTER THE
 *				 PAGE TABLE HAS BEEN MADE AND LOADED INTO CR3)
 */
extern void enable_paging();

/* 
 * make_page
 *   DESCRIPTION: Populates an already declared page directory (and table)
 *				  so they point to the correct places
 *   INPUTS: page_dir out page director as well as page_tab a page table for video memory
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Populates page directory and page table and loads them into cr0
 */
extern void  make_page(unsigned int * page_dir, unsigned int  * page_tab);

#endif /* _PAGING_H */
