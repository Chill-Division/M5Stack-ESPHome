### Glass 2 Unit w/ 1.51inch Transparent OLED

https://shop.m5stack.com/products/glass-2-unit-w-1-51inch-transparent-oled

<pre>
i2c:
sda: 26
scl: 32
scan: true
id: bus_1

font:
  - file: "gfonts://Roboto@medium"
    id: font_m5stack_atom_echo_80313c
    size: 18
    glyphs: '!"%()+,-_.:°0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz/³µ'

display:
  - platform: ssd1306_i2c
    model: "SSD1306 128x64"
    address: 0x3C
    lambda: |-
      it.print(0, 0, id(font_m5stack_atom_echo_80313c), "Hello");
      it.print(0, 20, id(font_m5stack_atom_echo_80313c), "World!");
</pre>
