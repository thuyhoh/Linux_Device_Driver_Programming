/*
 ============================================================================
 Name        : 002_Button.c
 Author      : thuy
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "gpio_driver.h"

int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

	gpio_export(0);

	return EXIT_SUCCESS;
}
