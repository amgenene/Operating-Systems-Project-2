#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/cred.h>
#include <linux/utsname.h>
#include <linux/string.h>
#include <linux/sched.h>

//#include <linux/include/asm-generic/uaccess.h>
#include <linux/list.h>
unsigned long **sys_call_table;
struct ancestry {
	pid_t ancestors[10];
	pid_t siblings[100];
	pid_t children[100];
};
asmlinkage long (*ref_sys_cs3013_syscall2)(unsigned short *target_pid, struct ancestry *response);


asmlinkage long new_sys_cs3013_syscall2(unsigned short *target_pid, struct ancestry *response)
{
	unsigned short tar_pid[64];
	struct ancestry kresponse;
	struct task_struct *kcurrent;
	struct list_head *klist;
	int flag=0,i=0,j=0,k=0;
	printk(KERN_INFO "Made it into syscall2\n");
	if(copy_from_user(tar_pid,target_pid,sizeof(unsigned short))//||
	//copy_from_user(response,kresponse(sizeof(struct ancestry))))
	{
		printk(KERN_INFO "I messed up, Bytes not copied: \n");		
		return EFAULT;
	}
	printk(KERN_INFO "Made it past user copy\n");
	struct task_struct *pid_struct = pid_task(find_vpid(targ_pid), PIDTYPE_PID);
	if(flag!=2)
	{	
		printk(KERN_INFO "Checking Parent PID:%d\n",current->pid);		
		list_for_each_entry(pid_struct,&pid_struct->children)
		{
			
			struct task_struct *child;
			child = list_entry(pid_struct,struct task_struct,sibling);
				if(!list_empty(&child))
				{
					kresponse.children[i] = child->pid;
						i++;
				}
		list_for_each_entry(pid_struct,&pid_struct->children)
		{
			list_for_each_entry(pid_struct,&pid_struct->sibling)
			{
				struct task_struct *siblings;
				siblings = list_entry(pid_struct,struct task_struct,sibling);
				if(!list_empty(&child)){
					kresponse.siblings[j] = siblings->pid;
					j++;
				}
			}
		}
		
		list_for_each(pid_struct,&pid_struct->parent)
		{
			struct task_struct *child;
			child = list_entry(pid_struct,struct task_struct,sibling);
			printk(KERN_INFO "This is a child process!! PID:%d\n",current->pid);					
			kresponse.ancestor[k]=ancestors->pid;
			k++;
				}
			}
	}
			/*else
			{
				if(flag==1&&current->parent->pid==tar_pid)
				{
					printk(KERN_INFO "This is a child process!! PID:%d\n",child->pid);					
					kresponse.children[i]=child->pid;
					i++;
				}
			}
*/
			
		}
	}
	i=0;
	list_for_each(klist,&current->children)
	{
		struct task_struct *siblings;
		siblings = list_entry(klist,struct task_struct,sibling);
		printk(KERN_INFO "This is a sibling process!!PID:%d\n",siblings->pid);					
		kresponse.siblings[i]=siblings->pid;
	}
	i=0;	
	if(current->pid!=0)
	{
		kresponse.ancestors[i]=current->pid;
		printk(KERN_INFO "This is a ancestor process!!PID:%d\n",current->pid);					
		i++;
		kcurrent=current->parent;
  }
	kresponse.ancestors[i]=current->pid;
	printk(KERN_INFO "This is a ancestor process!!PID:%d\n",current->pid);
  if(copy_to_user(response,&kresponse,sizeof kresponse))
		return EFAULT;					
	return 0;
}

static unsigned long **find_sys_call_table(void) {
  unsigned long int offset = PAGE_OFFSET;
  unsigned long **sct;
  
  while (offset < ULLONG_MAX) {
    sct = (unsigned long **)offset;

    if (sct[__NR_close] == (unsigned long *) sys_close) {
      printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX",
	     (unsigned long) sct);
      return sct;
    }
    
    offset += sizeof(void *);
  }
  return NULL;
}

static void disable_page_protection(void) {
/*
Control Register 0 (cr0) governs how the CPU operates.
Bit #16, if set, prevents the CPU from writing to memory marked as
read only. Well, our system call table meets that description.
But, we can simply turn off this bit in cr0 to allow us to make
changes. We read in the current value of the register (32 or 64
bits wide), and AND that with a value where all bits are 0 except
the 16th bit (using a negation operation), causing the write_cr0
value to have the 16th bit cleared (with all other bits staying
the same. We will thus be able to write to the protected memory.
It’s good to be the kernel!
*/
	write_cr0 (read_cr0 () & (~ 0x10000));
}
static void enable_page_protection(void) {
/*
See the above description for cr0. Here, we use an OR to set the
16th bit to re-enable write protection on the CPU.
*/
	write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
  /* Find the system call table */
  if(!(sys_call_table = find_sys_call_table())) {
    /* Well, that didn't work. 
       Cancel the module loading step. */
    return -1;
  }

  ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];

//read and open
  /* Replace the existing system calls */
  disable_page_protection();

  //read and open funcitons
  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)new_sys_cs3013_syscall2;
 
  enable_page_protection();
 
  /* And indicate the load was successful */
  printk(KERN_INFO "Loaded interceptor!");

  return 0;
}
static void __exit interceptor_end(void) {
  /* If we don't know what the syscall table is, don't bother. */
  if(!sys_call_table)
    return;
 
  /* Revert all system calls to what they were before we began. */
  disable_page_protection();
  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
 
  enable_page_protection();

  printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);
