/*
 * Weighted DCTCP congestion control interface
 */
#ifndef __WDCTCP_H
#define __WDCTCP_H

#include <linux/kobject.h>
#include <linux/sysfs.h>

/*
 * This is our "object" that we will create a few of and register them with
 * sysfs.
 */
struct wdctcp_obj {
	struct kobject kobj;
	u32 weight;
};

struct wdctcp {
	/* dctcp related params */
	u32 acked_bytes_ecn;
	u32 acked_bytes_total;
	u32 prior_snd_una;
	u32 prior_rcv_nxt;
	u32 dctcp_alpha;
	u32 next_seq;
	u32 ce_state;
	u32 delayed_ack_reserved;
	/* pointer to wdctcp_obj containing the weight */
	struct wdctcp_obj *obj;
	/* counter for weighted increase */
	u32 weight_acked_cnt;
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

struct wdctcp_obj *wdctcp_obj_create(const struct sock *sk);
void wdctcp_obj_put(struct wdctcp_obj *obj);

int wdctcp_sysfs_init(void);
void wdctcp_sysfs_exit(void);

extern unsigned int dctcp_shift_g __read_mostly;
extern unsigned int dctcp_alpha_on_init __read_mostly;
extern unsigned int dctcp_clamp_alpha_on_loss __read_mostly;

extern unsigned int wdctcp_precision __read_mostly;
extern unsigned int wdctcp_weight_on_init __read_mostly;

#endif	/* __WDCTCP_H */
