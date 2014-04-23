#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/acpi.h>

MODULE_AUTHOR("Viktar Vauchkevich <victorenator@gmail.com>");
MODULE_DESCRIPTION("Ambient Light Sensor Driver");
MODULE_LICENSE("GPL");

static int als_add(struct acpi_device *device);
static int als_remove(struct acpi_device *device);
static void als_notify(struct acpi_device *device, u32 event);

static const struct acpi_device_id als_device_ids[] = {
	{"ACPI0008", 0},
	{},
};
MODULE_DEVICE_TABLE(acpi, als_device_ids);

static struct acpi_driver als_driver = {
	.name = "als",
	.class = "ALS",
	.ids = als_device_ids,
	.ops =
	{
		.add = als_add,
		.remove = als_remove,
		.notify = als_notify
	},
	.owner = THIS_MODULE,
};

static u32 als_get_ali(struct acpi_device *device)
{
	u64 ali;
	acpi_status status;

	status = acpi_evaluate_integer(device->handle, (acpi_string) "_ALI", NULL, &ali);

	return ali;
}

static ssize_t als_show_ali(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct acpi_device *device = to_acpi_device(dev);

	return sprintf(buf, "%d\n", als_get_ali(device));
}

static DEVICE_ATTR(ali, S_IRUGO, als_show_ali, NULL);

static acpi_handle acpi_handle_from_string(const char *str) {
	acpi_handle handle;
	acpi_status handle_status;
	handle_status = acpi_get_handle(NULL, (acpi_string) str, &handle);

	if (ACPI_FAILURE(handle_status)) {
		return NULL;
	} else {
		return handle;
	}
}

static ssize_t als_enable_disable(struct device *dev, struct device_attribute *attr,
	const char *buf, size_t count)
{
	if (count >= 1) {
    acpi_handle tals_handle;
		struct acpi_object_list arg;
		union acpi_object param_0, param_1;

		param_0.type = ACPI_TYPE_INTEGER;
		param_0.integer.value = 0;
		param_1.type = ACPI_TYPE_INTEGER;
		param_1.integer.value = 1;

    tals_handle = acpi_handle_from_string("\\_SB.PCI0.LPCB.EC0.TALS");
    if (tals_handle == NULL) {
			printk(KERN_ERR "als: failed reading \\_SB.PCI0.LPCB.EC0.TALS\n");
			return count;
		}

		if (buf[0] == '0') {
			// Disable
			acpi_status result;

			arg.count = 1;
			arg.pointer = &param_0;
			result = acpi_evaluate_object(tals_handle, NULL, &arg, NULL);

		} else if (buf[0] == '1') {
			// Enable
			acpi_handle alsc_handle;
			acpi_status result;

			arg.count = 1;
			arg.pointer = &param_1;
			result = acpi_evaluate_object(tals_handle, NULL, &arg, NULL);

		  alsc_handle = acpi_handle_from_string("\\_SB.ATKD.ALSC");
		  if (alsc_handle == NULL) {
				printk(KERN_ERR "als: failed reading \\_SB.ATKD.ALSC\n");
				return count;
			}

			result = acpi_evaluate_object(alsc_handle, NULL, &arg, NULL);
		}

		return count;

	} else {
		return count;
	}
}

static DEVICE_ATTR(enable, S_IWUSR, NULL, als_enable_disable);

static struct attribute *als_attributes[] = {
	&dev_attr_ali.attr,
	&dev_attr_enable.attr,
	NULL
};

static const struct attribute_group als_attr_group = {
	.attrs = als_attributes,
};

static int als_add(struct acpi_device *device)
{
	int result;
	result = sysfs_create_group(&device->dev.kobj, &als_attr_group);
	return result;
}
	
static int als_remove(struct acpi_device *device)
{
	sysfs_remove_group(&device->dev.kobj, &als_attr_group);
	return 0;
}

static void als_notify(struct acpi_device* device, u32 event)
{
	pr_info("als_notify %x %x\n", event, als_get_ali(device));
	kobject_uevent(&device->dev.kobj, KOBJ_CHANGE);
}

static int __init als_init(void)
{
	int result = 0;

	result = acpi_bus_register_driver(&als_driver);
	if (result < 0) {
		ACPI_DEBUG_PRINT((ACPI_DB_ERROR,
			"Error registering driver\n"));
		return -ENODEV;
	}

	return 0;
}

static void __exit als_exit(void)
{
	acpi_bus_unregister_driver(&als_driver);
}


module_init(als_init);
module_exit(als_exit);
