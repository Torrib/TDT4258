
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/ioport.h>
#include <linux/types.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/signal.h>
#include <linux/interrupt.h>
#include <asm/siginfo.h>    //siginfo
#include <linux/rcupdate.h> //rcu_read_lock
#include <linux/sched.h>    //find_task_by_pid_type

#include "efm32gg.h"

#define NAME "gamepad1337"

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

//Prototypes
static int __init driver_init(void);
static void __exit driver_exit(void);
static int driver_open(struct inode *inode, struct file *filep);
static int driver_release(struct inode *inode, struct file *filep);
static ssize_t driver_read(struct file *filep, char __user *buff, size_t count, loff_t *offp);
static ssize_t driver_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp);
void write_register(uint32_t offset, uint32_t value);
uint32_t read_register(uint32_t offset);
static irqreturn_t irq_handler(int irq, void *dummy, struct pt_regs * regs);

//File operation functions
static struct file_operations driver_fops = {
    .owner = THIS_MODULE,
    .read = driver_read,
    .write = driver_write,
    .open = driver_open,
    .release = driver_release
};

//This driver
static struct cdev driver_cdev = {};

//Device number
dev_t devicenumber = 0;

//GPIO memory pointer
void __iomem *gpio;

char output;
/** Pid and info for the userspace registered program */
struct task_struct *t;

/** Class for userspace /dev/NAME file */
struct class *cl;

static int __init driver_init(void)
{
    //Get device number
    alloc_chrdev_region(&devicenumber, 0, 1, NAME);

    //Request and map memory
    check_mem_region(GPIO_BASE, GPIO_SIZE);
    request_mem_region(GPIO_BASE, GPIO_SIZE, NAME);

    gpio = ioremap_nocache(GPIO_BASE, GPIO_SIZE);

    //Set up GPIO
    write_register(GPIO_PA_CTRL, 0x2);
    write_register(GPIO_PA_MODEH, 0x55555555);
    write_register(GPIO_PA_DOUT, 0xFF00);
    write_register(GPIO_PC_MODEL, 0x33333333);
    write_register(GPIO_PC_DOUT, 0xFF);
    write_register(GPIO_IEN, 0xFF);
    write_register(GPIO_EXTIPSELL, 0x22222222);
    write_register(GPIO_EXTIRISE, 0xFF);
    write_register(GPIO_EXTIFALL, 0xFF);
    write_register(GPIO_IFC, 0xFFFF);

    request_irq(17, irq_handler, 0, NAME, NULL);
    request_irq(18, irq_handler, 0, NAME, NULL);

    //Register driver
    cdev_init(&driver_cdev, &driver_fops);
    driver_cdev.owner = THIS_MODULE;
    cdev_add(&driver_cdev, devicenumber, 1);

    printk(KERN_INFO "%s loaded... Major number is %d\n", NAME, MAJOR(devicenumber));


    /* Make the userpace driver file. */
    //Create file based on driver name
    cl = class_create(THIS_MODULE, NAME);
    device_create(cl, NULL, devicenumber, NULL, MAJOR);

    return 0;
}

static void __exit driver_exit(void)
{
    cdev_del(&driver_cdev);
    iounmap(gpio);
    release_mem_region(GPIO_BASE, GPIO_SIZE);
    unregister_chrdev_region(devicenumber, 1);
    free_irq(17, NULL);
    free_irq(18, NULL);
    printk(KERN_INFO "%s unloaded...", NAME);
}

static int driver_open (struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "%s opened\n", NAME);
    return 0;
}

static int driver_release (struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "%s closed\n", NAME);
    return 0;
}

static ssize_t driver_read (struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
    //Return button state
    if (count == 0)
        return 0;
    copy_to_user(buff, &output, 1);
    return 1;
}

static ssize_t driver_write (struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
    char pid_string[10];
    int pid = 0;
    int ret;

    /* read the value from user space */
    if(count > 10)
        return -EINVAL;
    copy_from_user(pid_string, buff, count);
    sscanf(pid_string, "%d", &pid);

    rcu_read_lock();
    t = pid_task(find_pid_ns(pid, &init_pid_ns), PIDTYPE_PID);
    if(t == NULL){
        printk("no such pid\n");
        rcu_read_unlock();
        return -ENODEV;
    }
    rcu_read_unlock();
    return count;
}

void write_register(uint32_t offset, uint32_t value)
{
    *(volatile uint32_t *) ((uint32_t) gpio + offset) = value;
}

uint32_t read_register(uint32_t offset)
{
    return *(volatile uint32_t *) ((uint32_t) gpio + offset);
}

static irqreturn_t irq_handler(int irq, void *dev_id, struct pt_regs * regs)
{
    uint32_t buttons = read_register(GPIO_PC_DIN);
    struct siginfo info;
    int ret;
    output = (uint8_t) ~buttons;
    /* send the signal */
    // memset(&info, 0, sizeof(struct siginfo));
    // info.si_signo = 42;
    // info.si_code = SI_QUEUE;
    // info.si_int = output;
    // ret = send_sig_info(42, &info, t);
    // if (ret < 0) {
    //  printk("error sending signal\n");
    //  return ret;
    // }

    printk(output);

    write_register(GPIO_IFC, 0xFFFF);
    return IRQ_HANDLED;
}

module_init(driver_init);
module_exit(driver_exit);

MODULE_DESCRIPTION("Gamepad driver");
MODULE_LICENSE("GPL");
