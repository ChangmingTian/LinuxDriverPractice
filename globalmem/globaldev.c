#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#include "globaldev.h"


static int globalmem_major = GLOBALMEM_MAJOR;
struct globalmem_dev *globalmem_devp;

static struct class *globalmem_class;
static struct device *globalmem_class_dev;

int devno = 0;


static void create_dev(struct globalmem_dev *dev, int index)
{
	int err;
	devno = MKDEV(globalmem_major, index);
	
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev, devno, 1);
	if (err)
		printk(KERN_NOTICE "Error %d adding globalmem%d", err, index);

	globalmem_class_dev = device_create(globalmem_class, NULL, devno, NULL, "globalmem");
}

static const struct file_operations cdev_cc_fops = {
	.owner = THIS_MODULE,
	.llseek = NULL,
	.read = NULL,
	.write = NULL,
	.unlocked_ioctl = NULL,
	.open = NULL,
	.release = NULL,
};

const static char *drv_name = "gm_drv";
const static char *dev_name = "gm_dev";
#define THIS_MAJOR 0

static struct cdev_cc cdev_cc = {
		.class = NULL,
		.dev_start = 0,
		.dev_count = 10,
		.dev_seq = 0,
		.drv_name = drv_name,
		.dev_name = dev_name,
		.major_dev_no = THIS_MAJOR,
	};

/*TODO 检查该函数是否正确,并完成其它部分*/
static int __init cdev_cc_init(void)
{
	int ret;

	dev_t dev_no = MKDEV(cdev_cc.major_dev_no, 0);
	
	/*register char dev no*/
	if (cdev_cc.major_dev_no)
		ret = register_chrdev_region(devno, cdev_cc.count, cdev_cc.drv_name);
	else {
		ret = alloc_chrdev_region(&devno, cdev_cc.dev_start, cdev_cc.count, cdev_cc.drv_name);
		cdev_cc.major_dev_no = MAJOR(devno);
	}

	if (ret < 0) {
		goto just_return;
	}

	cdev_init(&cdev_cc.cdev, &cdev_cc_fops);

	/*create class*/
	cdev_cc.class = class_create(THIS_MODULE, cdev_cc.dev_name);
	if (IS_ERR(cdev_cc.class) || cdev_cc.class == NULL) {
		ret = PTR_ERR(cdev_cc.class);
		cdev_cc.class = NULL;
		goto release_dev_no;
	}
	
	return 0;

release_dev_no:
	unregister_chrdev_region(cdev_cc.major_dev_no, cdev_cc.dev_count);
just_return:
	return ret;
}

static void __exit globalmem_exit(void)
{
	cdev_del(&globalmem_devp->cdev);
	unregister_chrdev_region(globalmem_major, 1);

	device_unregister(globalmem_class_dev);
	device_destroy(globalmem_class, devno);

	kfree(globalmem_devp);
}

module_init(globalmem_init);
module_exit(globalmem_exit);
module_param(globalmem_major, int, S_IRUGO);
MODULE_LICENSE("Dual BSD/GPL");
