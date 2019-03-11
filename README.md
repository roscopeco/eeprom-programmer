# Very hacky EEPROM programmer

This project uses an ESP8266 dev board (NodeMCU 1.0) along with three 74LS174 hex D-flip flops to program the 64 Kbit EEPROMs 
I'm using in my [m68k breadboard computer](https://hackaday.io/project/164305-yet-another-m68k-homebrew).

The choice of components may seem a bit odd - the design was driven entirely by what I had available on the bench at the
time, and my being too impatient to wait for a ten-quid programmer to arrive in the post. So I just hacked this together
quickly to tide me over. 

Schematic (such as it is) coming soon. 

## The board

![Board image](board.jpg?raw=true "The board")

The board is driven by a NodeMCU, which has 9 GPIOs (plus a few more if you use the SPI pins etc, but I've had 
limited success with these on some of my boards). Its real purpose is as a WiFi-enabled IOT controller, but I like 
it for quick hack projects like this too. It's Arduino compatible so I can program it in C, and (more crucially for this
project) the GPIO pins are 5v tolerant. It even provides a handy 5V on its `Vin` pin when powered via USB.

Because of limited GPIOs I'm using a shift register to handle 18 of the 21 input pins on the EEPROM (An Atmel AT28C64B).
This only handles 18 bits, because I only had three 74LS174s lying around, each of which has 6 flip-flops. I obviously
didn't have any 74LS595s, or I'd have used them instead.

I decided to dedicate the low 8 bits to the 8 data lines, and the high 10 bits to the lowest 10 address lines (`A0` - `A9`).
The remaining three address lines are handled directly by GPIO pins. I would probably have done this differently if I'd
had chip-enable on the shift register, and handled at least the highest data pin (`IO7`) with a dedicated GPIO, along with
the output enable (`OE`) pin on the EEPROM - This would have allowed me to do the polling on the data line that
the datasheet suggests in order to detect the end of a write cycle. As it is, the code just waits a (fairly long) while
between writes - the value seems to work for the chips I have. YMMV.

The code clocks the data out LSB first - so, the last flip-flop in the shift register is wired to the lowest data line,
while the first flip-flop is wired to the tenth address line (`A9`).

Two of the Node's pins handle clocking data out to this register (`D0` is data, `D1` is clock). The `D0` is wired 
to the input of the first flip-flop (with a weak pull-down which is probably unnecessary now but was useful during 
manual testing).

The Write Enable pin on the EEPROM is handled by `D2`. There's a weak (10K) pull-up on this line as it's active-low and I 
wanted to prevent accidental writes.

`D3`, `D4` and `D5` handle the EEPROM's `A10`, `A11` and `A12` pins, the three address bits that don't fit in the
shift register.

## Enhancements

At the moment this just writes the same data to every byte. I then hook up the EEPROM to a bunch of LEDs and
use jumpers to drive the address bus. The next step is to allow it receive (via USB serial probably) a bunch
of actual data to write.

It could also get the data over WiFi, but that would just be silly.

