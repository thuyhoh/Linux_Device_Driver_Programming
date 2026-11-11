#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <gpiod.h>

static void usage(const char *program)
{
    fprintf(stderr,
            "Cach dung:\n"
            "  %s <gpio> in\n"
            "  %s <gpio> out <0|1>\n"
            "\nVi du:\n"
            "  %s 60 in\n"
            "  %s 60 out 1\n",
            program, program, program, program);
}

int main (int argc, char *argv[])
{
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    for (int i = 0; i < argc; i++) {
        printf("%s ", i, argv[i]);
        if(i == argc - 1) printf("\n");
    }


}

int main(int argc, char *argv[])
{
    struct gpiod_chip *chip = NULL;
    struct gpiod_line *line = NULL;

    char chip_label[64];
    char *end;

    long pin_no;
    unsigned int port;
    unsigned int pin;

    int output_mode;
    int value = 0;
    int requested = 0;
    int result = EXIT_FAILURE;

    /* 1. Kiem tra tham so */
    if (argc < 3) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[2], "in") == 0) {
        if (argc != 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }

        output_mode = 0;
    } else if (strcmp(argv[2], "out") == 0) {
        if (argc != 4 ||
            (strcmp(argv[3], "0") != 0 &&
             strcmp(argv[3], "1") != 0)) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }

        output_mode = 1;
        value = argv[3][0] - '0';
    } else {
        fprintf(stderr, "Huong GPIO phai la in hoac out.\n");
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    /* 2. Chuyen so GPIO tu chuoi sang so nguyen */
    errno = 0;
    pin_no = strtol(argv[1], &end, 10);

    if (errno == ERANGE ||
        end == argv[1] ||
        *end != '\0' ||
        pin_no < 0 ||
        pin_no > 127) {
        fprintf(stderr, "So GPIO khong hop le: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    /*
     * Cach danh so GPIO truyen thong tren BBB:
     * GPIO = bank * 32 + offset
     *
     * GPIO60 = bank 1, offset 28
     */
    port = (unsigned int)pin_no / 32;
    pin = (unsigned int)pin_no % 32;

    snprintf(chip_label, sizeof(chip_label),
             "gpio-%u-%u", port * 32, port * 32 + 31);

    printf("GPIO%ld: bank %u, offset %u\n", pin_no, port, pin);

    /* 3. Mo chip theo nhan bank */
    chip = gpiod_chip_open_by_label(chip_label);
    if (chip == NULL) {
        perror("gpiod_chip_open_by_label");
        fprintf(stderr,
                "Khong mo duoc chip co nhan \"%s\".\n"
                "Chay gpiodetect de kiem tra nhan chip.\n",
                chip_label);
        goto cleanup;
    }

    /* 4. Lay duong GPIO */
    line = gpiod_chip_get_line(chip, pin);
    if (line == NULL) {
        perror("gpiod_chip_get_line");
        goto cleanup;
    }

    if (output_mode) {
        /* 5a. Request output voi gia tri ban dau */
        if (gpiod_line_request_output(
                line, "bbb-gpio-app", value) == -1) {
            perror("gpiod_line_request_output");
            goto cleanup;
        }

        requested = 1;

        printf("GPIO%ld da duoc dat thanh output, gia tri = %d.\n",
               pin_no, value);
        printf("Nhan Enter de giai phong GPIO va thoat.\n");

        /*
         * Giu request trong luc cho.
         * Sau khi release, khong dam bao GPIO giu nguyen muc.
         */
        getchar();
    } else {
        /* 5b. Request input */
        if (gpiod_line_request_input(
                line, "bbb-gpio-app") == -1) {
            perror("gpiod_line_request_input");
            goto cleanup;
        }

        requested = 1;

        /* Doc gia tri mot lan */
        value = gpiod_line_get_value(line);
        if (value == -1) {
            perror("gpiod_line_get_value");
            goto cleanup;
        }

        printf("GPIO%ld input = %d\n", pin_no, value);
    }

    result = EXIT_SUCCESS;

cleanup:
    if (requested)
        gpiod_line_release(line);

    if (chip != NULL)
        gpiod_chip_close(chip);

    return result;
}