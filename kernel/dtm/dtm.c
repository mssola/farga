#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>

MODULE_DESCRIPTION("Kernel module to show off the DeviceTree Kernel API");
MODULE_AUTHOR("Miquel Sabaté Solà");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

// `compatible` value for QEMU/KVM.
static const char *const qemu[] = {
	"riscv-virtio",
	NULL,
};

// `compatible` values that will match on Starfive VisionFive2 v1.3b (plus a
// boogie value).
static const char *const vf2[] = {
	"starfive,visionfive-2-v1.3b",
	"starfive,jh7110",
	"this is bullshit but it won't interfere",
	NULL,
};

static int __init dtm_init(void)
{
	unsigned int cpu;
	struct device_node *node, *parent;
	const char *isa;
	int rc;

	/*
	 * `of_compatible_match` returns true if there is _one_ element of the given
	 * array which matches a string on the `compatible` file. That is, not all
	 * of them have to match.
	 */
	if (of_machine_compatible_match(qemu)) {
		pr_info("Inside of QEMU\n");
	} else if (of_machine_compatible_match(vf2)) {
		pr_info("Inside of VisionFive2\n");
	} else {
		pr_info("Somewhere over the rainbow\n");
	}

	/*
	 * The `for_each_possible_cpu` is a function-like macro provided by the
	 * Kernel which allows `cpu` to be an iterator over CPU device nodes.
	 */
	for_each_possible_cpu(cpu) {
		/*
		 * Given a CPU identifier, `of_cpu_device_node_get` returns a `struct
		 * device_node` related to the specified CPU (or NULL on error).
		 */
		node = of_cpu_device_node_get(cpu);
		if (!node) {
			pr_warn("Unable to find cpu node\n");
			continue;
		}

		/*
		 * `of_property_present` returns a boolean on whether the given node
		 * property is present for the given device node.
		 */
		if (!of_property_present(node, "riscv,isa")) {
			pr_warn("Unable to find 'riscv,isa' for this node\n");
			goto release_cpu_node;
		}

		/*
		 * `of_property_read_string` initializes the given `isa` string with the
		 * contents of the property we are trying to fetch. Returns 0 on
		 * success, and a kernel error otherwise. Note that the previous
		 * `of_property_present` call was actually not needed, we would've get
		 * here an `-EINVAL` error eitherway.
		 */
		rc = of_property_read_string(node, "riscv,isa", &isa);
		if (rc) {
			pr_warn("Unable to find \"riscv,isa\" devicetree entry\n");
			goto release_cpu_node;
		}
		pr_info("RISC-V ISA for '%s': %s\n", node->full_name, isa);

		/*
		 * `of_get_parent` returns the parent device node for the one provided
		 * or NULL if there's none.
		 */
		parent = of_get_parent(node);
		if (!node) {
			pr_warn("No parent for the given node!\n");
			goto release_cpu_node;
		}
		pr_info("parent: %s\n", node->full_name);

		/*
		 * `of_node_put` is used to decrement the reference counter on device
		 * nodes. We are done using these instances, so we should decrement
		 * their life times.
		 */
		of_node_put(parent);
release_cpu_node:
		of_node_put(node);
	}
	return 0;
}

static void __exit dtm_exit(void)
{
	pr_info("Exiting from DTM module\n");
}

module_init(dtm_init);
module_exit(dtm_exit);
