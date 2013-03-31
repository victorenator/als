#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <acpi/acpi_bus.h>
#include <acpi/acpi_drivers.h>

MODULE_AUTHOR("Viktar Vauchkevich <victorenator@gmail.com>");
MODULE_DESCRIPTION("Ambient Light Sensor Driver");
MODULE_LICENSE("GPL");

static int als_add(struct acpi_device *device);
static int als_remove(struct acpi_device *device, int type);
static void als_notify(struct acpi_device *device, u32 event);

static const struct acpi_device_id als_device_ids[] = {
	{ "ACPI0008", 0},
	//    { "PNP0C02", 0},
	{ "", 0},
};
MODULE_DEVICE_TABLE(acpi, als_device_ids);

static struct acpi_driver als_driver = {
	.name = "asus-als",
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

static struct attribute *als_attributes[] = {
	&dev_attr_ali.attr,
	NULL
};

static const struct attribute_group als_attr_group = {
	.attrs = als_attributes,
};

static int als_add(struct acpi_device *device)
{
	int result = sysfs_create_group(&device->dev.kobj, &als_attr_group);
	return 0;
}

static int als_remove(struct acpi_device *device, int type)
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
