NRFX Timer example
##################

Overview
********

This is a simple example showing how to use the nrfx_timer driver to access one of the TIMER modules in the nRF devices. 
The TIMER modules allow you to do more accurate timing than the RTC modules used by the standard Zephyr timer functions, since the timers can be configured to run at up to 16MHz clock speed (vs 32.768kHz of the RTC). 

The example enables the TIMER1 module, and configures an application callback that will be triggered by the TIMER1 interrupt when the compare0 event occurs. 
This can be used to trigger an action at regular intervals with higher accuracy than that possible through the RTC. 

Requirements
************

nRF Connect SDK v2.5.2

Tested on the following boards:
    - nrf52dk_nrf52832
    - nrf52840dk_nrf52840
    - nrf5340dk_nrf5340_cpuapp