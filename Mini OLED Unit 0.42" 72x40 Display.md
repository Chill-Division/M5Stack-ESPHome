### Mini OLED Unit 0.42" 72x40 Display

https://shop.m5stack.com/products/mini-oled-unit-0-42-72x40-display?variant=43973665718529

<pre>
i2c:
  sda: 26
  scl: 32

font:
  - file: "gfonts://Roboto@medium"
    id: font_m5stack_atom_echo_80313c
    size: 18
    glyphs: '!"%()+,-_.:°0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz/³µ'

display:
  - platform: ssd1306_i2c
    model: "SSD1306 72x40"
    address: 0x3C
    lambda: |-
      it.print(0, 0, id(font_m5stack_atom_echo_80313c), "Hello");
      it.print(0, 20, id(font_m5stack_atom_echo_80313c), "World!");
</pre>
