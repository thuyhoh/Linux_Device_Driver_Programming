#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/interrupt.h>
#include <linux/gpio/consumer.h>
#include <linux/atomic.h>

struct bbb_irq_dev {
	struct gpio_desc *btn_gpiod;
	int irq;
	atomic_t irq_count;
};

static irqreturn_t bbb_button_top_half(int irq, void *dev_id)
{
	struct bbb_irq_dev *dev = (struct bbb_irq_dev *)dev_id;

	atomic_inc(&dev->irq_count);
	pr_info("BBB Top Half IRQ %d triggered! Total count = %d\n",
		irq, atomic_read(&dev->irq_count));

	return IRQ_HANDLED;
}

static int bbb_irq_probe(struct platform_device *pdev)
{
	struct bbb_irq_dev *dev;
	int ret;

	dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	atomic_set(&dev->irq_count, 0);

	dev->btn_gpiod = devm_gpiod_get(&pdev->dev, "irq", GPIOD_IN);
	if (IS_ERR(dev->btn_gpiod)) {
		dev_err(&pdev->dev, "Failed to get GPIO descriptor\n");
		return PTR_ERR(dev->btn_gpiod);
	}

	dev->irq = gpiod_to_irq(dev->btn_gpiod);
	if (dev->irq < 0) {
		dev_err(&pdev->dev, "Failed to map GPIO to IRQ: %d\n", dev->irq);
		return dev->irq;
	}

	ret = request_irq(dev->irq, bbb_button_top_half, IRQF_TRIGGER_FALLING, "bbb_btn_top_half", dev);
	if (ret) {
		dev_err(&pdev->dev, "Failed to request IRQ %d: %d\n", dev->irq, ret);
		return ret;
	}

	platform_set_drvdata(pdev, dev);
	dev_info(&pdev->dev, "BBB GPIO IRQ driver probed (IRQ = %d)\n", dev->irq);
	return 0;
}

static int bbb_irq_remove(struct platform_device *pdev)
{
	struct bbb_irq_dev *dev = platform_get_drvdata(pdev);

	free_irq(dev->irq, dev);
	pr_info("BBB GPIO IRQ driver removed. Final count = %d\n",
		atomic_read(&dev->irq_count));
	return 0;
}

static const struct of_device_id bbb_irq_of_match[] = {
	{ .compatible = "custom,bbb-gpio-irq" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, bbb_irq_of_match);

static struct platform_driver bbb_irq_driver = {
	.probe  = bbb_irq_probe,
	.remove = bbb_irq_remove,
	.driver = {
		.name           = "bbb_top_half_irq",
		.of_match_table = bbb_irq_of_match,
	},
};

module_platform_driver(bbb_irq_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thuy");
MODULE_DESCRIPTION("BeagleBone Black GPIO Interrupt Top Half Example");