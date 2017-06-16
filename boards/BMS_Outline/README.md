# General BMS Outline

There are a number of different elements that go into BMS firmware design. 
The key components are being able to check voltages and temperatures at a regular interval, as well as ensuring 100% uptime.

## Sampling

The voltage and temperature sampling for the BMS master is going to utilize the code from the LT-whatever chip. In theory it should be as simple as setting up SPI to run through a set of pre-computed bytes, and then storing the received data. 

(Note that we could calculate the bytes we need to send on the MCU, in which case the code below would have a slight modification)

For example,

```c
// The SPI codes to send for reading voltages
#DEFINE SPI_VOLT_SENSE_COUNT 4
uint8_t voltage_sense_SPI_codes[] = { 0x55, 0x56, 0x57, 0x58 };

// The SPI codes to send for reading temperature
#DEFINE SPI_TEMP_SENSE_COUNT 4
uint8_t temperature_sense_SPI_codes[] = { 0x65, 0x66, 0x67, 0x68 };

// Our data collection
uint8_t voltages[SPI_VOLT_SENSE_COUNT];
uint8_t temperatures[SPI_TEMP_SENSE_COUNT];

// Function call for reading voltages
uint8_t sample_all_voltages(void)
{
    for (uint8_t i = 0; i < SPI_VOLT_SENSE_COUNT; i++)
    {
        // Start transmission
        SPIDR = voltage_sense_SPI_codes[i];

        // Wait for received bits
        while( bit_is_clear(SPSR, SPIF) ) continue;

        // Read the SPI data
        volatile uint8_t data = SPIDR;

        // Possibly do data checking here
        if ( !data_valid_volt(data) )
        {
            // Return error
            return 1;
        }

        // Store the data
        voltages[i] = data;
    }

    // No error
    return 0;
}


// Function call for reading temperatures
uint8_t sample_all_temperatures(void)
{
    for (uint8_t i = 0; i < SPI_TEMP_SENSE_COUNT; i++)
    {
        // Start transmission
        SPIDR = temperature_sense_SPI_codes[i];

        // Wait for received bits
        while( bit_is_clear(SPSR, SPIF) ) continue;

        // Read the SPI data
        volatile uint8_t data = SPIDR;

        // Possibly do data checking here
        if ( !data_valid_temp(data) )
        {
            // Return error
            return 1;
        }

        // Store the data
        temperatures[i] = data;
    }

    // No error
    return 0;
}

```

## Timing

The (slightly) trickier part is going to be setting up the timing so that the voltages and temperatures are ready at a regular interval. For this, you will need to use one of the timers (8-bit or 16-bit) and have it interrupt on overflow, or at a comparison match.

In order to keep our processing in the interrupt at a minimum, we can simply set a flag.

For instance,

```c
uint8_t flags = 0;
#DEFINE READ_VALS 1

void setup_timer(void)
{
    // Initialize 8-bit or 16-bit timer
    // Either in OVF or Comparision mode; doesn't really matter
}

ISR (TIMER0_OVF_vect) // or any of the other timer interrupt vectors
{
    flags |= _BV(READ_VALS);
}

uint8_t read_all_vals(void)
{
    uint8_t err = 0;
    err = sample_all_voltages();
    if (err)
    {
        // propagate error
        return err;
    }

    err = sample_all_temperatures();
    if (err)
    {
        return err;
    }

    // TODO: Send CAN data (?)

    return;
}

int main(void)
{ 
    // Initialize stuff here...

    // Our favorite infinite loop
    uint8_t err = 0;
    while(1)
    {
        if (bit_is_set(flags, READ_VALS))
        {
            err = read_all_vals();
            if (err)
            {
                // Handle the error in some way
            }
        }
    }
}

```

## CAN

CAN for the BMS is no different than for any other board. There may need to be additional processing on the data (say averaging it) so that we can send the minimal amount of necessary information.

When averaging data or working with data, keep the data as integers and NEVER start using floats. It may sound like a good idea to do:

```c
int total = 0;
for (uint8_t i = 0; i < LEN_OF_DATA; i++)
{
    total += data[i];
}

float avg = ((float) total) / ((float) LEN_OF_DATA);

```

But this will be absolutely detrimental for the AVR performance. Float arithmetic is wildly expensive.


Arithmetic processes that are incredibly fast on an AVR are division by a multiple of 2, multiplication by a multiple of 2, and any bit-arithmetic. Stick with those for processing data. When in doubt, just simply don't process the data. (Do we really need the average, or can we get away with just sending one of the voltages and ensuring the rest are at least in a safe range? Or maybe send the highest value.)

## Watchdog

Since the BMS code must have 100% uptime, getting stuck in an infinite loop or have other bad behavior would be detrimental. Thankfully, there are systems in place to handle this type of issue.

The one we will use is a watchdog. The general idea behind a watchdog is that you have to "kick" the dog at a regular interval or else it will "kill" you. Usually they are a separate chip from the MCU that you have to pulse at a regular interval or else it will pull the reset pin low (therefore reseting the chip).

For us, the ATmega16m1 has a watchdog built in. Information about how to use it is in section 11.8 of the datasheet. There are a few quirks to the watchdog timer, one being that if it is ever turned on it will remain on unless you actively turn it off (which can be a massive pain).

## Sleep

The last part of the BMS functionality will be going to sleep when not in use. To actually put the AVR to sleep is fairly straightforward, and there are a number of additional macros Atmel provides for doing so. The tricky part will be keeping the Watchdog happy while going to sleep for as long as possible.

## Putting it all together

Thankfully none of the above code is very tricky. In terms of architecture, it is not a good idea to do a lot of processing within an interrupt, and so make heavy usage of flags while doing interrupt based code.

Additionally, ensure that errors are handled properly. Generally you should propagate the error until it is at a part of the code where it is easy to verify the error handling. Honestly, it might be a good idea to have only one section of the code actually handle the errors and everywhere else just propagates them down. This way the code can be more easily verified for correctness.

A good rule of thumb is to keep one function for doing one thing, and to never nest code more than 3 levels deep.If a function gets too long, consider breaking it into smaller functions.

