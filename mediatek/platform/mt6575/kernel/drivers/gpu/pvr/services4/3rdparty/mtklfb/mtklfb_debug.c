#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>

#include "img_defs.h"
#include "servicesext.h"
#include "kerneldisplay.h"
#include "mtklfb.h"

#ifdef MTK_DEBUG_LFB

static char USAGE[] =
    "\n"
    "USAGE\n"
    "        echo [ACTION]... > mtklfb\n"
    "\n"
    "ACTION\n";

static void do_command(const char *input)
{
    xlog_printk(ANDROID_LOG_ERROR, DRIVER_PREFIX, DRIVER_PREFIX ": invalid debug command...\n");
}

static void process_commands(char *input)
{
    char *ptr = NULL;

    xlog_printk(ANDROID_LOG_DEBUG, DRIVER_PREFIX, DRIVER_PREFIX ": %s\n", input);

    while ((ptr = strsep(&input, " ")) != NULL)
    {
        do_command(ptr);
    }
}


////////////////////////////////////////////////////////////////////////////////////

struct dentry *mtklfb_debugfs = NULL;
static char debug_buffer[2048];

static ssize_t debug_open(struct inode *inode, struct file *file)
{
    file->private_data = inode->i_private;
    return 0;
}

static ssize_t debug_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
    const int debug_bufmax = sizeof(debug_buffer) - 1;
    int n = scnprintf(debug_buffer, debug_bufmax, USAGE);
    debug_buffer[n++] = 0;
    return simple_read_from_buffer(ubuf, count, ppos, debug_buffer, n);
}

static ssize_t debug_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos)
{
    const int debug_bufmax = sizeof(debug_buffer) - 1;

    if (count > debug_bufmax)
        count = debug_bufmax;

    if (copy_from_user(&debug_buffer, ubuf, count))
        return -EFAULT;

    debug_buffer[count] = 0;

    process_commands(debug_buffer);

    return count;
}

static struct file_operations debug_fops = 
{
    .read  = debug_read,
    .write = debug_write,
    .open  = debug_open,
};

void dbg_init(void)
{
    mtklfb_debugfs = debugfs_create_file("mtklfb", S_IFREG|S_IRUGO, NULL, (void *)0, &debug_fops);
}

void dbg_exit(void)
{
    debugfs_remove(mtklfb_debugfs);
}

#endif
