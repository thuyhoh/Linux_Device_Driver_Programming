#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/spi/spi.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/string.h>

static void spidev_printinfo(struct spi_device *spi)
{
	u32 cpol = (spi->mode & SPI_CPOL) ? 1 : 0;
	u32 cpha = (spi->mode & SPI_CPHA) ? 1 : 0;
	u32 spi_mode_num = (cpol << 1) | cpha;

	dev_info(&spi->dev, "=== SPI Device Configuration ===\n");
	dev_info(&spi->dev, "  modalias      : %s\n", spi->modalias);
	dev_info(&spi->dev, "  controller    : %s\n",
		 spi->controller ? dev_name(&spi->controller->dev) : "NULL");
	dev_info(&spi->dev, "  chip_select   : %u\n", spi->chip_select);
	dev_info(&spi->dev, "  max_speed_hz  : %u Hz (%u kHz)\n",
		 spi->max_speed_hz, spi->max_speed_hz / 1000);
	dev_info(&spi->dev, "  bits_per_word : %u\n",
		 spi->bits_per_word ? spi->bits_per_word : 8);
	dev_info(&spi->dev, "  SPI Mode      : Mode %u (CPOL=%u, CPHA=%u)\n",
		 spi_mode_num, cpol, cpha);
	dev_info(&spi->dev, "  raw mode bits : 0x%x\n", spi->mode);
	dev_info(&spi->dev, "  flags         : CS_HIGH=%u, LSB_FIRST=%u, 3WIRE=%u, NO_CS=%u, READY=%u\n",
		 !!(spi->mode & SPI_CS_HIGH),
		 !!(spi->mode & SPI_LSB_FIRST),
		 !!(spi->mode & SPI_3WIRE),
		 !!(spi->mode & SPI_NO_CS),
		 !!(spi->mode & SPI_READY));
	dev_info(&spi->dev, "  cs_gpiod      : %s\n",
		 spi->cs_gpiod ? "assigned (GPIO CS)" : "none (HW/default CS)");
	dev_info(&spi->dev, "  irq           : %d\n", spi->irq);
}

static int spidev_send_hello(struct spi_device *spi)
{
	const char *hello = "hello world";
	size_t len = strlen(hello);
	u8 *tx_buf;
	int ret;

	/* Cấp phát buffer riêng để an toàn với controller dùng DMA */
	tx_buf = kmemdup(hello, len, GFP_KERNEL);
	if (!tx_buf)
		return -ENOMEM;

	ret = spi_write(spi, tx_buf, len);
	kfree(tx_buf);

	if (ret < 0) {
		dev_err(&spi->dev, "spi_write(\"%s\") failed: %d\n", hello, ret);
		return ret;
	}

	dev_info(&spi->dev, "Sent %zu bytes over SPI: \"%s\"\n", len, hello);
	return 0;
}

static int spidev_probe(struct spi_device *spi)
{
	int ret;

	spidev_printinfo(spi);

	/* Đảm bảo cấu hình SPI được áp dụng trước khi truyền */
	if (!spi->bits_per_word)
		spi->bits_per_word = 8;

	ret = spi_setup(spi);
	if (ret < 0) {
		dev_err(&spi->dev, "spi_setup failed: %d\n", ret);
		return ret;
	}

	ret = spidev_send_hello(spi);
	if (ret < 0)
		return ret;

	return 0;
}

static int spidev_remove(struct spi_device *spi)
{
	dev_info(&spi->dev, "unbound from SPI device\n");
	return 0;
}

static const struct of_device_id spidev_of_match[] = {
	{ .compatible = "company,spi-sample" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, spidev_of_match);

static struct spi_driver spidev_spi_driver = {
	.driver = {
		.name = "spidev_dummy",
		.of_match_table = spidev_of_match,
	},
	.probe = spidev_probe,
	.remove = spidev_remove,
};

module_spi_driver(spidev_spi_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thuy");
MODULE_DESCRIPTION("Minimal SPI driver for Device Tree binding");