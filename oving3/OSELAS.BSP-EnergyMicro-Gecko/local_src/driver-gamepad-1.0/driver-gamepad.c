
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
static void memwrite(void *base, uint32_t offset, uint32_t value);
static uint32_t memread(void *base, uint32_t offset);
static int driver_open(struct inode *inode, struct file *filep);
static int driver_release(struct inode *inode, struct file *filep);
static ssize_t driver_read(struct file *filep, char __user *buff, size_t count, loff_t *offp);
static ssize_t driver_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp);
static irqreturn_t interrupt_handler(int irq, void *dummy, struct pt_regs * regs);

static struct file_operations file_ops = {
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
uint8_t driver_enabled = 0;

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

    memwrite(gpio, GPIO_PA_CTRL, 0x2);
    //Set pins A8-15 to output
    memwrite(gpio, GPIO_PA_MODEH, 0x55555555);
    memwrite(gpio, GPIO_PA_DOUT, 0xFF00);

    //Sets pins 1-7 on PC to input
    memwrite(gpio, GPIO_PC_MODEL, 0x33333333);
    memwrite(gpio, GPIO_PC_DOUT, 0xFF);

    //Enable interrupt generation
    memwrite(gpio, GPIO_IEN, 0xFF);
    memwrite(gpio, GPIO_EXTIPSELL, 0x22222222);

    //Trigger interrupt on button press
    memwrite(gpio, GPIO_EXTIFALL, 0xFF);

    //Clear interrupts
    memwrite(gpio, GPIO_IFC, 0xFFFF);

    //Enable interruption generation
    request_irq(17, interrupt_handler, 0, NAME, NULL);
    request_irq(18, interrupt_handler, 0, NAME, NULL);


    //Setup signal sending, to trigger interrupts in the game.
    memset(&signal_info, 0, sizeof(struct siginfo));
    signal_info.si_signo = 50;
    signal_info.si_code = SI_QUEUE;

    //Register driver
    cdev_init(&gamepad_cdev, &file_ops);
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


    printk(KERN_INFO "Gamepad driver closed");
}

void memwrite(void *base, uint32_t offset, uint32_t value)
{
    *(volatile uint32_t *) ((uint32_t) base + offset) = value;
}

uint32_t memread(void *base, uint32_t offset)
{
    return *(volatile uint32_t *) ((uint32_t) base + offset);
}

static int driver_open(struct inode *inode, struct file *filp)
{
    if(driver_enabled == 0) {
        driver_enabled = 1;
        return 0;
    }
    printk(KERN_INFO "Unable to open gamepad driver";
    return 0;
}

static int driver_release(struct inode *inode, struct file *filp)
{
    driver_enabled = 0;
    printk(KERN_INFO "Gamepad driver closed");
    return 0;
}

static ssize_t driver_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
    return count;
}

static ssize_t driver_write(struct file *filp, const char __user *buffer, size_t count, loff_t *offp)
{
    char pid_array[5];
    int pid = 0;

    //return if the pid is to big
    if(count > 5)
        return -1;


    //Copy userspace data to the buffer
    copy_from_user(pid_array, buff, count);
    sscanf(pid_array, "%d", &pid);

    //Locks the RCU while writing
    rcu_read_lock();
    //Finds the task based on the PID
    task = pid_task(find_pid_ns(pid, &init_pid_ns), PIDTYPE_PID);
    if(task == NULL){
        printk("Error: Could not find the task with pid: %d\n", pid);
        rcu_read_unlock();
        return -1;
    }
    rcu_read_unlock();
    return count;
}

static irqreturn_t interrupt_handler(int irq, void *dev_id, struct pt_regs * regs)
{
    //Read the button status
    uint32_t buttons = memread(gpio, GPIO_PC_DIN);

    //Set the signal value to the reversed button value(Because they are active low)
    signal_info.si_int = ~buttons;

    //Resets the interrupt
	memwrite(gpio, GPIO_IFC, 0xFFFF);

	//Checks if everything is up and running and sends the signal to the game.
    if(driver_enabled)
    {
        int status = send_sig_info(50, &signal_info, task);
        if (status < 0) 
        {
            printk("Unable to send interrupt\n");
            return -1;
        }       
    }
    else
    {
        printk("Driver not enabled\n");
        return -1
    }

	return IRQ_HANDLED;
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_DESCRIPTION("Gamepad driver");
MODULE_LICENSE("GPL");
