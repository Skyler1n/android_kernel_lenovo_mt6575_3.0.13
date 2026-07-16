
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/oom.h>
#include <linux/sched.h>
#include <linux/notifier.h>

#include <linux/slab.h> 
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <linux/file.h>
#include <linux/uio.h>
//#include <linux/smp_lock.h>
#include <linux/fsnotify.h>
#include <linux/security.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/pagemap.h>
#include <linux/splice.h>
//#include <trace/fs.h>
#include "read_write.h"

#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <linux/statfs.h>
#include <linux/fs_struct.h>
#include <linux/mount.h>
#include <linux/timer.h>


typedef struct 
{
	int    precessID;
	char*	  processName;
}TSystemAppTable;

static long lowmem_threshold_size = (1048576);

static char* SystemAppPackage[] =
{
    "system_process",
    "ndroid.systemui",
    "d.process.acore",
    "m.android.phone",
    "ndroid.settings",    
    "d.process.media",
    "ovo.appsmanager",
    "system_server",
    "ackageinstaller",
    "lenovo.launcher"
    //{UNDEFINE_PROCESS_ID, "iatek.bluetooth"},
    //{UNDEFINE_PROCESS_ID, "om.mediatek.vlw"},
    //{UNDEFINE_PROCESS_ID, "dexopt"},    
    //{UNDEFINE_PROCESS_ID, "gsm0710muxd"},
    //{UNDEFINE_PROCESS_ID, "rild"},
};

static int g_nSystemAppPackageCount = 10;


bool isCanWrite(struct file *file)
{
    bool bResult = true, bIsLowMem = false; 
    //printk("lenovo get data file->f_op->write = %d, d_name.name = %s\n", file->f_op->write, file->f_path.mnt->mnt_mountpoint->d_name.name);
    //return true;
    //printk("lenovo isCanWrite enter comm = %s;d_name.name = %s\n", current->group_leader->comm, file->f_path.mnt->mnt_mountpoint->d_name.name);
	if(memcmp(file->f_path.mnt->mnt_mountpoint->d_name.name, "data", 4) == 0)
    {
        int i = 0, writepetr = 0;
        char *currentComm = current->group_leader->comm;
        struct kstatfs sbuf;
        
        if ((vfs_statfs(&(file->f_path), &sbuf)) == 0)
        {
            u64 freemem = sbuf.f_bsize * sbuf.f_bfree;
            //printk("lenovo get data freemem = %llu\n", freemem);

            if(freemem < lowmem_threshold_size)
            {
                bIsLowMem = true;
            }
        }

        //bIsLowMem = true;

        if(bIsLowMem)
        {
            int currentPPID = current->real_parent->pid;
            
            if((currentPPID != 0) && (currentPPID != 1) && (currentPPID != 2))
            {
                for(i=0; i<g_nSystemAppPackageCount; i++)
                {                    
                    char* appPackage = SystemAppPackage[i];
                    int len = strlen(appPackage);

                    if(memcmp(currentComm, appPackage, len) == 0)
                    {
                        break;
                    }
                }
            }
        }
        
        if(i >= g_nSystemAppPackageCount)
        {
            //printk("lenovo get data currentComm = %s, currentPPID = %d failed\n", currentComm, current->real_parent->pid);
            bResult = false;
        }
    }
    
    return bResult;
}

static struct file_operations	*lenovo_file_operations = 0;
static struct file_operations	original_file_operations = {0};

ssize_t lenovo_file_write(struct file *filp, const char __user *buf, size_t len, loff_t *ppos)
{
    if(!isCanWrite(filp))
    {
        return -ENOSPC;
    }

    return original_file_operations.write(filp, buf, len, ppos);
}

ssize_t lenovo_file_aio_write(struct kiocb *iocb, const struct iovec *iov,
		unsigned long nr_segs, loff_t pos)
{
    if(!isCanWrite(iocb->ki_filp))
    {
        return -ENOSPC;
    }

    return original_file_operations.aio_write(iocb, iov, nr_segs, pos);
}

static int __init lowmemdetect_init(void)
{
    struct file *myfile;
    myfile = filp_open("/data/abcdef", O_WRONLY | O_CREAT, 0);

    if (!IS_ERR(myfile) && (myfile->f_op != NULL))
    {
        //printk("lenovo myfile->f_op->write = %d \n", myfile->f_op->write);
        original_file_operations.write = myfile->f_op->write;
        original_file_operations.aio_write = myfile->f_op->aio_write;

        lenovo_file_operations = myfile->f_op;
        lenovo_file_operations->write = lenovo_file_write;
        lenovo_file_operations->aio_write = lenovo_file_aio_write;
        //printk("lenovo write = %d lenovo_file_write = %d\n", myfile->f_op->write, lenovo_file_write);
        filp_close(myfile, NULL);
    }
	
	return 0;
}

static void __exit lowmemdetect_exit(void)
{
    if(lenovo_file_operations != NULL)
    {
        lenovo_file_operations->write = original_file_operations.write;
        lenovo_file_operations->aio_write = original_file_operations.aio_write;
    }
}

module_param_named(lowmem_threshold, lowmem_threshold_size, long, S_IRUGO | S_IWUSR);
module_param_array_named(systemapp, SystemAppPackage, charp, &g_nSystemAppPackageCount,
			 S_IRUGO | S_IWUSR);
module_param_named(systemapp_count, g_nSystemAppPackageCount, int, S_IRUGO | S_IWUSR);
module_param_named(original_file_writeP, original_file_operations.write, int, S_IRUGO | S_IWUSR);


module_init(lowmemdetect_init);
module_exit(lowmemdetect_exit);

MODULE_LICENSE("GPL");

