#pragma once
#include <linux/fs.h>
//#define LENOVO_CABC_NAME "Lenovo_cabc"
#define CABC_ON 1
#define CABC_OFF 0
#define SET_CABC_CMD _IOW('O', 10, unsigned int)
#define GET_CABC_CMD _IOR('O', 11, unsigned int)


static ssize_t cabc_read_debug (struct file *file, char __user *src,  size_t len,  loff_t *offset);
static long cabc_ioctl (struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations cabc_fops = {
	.owner = THIS_MODULE,
	.read = cabc_read_debug,
	.unlocked_ioctl = cabc_ioctl,
};
static struct miscdevice misc_cabc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "lenovo_cabc",//LENOVO_CABC_NAME,
	.fops = &cabc_fops,
	//.mode = 0666,
};


//extern int  lcm_set_cabcmode(int mode);
//extern int lcm_get_cabcmode(void);
//the data of semaphore were defined by mtkfg.c