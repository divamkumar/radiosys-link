#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>

static irqreturn_t radiosys_irq_handler(int irq, void *dev_id) {
    printk(KERN_CRIT "RADIOSYS: *** CRITICAL ALERT RECEIVED ***\n");
    return IRQ_HANDLED;
}

static int radiosys_probe(struct platform_device *pdev) {
    struct device *dev = &pdev->dev;
    struct gpio_desc *gpio;
    int irq;

    printk(KERN_INFO "RADIOSYS: Probing Mission Alert Device...\n");
    gpio = devm_gpiod_get(dev, NULL, GPIOD_IN);
    if (IS_ERR(gpio)) return PTR_ERR(gpio);

    irq = gpiod_to_irq(gpio);
    return devm_request_irq(dev, irq, radiosys_irq_handler, IRQF_TRIGGER_RISING, "radiosys_alert", NULL);
}

static const struct of_device_id radiosys_dt_ids[] = {
    { .compatible = "radiosys,mission-alert" },
    { }
};
MODULE_DEVICE_TABLE(of, radiosys_dt_ids);

static struct platform_driver radiosys_driver = {
    .probe = radiosys_probe,
    .driver = {
        .name = "radiosys_driver",
        .of_match_table = radiosys_dt_ids,
    },
};
module_platform_driver(radiosys_driver);
MODULE_LICENSE("GPL");

