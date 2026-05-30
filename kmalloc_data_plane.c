#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>

#define MAX_ITEMS 10

static void *buffers[MAX_ITEMS];
static int sizes[MAX_ITEMS];
static int count = 0;

/* module parameters (control plane → data plane input) */

static char *names[MAX_ITEMS];
static int values[MAX_ITEMS];
static int arr_size = 0;

module_param_array(names, charp, &arr_size, 0644);
MODULE_PARM_DESC(names, "Allocation names");

module_param_array(values, int, &arr_size, 0644);
MODULE_PARM_DESC(values, "Allocation sizes");

/* DATA PLANE: perform allocations */
static int __init dp_init(void)
{
    int i;

    pr_info("Data plane init: allocating memory\n");

    if (arr_size > MAX_ITEMS)
        arr_size = MAX_ITEMS;

    for (i = 0; i < arr_size; i++) {

        buffers[i] = kmalloc(values[i], GFP_KERNEL);

        if (!buffers[i]) {
            pr_err("Allocation failed for %s\n", names[i]);
            continue;
        }

        sizes[i] = values[i];
        count++;

        pr_info("Allocated %s -> %d bytes\n",
                names[i],
                values[i]);
    }

    return 0;
}

/* cleanup */
static void __exit dp_exit(void)
{
    int i;

    pr_info("Data plane exit: freeing memory\n");

    for (i = 0; i < count; i++) {

        kfree(buffers[i]);

        pr_info("Freed buffer %d (%s)\n",
                i,
                names[i]);
    }
}

module_init(dp_init);
module_exit(dp_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Clean Rewrite");
MODULE_DESCRIPTION("kmalloc data plane (5.15 safe, module-param driven)");
