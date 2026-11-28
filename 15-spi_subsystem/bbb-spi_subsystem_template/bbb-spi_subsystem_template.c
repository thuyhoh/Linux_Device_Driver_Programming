#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/spi/spi.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/interrupt.h>

struct my_spi_priv {
	struct spi_device *spi;
	u8 *tx_buf;
	u8 *rx_buf;
};

static irqreturn_t my_spi_irq_top(int irq, void *dev_id)
{
	struct my_spi_priv *priv = dev_id;

	if (!priv)
		return IRQ_NONE;

	return IRQ_WAKE_THREAD;
}

static irqreturn_t my_spi_irq_thread(int irq, void *dev_id)
{
	struct my_spi_priv *priv = dev_id;
	int ret;

	/* Ví dụ: gửi 1 byte lệnh đọc trạng thái/dữ liệu và nhận 2 byte */
	priv->tx_buf[0] = 0x01;
	ret = spi_write_then_read(priv->spi, priv->tx_buf, 1, priv->rx_buf, 2);
	if (ret < 0) {
		dev_err(&priv->spi->dev, "SPI read in IRQ thread failed: %d\n", ret);
		return IRQ_NONE;
	}

	dev_dbg(&priv->spi->dev, "IRQ data: 0x%02x 0x%02x\n",
		priv->rx_buf[0], priv->rx_buf[1]);

	return IRQ_HANDLED;
}

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
	struct my_spi_priv *priv;
	int ret;

	priv = devm_kzalloc(&spi->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->tx_buf = devm_kzalloc(&spi->dev, 8, GFP_KERNEL);
	priv->rx_buf = devm_kzalloc(&spi->dev, 8, GFP_KERNEL);
	if (!priv->tx_buf || !priv->rx_buf)
		return -ENOMEM;

	priv->spi = spi;
	spi_set_drvdata(spi, priv);

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

	if (spi->irq > 0) {
		ret = devm_request_threaded_irq(&spi->dev, spi->irq,
						my_spi_irq_top,
						my_spi_irq_thread,
						IRQF_ONESHOT,
						dev_name(&spi->dev),
						priv);
		if (ret < 0) {
			dev_err(&spi->dev, "Failed to request IRQ %d: %d\n",
				spi->irq, ret);
			return ret;
		}
	}

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