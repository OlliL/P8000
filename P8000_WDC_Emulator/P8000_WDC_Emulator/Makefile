OUTPUT_FILE=P8000_WDC_Emulator
MCU=	-mmcu=atmega1284p

CC=avr-gcc
CFLAGS= -I/usr/local/avr/include -funsigned-char -funsigned-bitfields -O1 \
	-ffunction-sections -fpack-struct -fshort-enums -Wall -Wundef \
	-Werror -pedantic -pedantic-errors -c -std=c99  ${MCU}

LDFLAGS=-Wl,-Map="${OUTPUT_FILE}.map" -Wl,--start-group -Wl,-lm  \
	-Wl,--end-group -Wl,--gc-sections -mrelax ${MCU}

RM := rm -rf

OBJS +=  \
wdc_avr.o \
wdc_main.o \
wdc_drv_mmc.o \
wdc_drv_mmc_perf.o \
uart.o \
wdc_if_p8000.o \
wdc_if_disk.o \
wdc_par.o \
wdc_ram.o

all: ${OUTPUT_FILE}

${OUTPUT_FILE}: ${OBJS}
	avr-gcc -o${OUTPUT_FILE}.elf ${OBJS} ${LDFLAGS}
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature  "${OUTPUT_FILE}.elf" "${OUTPUT_FILE}.hex"
	avr-objcopy -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "${OUTPUT_FILE}.elf" "${OUTPUT_FILE}.eep" || exit 0
	avr-objdump -h -S "${OUTPUT_FILE}.elf" > "${OUTPUT_FILE}.lss"
	avr-size "${OUTPUT_FILE}.elf"
	
clean:
	-${RM} ${OBJS}
	rm -rf "${OUTPUT_FILE}.elf" "${OUTPUT_FILE}.a" "${OUTPUT_FILE}.hex" "${OUTPUT_FILE}.lss" "${OUTPUT_FILE}.eep" "${OUTPUT_FILE}.map"
	
