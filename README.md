# Custom firmware for Asus Falchion


The aim of this project is to revive dying asus falchion keyboards.

I bought 2 dead units out of eBay. Different failure but same problem, MCU was fried.

Solution was clear, I had to replace MCUs with P2P compatibles one, and write a new firmware for them !

Two options were available :
1. Use QMK project
2. Build something from scratch using my knowledge and improve my skills

Obviously, I went with the second option :)

### Current dev status

- [X] Find a compatible MCU
- [X] Identify on board ICs
- [X] Key matrix reverse
- [X] Led matrix reverse
- [X] USB HID (in & out)
- [X] First version of key processing
- [X] Enable LEDs
- [X] Control LEDs independently + Fancy layouts
- [ ] Optimize code
- [ ] Test as much as possible and list remaining bugs
- [ ] Enjoy keyoard :)  

**Ongoing Dev**

Download any dfu flasher (dfuse ?)

.bin needs to be flashed at 0x8000000. Or use .hex file.

Enjoy what's available for now ;)
