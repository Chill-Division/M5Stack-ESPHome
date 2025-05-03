### Passive Buzzer Unit

https://shop.m5stack.com/products/passive-buzzer-unit

This unit offers limited functionality. Consider using a dedicated speaker instead, such as Apple HomePod.

<pre>
output:
  - platform: ledc
    id: buzzer_output
    pin: 26
    frequency: 4000 Hz


switch:
  - platform: template
    name: Buzzer
    id: switch_buzzer
    optimistic: false
    restore_mode: ALWAYS_OFF
    turn_on_action:
      then:
        - repeat:
            count: 10000
            then:
              - output.ledc.set_frequency:
                  id: buzzer_output
                  frequency: 3000Hz
              - output.set_level:
                  id: buzzer_output
                  level: 90%
              - delay: 120ms
              - output.turn_off: buzzer_output
              - delay: 200ms
              - output.ledc.set_frequency:
                  id: buzzer_output
                  frequency: 2000Hz
              - output.set_level:
                  id: buzzer_output
                  level: 90%
              - delay: 120ms
              - output.turn_off: buzzer_output
              - delay: 200ms
    turn_off_action: 
      - output.turn_off: buzzer_output # If the buzzer runs for more than 5 seconds, the switch will appear as 'off', but the buzzer will continue running. To stop it, toggle the switch on and then off again quickly

</pre>
