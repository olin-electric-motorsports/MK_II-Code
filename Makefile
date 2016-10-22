ifndef TARGET
$(info Target is not set; do `TARGET={dir}` to set the compilation route.)
endif

CC=avr-gcc
MCU=atmega16m1
#PROGRAMMER=avrispmkII
PROGRAMMER=usbtiny
PORT=usb
AVRDUDE=avrdude
OBJCOPY=avr-objcopy

SRCDIR=src
OBJDIR=obj
BUIDIR=build

SRCS=$(wildcard $(SRCDIR)/$(TARGET)/*.c)
OBJS=$(patsubst $(SRCDIR)/$(TARGET)/%.c, $(OBJDIR)/%.o, $(SRCS))

CFLAGS+=-mmcu=$(MCU) -g -Os -Wall -Wunused -I$(LIBDIR)/
LDFLAGS=-mmcu=$(MCU) -Wl,-Map=$(BUIDIR)/$(TARGET).map -lm
AVRFLAGS=-p $(MCU) -v -c $(PROGRAMMER) -P $(PORT)

# Recipes

all: $(BUIDIR)/$(TARGET).hex $(BUIDIR)/$(TARGET).elf

$(BUIDIR)/%.hex: $(BUIDIR)/%.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(BUIDIR)/%.elf: $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/$(TARGET)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJS): | $(OBJDIR)
$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(BUIDIR)

.PHONY: flash
flash: $(BUIDIR)/$(TARGET).hex
	sudo $(AVRDUDE) $(AVRFLAGS) -U flash:w:$<

.PHONY: clean
clean:
	rm -rf $(BUIDIR) $(OBJDIR)
