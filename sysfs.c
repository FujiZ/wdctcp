/*
 * Sample kset and ktype implementation
 *
 * Released under the GPL version 2 only.
 *
 */
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>

/*
 * This module shows how to create a kset in sysfs called
 * /sys/kernel/wdctcp
 * Then tree kobjects are created and assigned to this kset, "wvcc", "baz",
 * and "bar".  In those kobjects, attributes of the same name are also
 * created and if an integer is written to these files, it can be later
 * read out of it.
 */


/*
 * This is our "object" that we will create a few of and register them with
 * sysfs.
 */
struct wdctcp_obj {
	struct kobject kobj;
	// TODO change to uint
	u32 weight;
};
#define to_wdctcp_obj(x) container_of(x, struct wdctcp_obj, kobj)

/* a custom attribute that works just for a struct wdctcp_obj. */
struct wdctcp_attr {
	struct attribute attr;
	ssize_t (*show)(struct wdctcp_obj *obj, struct wdctcp_attr *attr, char *buf);
	ssize_t (*store)(struct wdctcp_obj *obj, struct wdctcp_attr *attr,
			 const char *buf, size_t count);
};
#define to_wdctcp_attr(x) container_of(x, struct wdctcp_attr, attr)

/*
 * The default show function that must be passed to sysfs.  This will be
 * called by sysfs for whenever a show function is called by the user on a
 * sysfs file associated with the kobjects we have registered.  We need to
 * transpose back from a "default" kobject to our custom struct wdctcp_obj and
 * then call the show function for that specific object.
 */
static ssize_t wvcc_attr_show(struct kobject *kobj,
			      struct attribute *attr,
			      char *buf)
{
	struct wdctcp_attr *attribute = to_wvcc_attr(attr);
	struct wdctcp_obj *wvcc;

	if (!attribute->show)
		return -EIO;

	wvcc = to_wdctcp_obj(kobj);
	return attribute->show(wvcc, attribute, buf);
}

/*
 * Just like the default show function above, but this one is for when the
 * sysfs "store" is requested (when a value is written to a file.)
 */
static ssize_t wvcc_attr_store(struct kobject *kobj,
			       struct attribute *attr,
			       const char *buf, size_t len)
{
	struct wdctcp_attr *attribute = to_wvcc_attr(attr);
	struct wdctcp_obj *wvcc;

	if (!attribute->store)
		return -EIO;

	wvcc = to_wdctcp_obj(kobj);
	return attribute->store(wvcc, attribute, buf, len);
}

/* Our custom sysfs_ops that we will associate with our ktype later on */
static const struct sysfs_ops wvcc_sysfs_ops = {
	.show = wvcc_attr_show,
	.store = wvcc_attr_store,
};

/*
 * The release function for our object.  This is REQUIRED by the kernel to
 * have.  We free the memory held in our object here.
 *
 * NEVER try to get away with just a "blank" release function to try to be
 * smarter than the kernel.  Turns out, no one ever is...
 */
static void wvcc_release(struct kobject *kobj)
{
	struct wdctcp_obj *wvcc;

	wvcc = to_wdctcp_obj(kobj);
	kfree(wvcc);
}

/*
 * The "weight" file where the .weight variable is read from and written to.
 */
static ssize_t wvcc_weight_show(struct wdctcp_obj *obj,
				struct wdctcp_attr *attr,
				char *buf)
{
	return sprintf(buf, "%u\n", obj->weight);
}

static ssize_t wvcc_weight_store(struct wdctcp_obj *obj,
				 struct wdctcp_attr *attr,
				 const char *buf, size_t count)
{
	/** TODO do santity check before assigning to weight */
	sscanf(buf, "%u", &obj->weight);
	return count;
}

/* Sysfs attributes cannot be world-writable. */
static struct wdctcp_attr weight_attribute =
	__ATTR(weight, 0664, wvcc_weight_show, wvcc_weight_store);

/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *wvcc_default_attrs[] = {
	&weight_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

/*
 * Our own ktype for our kobjects.  Here we specify our sysfs ops, the
 * release function, and the set of default attributes we want created
 * whenever a kobject of this type is registered with the kernel.
 */
static struct kobj_type wvcc_ktype = {
	.sysfs_ops = &wvcc_sysfs_ops,
	.release = wvcc_release,
	.default_attrs = wvcc_default_attrs,
};

static struct kset *wvcc_kset;

/** TODO export interface for wvcc_tcp to create/put kobj */
static struct wdctcp_obj *create_wdctcp_obj(const char *name)
{
	struct wdctcp_obj *wvcc;
	int retval;

	/* allocate the memory for the whole object */
	wvcc = kzalloc(sizeof(*wvcc), GFP_KERNEL | GFP_ATOMIC);
	if (!wvcc)
		return NULL;

	/*
	 * As we have a kset for this kobject, we need to set it before calling
	 * the kobject core.
	 */
	wvcc->kobj.kset = wvcc_kset;

	/*
	 * Initialize and add the kobject to the kernel.  All the default files
	 * will be created here.  As we have already specified a kset for this
	 * kobject, we don't have to set a parent for the kobject, the kobject
	 * will be placed beneath that kset automatically.
	 */
	/** TODO parse addr and port from args */
	retval = kobject_init_and_add(&wvcc->kobj, &wvcc_ktype, NULL,
				      "%s", name);
	if (retval) {
		kobject_put(&wvcc->kobj);
		return NULL;
	}

	/*
	 * We are always responsible for sending the uevent that the kobject
	 * was added to the system.
	 */
	kobject_uevent(&wvcc->kobj, KOBJ_ADD);

	return wvcc;
}

// TODO create wdctcp_obj
static struct wdctcp_obj *;

static void destroy_wdctcp_obj(struct wdctcp_obj *wvcc)
{
	kobject_put(&wvcc->kobj);
}

static int __init example_init(void)
{
	/*
	 * Create a kset with the name of "wvcc",
	 * located under /sys/kernel/
	 */
	wvcc_kset = kset_create_and_add("wdctcp", NULL, kernel_kobj);
	if (!wvcc_kset)
		return -ENOMEM;

	/*
	 * Create three objects and register them with our kset
	 */
	wdctcp_obj = create_wdctcp_obj("foo");
	if (!wdctcp_obj)
		goto wvcc_error;

	return 0;

wvcc_error:
	kset_unregister(wvcc_kset);
	return -EINVAL;
}

static void __exit example_exit(void)
{
	destroy_wdctcp_obj(wdctcp_obj);
	kset_unregister(wvcc_kset);
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("FujiZ <i@fujiz.me>");
