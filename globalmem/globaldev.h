#ifndef GLOBALDEV_H
#define GLOBALDEV_H

#define GLOBALMEM_SIZE (1024 * 4)
#define MEM_CLEAR 0x01

struct cdev_cc {
	struct class *class;
	struct cdev cdev;
	unsigned int dev_start;
	unsigned int dev_count;
	unsigned int dev_seq;
	char *drv_name;
	char *dev_name;
	dev_t major_dev_no;
};
	struct device device;
	dev_t dev_no;
	char *priv_data;

#endif
