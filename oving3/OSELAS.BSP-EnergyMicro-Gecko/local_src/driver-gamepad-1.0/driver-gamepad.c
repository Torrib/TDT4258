
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/kdev_t.h>
#include <linux/signal.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/ioport.h>
#include <linux/types.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>

#include <linux/device.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/siginfo.h>

#include "efm32gg.h"


#define NAME "gamepad"

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

static int __init my_driver_init(void);
static void __exit my_driver_exit(void);
static void write_register(uint32_t offset, uint32_t value);
static uint32_t read_register(uint32_t offset);
static int driver_open(struct inode *inode, struct file *filep);
static int driver_release(struct inode *inode, struct file *filep);
static ssize_t driver_read(struct file *filep, char __user *buff, size_t count, loff_t *offp);
static ssize_t driver_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp);
static irqreturn_t irq_handler(int irq, void *dummy, struct pt_regs * regs);

//File operation functions
static struct file_operations driver_fops = {
    .owner = THIS_MODULE,
    .read = driver_read,
    .write = driver_write,
    .open = driver_open,
    .release = driver_release
};

//Gamepad driver
static struct cdev gamepad_cdev = {};
dev_t devicenumber = 0;

void __iomem *gpio;
char output;
struct task_struct *task;
struct siginfo signal_info;
uint8_t driver_open = 0;

/** Class for userspace /dev/NAME file */
struct class *cl;

static int __init my_driver_init(void)
{
    //Get device number
    alloc_chrdev_region(&devicenumber, 0, 1, NAME);

    //Request and map memory
    check_mem_region(GPIO_BASE, GPIO_SIZE);
    request_mem_region(GPIO_BASE, GPIO_SIZE, NAME);

    //makes memory accessable
    gpio = ioremap_nocache(GPIO_BASE, GPIO_SIZE);

    //Set up GPIO

    write_register(gpio, GPIO_PA_CTRL, 0x2);
    //Set pins A8-15 to output
    write_register(gpio, GPIO_PA_MODEH, 0x55555555);
    //write_register(GPIO_PA_DOUT, 0xFF00);
    //write_register(GPIO_PC_MODEL, 0x33333333);
   //write_register(GPIO_PC_DOUT, 0xFF);

    //Enable interrupt generation
    write_register(gpio, GPIO_IEN, 0xFF);
    write_register(gpio, GPIO_EXTIPSELL, 0x22222222);

    //Trigger interrupt on button press
    write_register(gpio, GPIO_EXTIFALL, 0xFF);

    //Clear interrupts
    write_register(gpio, GPIO_IFC, 0xFFFF);

    //Enable interruption generation
    request_irq(17, irq_handler, 0, NAME, NULL);
    request_irq(18, irq_handler, 0, NAME, NULL);


    //Setup signal sending, to trigger interrupts in the game.
    memset(&signal_info, 0, sizeof(struct siginfo));
    signal_info.si_signo = 50;
    signal_info.si_code = SI_QUEUE;

    //Register driver
    cdev_init(&gamepad_cdev, &driver_fops);
    gamepad_cdev.owner = THIS_MODULE;
    cdev_add(&gamepad_cdev, devicenumber, 1);

    /* Make the userpace driver file. */
    //Create file based on driver name
    cl = class_create(THIS_MODULE, NAME);
    device_create(cl, NULL, devicenumber, NULL, NAME);

    printk(KERN_INFO "Gamepad driver started");

    return 0;
}

static void __exit my_driver_exit(void)
{
    //Cleanup drivers
    device_destroy(cl, devicenumber);
    class_destroy(cl);

    //Unregister driver
    cdev_del(&gamepad_cdev);

    //Release interrupt handlers
    free_irq(17, NULL);
    free_irq(18, NULL);

    //Release memory
    iounmap(gpio);
    release_mem_region(GPIO_BASE, GPIO_SIZE);
    unregister_chrdev_region(devicenumber, 1);


    printk(KERN_INFO "Gamepad driver unloaded");
}

void write_register(__iomem base, uint32_t offset, uint32_t value)
{
    *(volatile uint32_t *) ((uint32_t) base + offset) = value;
}

uint32_t read_register(__iomem base, uint32_t offset)
{
    return *(volatile uint32_t *) ((uint32_t) base + offset);
}

static int driver_open(struct inode *inode, struct file *filp)
{
    if(driver_open == 0) {
        driver_open++;
        return 0;
    }
    printk(KERN_INFO "%s driver_open\n", NAME);
    return 0;
}

static int driver_release(struct inode *inode, struct file *filp)
{
    driver_open--;
    printk(KERN_INFO "%s closed\n", NAME);
    return 0;
}

static ssize_t driver_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
    return count;
}

static ssize_t driver_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
    char pid_string[5];
    int pid = 0;

    if(count > 5)
        return -1;

    /* read the value from user space */

    //Copy input from userspace to local buffer
    copy_from_user(pid_string, buff, count);
    sscanf(pid_string, "%d", &pid);

    rcu_read_lock();
    task = pid_task(find_pid_ns(pid, &init_pid_ns), PIDTYPE_PID);
    if(task == NULL){
        printk("Error: Could not find the task with pid: %d\n", pid);
        rcu_read_unlock();
        return -1;
    }
    rcu_read_unlock();
    return count;
}

static irqreturn_t irq_handler(int irq, void *dev_id, struct pt_regs * regs)
{
    uint32_t buttons = read_register(GPIO_PC_DIN);
    signal_info.si_int = ~buttons;
	int ret = 0;
	write_register(GPIO_IFC, 0xFFFF);
	/* send the signal */
    if(driver_open)
        ret = send_sig_info(50, &signal_info, task);
    if (ret < 0) {
        printk("Cannot send signal...\n");
        return ret;
    }

	return IRQ_HANDLED;
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_DESCRIPTION("Gamepad driver");
MODULE_LICENSE("GPL");
