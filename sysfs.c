/*
 * Sample kset and ktype implementation
 *
 * Released under the GPL version 2 only.
 *
 */
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/init.h>

#include "wdctcp.h"

/*
 * This module creates a kset in sysfs called /sys/kernel/wdctcp
 */

/*
 * The default show function that must be passed to sysfs.  This will be
 * called by sysfs for whenever a show function is called by the user on a
 * sysfs file associated with the kobjects we have registered.  We need to
 * transpose back from a "default" kobject to our custom struct wdctcp_obj and
 * then call the show function for that specific object.
 */
static ssize_t wdctcp_attr_show(struct kobject *kobj,
				struct attribute *attr,
				char *buf)
{
	struct wdctcp_attr *attribute = to_wdctcp_attr(attr);
	struct wdctcp_obj *object;

	if (!attribute->show)
		return -EIO;

	object = to_wdctcp_obj(kobj);
	return attribute->show(object, attribute, buf);
}

/*
 * Just like the default show function above, but this one is for when the
 * sysfs "store" is requested (when a value is written to a file.)
 */
static ssize_t wdctcp_attr_store(struct kobject *kobj,
				 struct attribute *attr,
				 const char *buf, size_t len)
{
	struct wdctcp_attr *attribute = to_wdctcp_attr(attr);
	struct wdctcp_obj *object;

	if (!attribute->store)
		return -EIO;

	object = to_wdctcp_obj(kobj);
	return attribute->store(object, attribute, buf, len);
}

/* Our custom sysfs_ops that we will associate with our ktype later on */
static const struct sysfs_ops wdctcp_sysfs_ops = {
	.show = wdctcp_attr_show,
	.store = wdctcp_attr_store,
};

/*
 * The release function for our object.  This is REQUIRED by the kernel to
 * have.  We free the memory held in our object here.
 *
 * NEVER try to get away with just a "blank" release function to try to be
 * smarter than the kernel.  Turns out, no one ever is...
 */
static void wdctcp_release(struct kobject *kobj)
{
	struct wdctcp_obj *object;

	object = to_wdctcp_obj(kobj);
	kfree(object);
}

/*
 * The "weight" file where the .weight variable is read from and written to.
 */
static ssize_t wdctcp_weight_show(struct wdctcp_obj *obj,
				  struct wdctcp_attr *attr,
				  char *buf)
{
	return sprintf(buf, "%u\n", obj->weight);
}

static ssize_t wdctcp_weight_store(struct wdctcp_obj *obj,
				   struct wdctcp_attr *attr,
				   const char *buf, size_t count)
{
	/** TODO do santity check before assigning to weight */
	sscanf(buf, "%u", &obj->weight);
	return count;
}

/* Sysfs attributes cannot be world-writable. */
static struct wdctcp_attr wdctcp_weight_attr =
	__ATTR(weight, 0644, wdctcp_weight_show, wdctcp_weight_store);

/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *wdctcp_default_attrs[] = {
	&wdctcp_weight_attr.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

/*
 * Our own ktype for our kobjects.  Here we specify our sysfs ops, the
 * release function, and the set of default attributes we want created
 * whenever a kobject of this type is registered with the kernel.
 */
static struct kobj_type wdctcp_ktype = {
	.sysfs_ops = &wdctcp_sysfs_ops,
	.release = wdctcp_release,
	.default_attrs = wdctcp_default_attrs,
};

static struct kset *wdctcp_kset;

/** interface for wdctcp to create kobj */
struct wdctcp_obj *wdctcp_obj_create(const struct sock *sk)
{
	struct wdctcp_obj *object;
	int retval;

	/* allocate the memory for the whole object */
	object = kzalloc(sizeof(*object), GFP_KERNEL | GFP_ATOMIC);
	if (!object)
		return NULL;

	/*
	 * As we have a kset for this kobject, we need to set it before calling
	 * the kobject core.
	 */
	object->kobj.kset = wdctcp_kset;

	/*
	 * Initialize and add the kobject to the kernel.  All the default files
	 * will be created here.  As we have already specified a kset for this
	 * kobject, we don't have to set a parent for the kobject, the kobject
	 * will be placed beneath that kset automatically.
	 */
	kobject_init(&object->kobj, &wdctcp_ktype);
	/* get addr && port from sock */
	// TODO we should check if this output-format is correct.
	switch (sk->sk_family) {
	case AF_INET:
		/* sk_daddr is of type __be32 */
		retval = kobject_add(&object->kobj, NULL, "%pI4:%hu-%pI4:%hu",
				     sk->sk_rcv_saddr, sk->sk_num,
				     sk->sk_daddr, ntohs(sk->sk_dport));
		break;
	case AF_INET6:
		/* sk_v6_daddr is passed by refrence */
		retval = kobject_add(&object->kobj, NULL, "[%pI6]:%hu-[%pI6]:%hu",
				     sk->sk_v6_rcv_saddr, sk->sk_num,
				     sk->sk_v6_daddr, ntohs(sk->sk_dport));
		break;
	default:
		/* neither ipv4 nor ipv6: not likely */
		retval = -EINVAL;
		break;
	}

	if (retval) {
		kobject_put(&object->kobj);
		return NULL;
	}

	object->weight = wdctcp_weight_on_init;

	/*
	 * We are always responsible for sending the uevent that the kobject
	 * was added to the system.
	 */
	kobject_uevent(&object->kobj, KOBJ_ADD);

	return object;
}

/** interface for wdctcp to put kobj */
void wdctcp_obj_put(struct wdctcp_obj *obj)
{
	kobject_put(&obj->kobj);
}

int wdctcp_sysfs_init(void)
{
	/*
	 * Create a kset with the name of "wdctcp",
	 * located under /sys/kernel/
	 */
	wdctcp_kset = kset_create_and_add("wdctcp", NULL, kernel_kobj);
	if (!wdctcp_kset)
		return -ENOMEM;
	return 0;
}

void wdctcp_sysfs_exit(void)
{
	kset_unregister(wdctcp_kset);
}
