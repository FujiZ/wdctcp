#include <linux/module.h>

#include "wdctcp.h"

/* DCTCP related module params */
unsigned int dctcp_shift_g __read_mostly = 4; /* g = 1/2^4 */
module_param(dctcp_shift_g, uint, 0644);
MODULE_PARM_DESC(dctcp_shift_g, "parameter g for updating dctcp_alpha");

unsigned int dctcp_alpha_on_init __read_mostly = DCTCP_MAX_ALPHA;
module_param(dctcp_alpha_on_init, uint, 0644);
MODULE_PARM_DESC(dctcp_alpha_on_init, "parameter for initial alpha value");

unsigned int dctcp_clamp_alpha_on_loss __read_mostly;
module_param(dctcp_clamp_alpha_on_loss, uint, 0644);
MODULE_PARM_DESC(dctcp_clamp_alpha_on_loss, "parameter for clamping alpha on loss");

/* Weighted DCTCP related module params */
unsigned int wdctcp_precision __read_mostly = 10000;
module_param(wdctcp_precision, uint, 0444);	/* read only */
MODULE_PARM_DESC(wdctcp_precision, "parameter for fix point precision");

unsigned int wdctcp_weight_on_init __read_mostly = 10000;
module_param(wdctcp_weight_on_init, uint, 0644);
MODULE_PARM_DESC(wdctcp_weight_on_init, "parameter for initial weight value");


static int __init wdctcp_init(void)
{
	BUILD_BUG_ON(sizeof(struct wdctcp) > ICSK_CA_PRIV_SIZE);
	int err;

	err = tcp_wdctcp_register();
	if (unlikely(err))
		goto failed_tcp_register;

	err = wdctcp_sysfs_init();
	if (unlikely(err))
		goto failed_sysfs_init;

	return 0;

failed_sysfs_init:
	tcp_wdctcp_unregister();

failed_tcp_register:
	return err;
}

static void __exit wdctcp_exit(void)
{
	wdctcp_sysfs_exit();
	tcp_wdctcp_unregister();
}

module_init(wdctcp_init);
module_exit(wdctcp_exit);

MODULE_AUTHOR("FujiZ <i@fujiz.me>");

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Weighted DCTCP (WDCTCP)");
