### I/O Hub 1 to 6 Expansion Unit (STM32F0)

https://shop.m5stack.com/products/i-o-hub-1-to-6-expansion-unit-stm32f0

NOTE: At present only seems to work on the Atom Lite, not the PoESP32 for reasons unknown that we haven't had time to troubleshoot

PbHUB Unit is a 6-channel expansion Unit with I2C control. Each Port B interface is capable of GPIO, PWM, Servo control, ADC sampling, RGB light control and other functions. Adopts STM32F030 for internal control.
![image](https://user-images.githubusercontent.com/162461/226787771-71f4fde0-d306-4885-8c91-d4b4156db560.png)


Under your "esphome" section of your yaml, you'll need to add some additional includes / libraries:
<pre>
esphome:
  name: pbhub-test
  libraries:
    - Wire
</pre>

Then you can add the main configuration for the unit:
<pre>
external_components:
  - source:
      type: git
      url: https://github.com/chill-Division/M5Stack-ESPHome/
      ref: main
    components: m5stack_pbhub

#Pin numberring in the PbHUB (The first digit is the channel number and the second digit is the pin number)
# CHANNEL 0 : 00 and 01
# CHANNEL 1 : 10 and 11
# CHANNEL 2 : 20 and 21
# CHANNEL 3 : 30 and 31
# CHANNEL 4 : 40 and 41
# CHANNEL 5 : 50 and 51
m5stack_pbhub:
  - id: 'M5Stack_HUB'
    address: 0x61 # Base I2C Address
    sda: 16 # I2C SDA Pin (Yellow grove cable)
    scl: 17 # I2C SCL Pin (White grove cable)

# Link the m5stack_pbhub to a switch 
switch:
  - platform: gpio
    name: "Res1 Core"
    pin:
      m5stack_pbhub: M5Stack_HUB
      # Use channel 0, pin number 0
      number: 00
      mode:
        output: true
      inverted: false
  - platform: gpio
    name: "Res1 Grow"
    pin:
      m5stack_pbhub: M5Stack_HUB
      # Use channel 1, pin number 0
      number: 10
      mode:
        output: true
      inverted: false

# Link the m5stack_pbhub to a  binary sensor
binary_sensor:
  - platform: gpio
    name: "Dual Button on PBHUB Channel #2 Pin #0"
    pin:
      m5stack_pbhub: M5Stack_HUB
      # Use channel 2, pin number 0
      number: 20
      mode:
        output: true
      inverted: false
  - platform: gpio
    name: "Dual Button on PBHUB Channel #2 Pin #1"
    pin:
      m5stack_pbhub: M5Stack_HUB
      #Use channel 2, pin number 1
      number: 21
      mode:
        output: true
      inverted: false
</pre>
