#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x355f7b1c, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x1a28d412, __VMLINUX_SYMBOL_STR(kobject_put) },
	{ 0xc247dd25, __VMLINUX_SYMBOL_STR(kset_create_and_add) },
	{ 0xfc2dd0ae, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x938919ec, __VMLINUX_SYMBOL_STR(kobject_uevent) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0x20c55ae0, __VMLINUX_SYMBOL_STR(sscanf) },
	{ 0x4bab515e, __VMLINUX_SYMBOL_STR(tcp_register_congestion_control) },
	{ 0xea4725b8, __VMLINUX_SYMBOL_STR(kobject_add) },
	{ 0xd8fc94a7, __VMLINUX_SYMBOL_STR(tcp_unregister_congestion_control) },
	{ 0x496e5589, __VMLINUX_SYMBOL_STR(tcp_send_ack) },
	{ 0x42de85f1, __VMLINUX_SYMBOL_STR(tcp_reno_ssthresh) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x311a546c, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x2f266567, __VMLINUX_SYMBOL_STR(kernel_kobj) },
	{ 0x3a18298e, __VMLINUX_SYMBOL_STR(tcp_slow_start) },
	{ 0x345508b5, __VMLINUX_SYMBOL_STR(kset_unregister) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xa07ddcd6, __VMLINUX_SYMBOL_STR(kobject_init) },
	{ 0x149c7752, __VMLINUX_SYMBOL_STR(param_ops_uint) },
	{ 0xa396a1c, __VMLINUX_SYMBOL_STR(tcp_reno_cong_avoid) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "FB5238F80D62A48A532A5AF");
