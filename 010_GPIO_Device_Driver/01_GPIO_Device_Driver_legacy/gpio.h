#ifndef __GPIO_H__
#define __GPIO_H__

#define GPIO0_STARTADDR             (0x44E07000)
#define GPIO0_ENDADDR               (0x44E07FFF)
#define GPIO0_SIZE                  (GPIO0_ENDADDR - GPIO0_STARTADDR)    

#define GPIO_OE_OFFSET              (0x134)
#define GPIO_DATAIN_OFFSET          (0x138)
#define GPIO_DATAOUT_OFFSET         (0x13C)
#define GPIO_CLEARDATAOUT_OFFSET    (0x190)
#define GPIO_SETDATAOUT_OFFSET      (0x194)


#endif  /* __GPIO_H__ */
