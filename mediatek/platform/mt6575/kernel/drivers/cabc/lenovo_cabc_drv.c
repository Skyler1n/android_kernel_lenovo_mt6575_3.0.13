/**************************
*FileName: 	lenovo_cabc.c
*CopyRight:	lenovo
*Date:		2012-03-26
*Version:		0.1
****************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/string.h>


#include "lenovo_cabc_drv.h"
#include "lcm_drv.h"

#define CABC_DEBUG
#define MAX_CABC_STATE_LENGTH 20

static struct proc_dir_entry *cabc_proc_entry;
static char cabc_buffer[MAX_CABC_STATE_LENGTH + 1] = {"UI mode"};

extern LCM_DRIVER *lcm_drv;
extern struct semaphore sem_flipping;
extern struct semaphore sem_early_suspend;

static void cabc_log(char *src)
{
	int len = strlen(src) > MAX_CABC_STATE_LENGTH  ? MAX_CABC_STATE_LENGTH : strlen(src);
	memset(cabc_buffer, 0, MAX_CABC_STATE_LENGTH + 1);
       memcpy(cabc_buffer, src, len);
       cabc_buffer[MAX_CABC_STATE_LENGTH + 1] = 0;
}
static ssize_t cabc_read(char *page, char **start, off_t off, int count,
                        int *eof, void *data)
{
  		return scnprintf(page, PAGE_SIZE, "%s\n", cabc_buffer);		   
}
#if 0
static int  cabc_write_debug( struct file *file, const char *buffer, unsigned long count,
                                                                     void *data)
{
		printk("[cabc_debug] entry %s\n", __FUNCTION__);
		char inputnum[10] = {'\0'};
		int num;
		if (copy_from_user(inputnum, buffer, count)) {
			return -EFAULT;
		}
		if ( sscanf(inputnum, "%x", &num) == 1) {
			if (num == 0){
				//cabc_log("set to UI mode");
				mtkfb_set_cabcmode(0);
			}else if (num == 1){
				//cabc_log("set to MOVE mode");
				mtkfb_set_cabcmode(1);
			}else{
				cabc_log("invalid parameter");
			}
		}else{
			cabc_log("too many param");
		}
		return 0;
}
#endif
static int init_cabc_state(void)
{
        int ret = 0;
        if(!cabc_buffer){
                ret = -ENOMEM;
        }else{
        	memset(cabc_buffer, 0, MAX_CABC_STATE_LENGTH + 1);
            cabc_proc_entry = create_proc_entry("lenovo_cabcstate", 0666, NULL);
            if (cabc_proc_entry == NULL){
                ret = -ENOMEM;
                printk("[CABC_STATE], can not creat cabc preoc entry\n");
            }else{
                cabc_proc_entry->read_proc = cabc_read;
				//cabc_proc_entry->write_proc = cabc_write_debug;
                printk("[CABC_STATE], cabc_proc loaded\n");
            }
        }
        return ret;
}

static void exit_cabc_state(void)
{
        remove_proc_entry("cabcstate", &cabc_proc_entry);
        printk("[CABC_STATE], cabc exit\n");
}



/****cabc drive code below***************/

static int set_cabc_mode(unsigned mode)
{
	if (down_interruptible(&sem_flipping)) {
		printk("[FB Driver] can't get semaphore in mtkfb_set_cabcmode\n");
		return -1;
	}
	if (down_interruptible(&sem_early_suspend)) 		{
		printk("[FB Driver] can't get semaphore in mtkfb_set_cabcmode\n");
		return -1;
	}

	if (mode){
		lcm_drv->set_cabcmode(CABC_ON);
		cabc_log("CABC: move mode");
	}else{
		lcm_drv->set_cabcmode(CABC_OFF);
		cabc_log("CABC: UI mode");
	}
	up(&sem_early_suspend);
	up(&sem_flipping);

	return 0;
}
static ssize_t cabc_read_debug (struct file *file, char __user *src,  size_t len,  loff_t *offset)
{
	printk("[cabc] entry cabc_read_deug\n");
	static char cabc_debug_flag = 0;
	if (cabc_debug_flag == 0){
		set_cabc_mode(CABC_OFF);
		cabc_debug_flag = 1;
	}else{
		set_cabc_mode(CABC_ON);
		cabc_debug_flag = 0;
	}
	return 0;
}
static long cabc_ioctl (struct file *file, unsigned int cmd, unsigned long arg)
{
	printk("[cabc] enter cabc_ioctl, cmd = %x\n", cmd);
	int databuf = 0;
	switch (cmd)
	{
		case SET_CABC_CMD:
			if ( copy_from_user(&databuf, (void __user *)arg, 1)){
				printk("[cabc] ioctl copy from user failed\n");
			}
			printk("[cabc] copy from user success, databuf = %d\n", databuf);
			if (databuf)
			{
				set_cabc_mode(CABC_ON);
			}else{
				
				set_cabc_mode(CABC_OFF);
			}
			printk("[cabc] set cabc cmd success\n");
			break;
		case GET_CABC_CMD:
			break;
		default:
			printk("[cabc] ioctl: invalid cmd\n");
			break;

	}
	return 0;
}
static int __init lenovo_cabc_init(void)
{
	printk("[cabc] enter cabc module init\n");
	int ret = misc_register(&misc_cabc);
	if (ret){
		printk("[cabc] misc_register failed\n");
		return -1;
	}
	printk("[cabc] misc_register success\n");
	init_cabc_state();
	return 0;
}
static void __exit lenovo_cabc_exit(void)
{
	misc_deregister(&misc_cabc);
	exit_cabc_state();
}

module_init(lenovo_cabc_init);
module_exit(lenovo_cabc_exit);
MODULE_LICENSE("GPL");
