#Led Actuator Application

This application registers Flow, Flow Access, Light Control object and toggle Led on receiving Button press event.

How To Compile

Assuming you have creator-contiki source code with directories constrained-os, packages/led-actuator, packages/libobjects and packages/AwaLWM2M

To build with TI CC2520 6lowpan driver
```
$ cd led-actuator
$ make TARGET=mikro-e USE_CC2520=1
```

To build with Cascoda CA8210 6lowpan driver
```
$ cd led-actuator
$ make TARGET=mikro-e USE_CA8210=1
```

This will generate hex file which can be flashed onto the MikroE clicker.

----

## Contributing

We welcome all contributions to this project and we give credit where it's due. Anything from enhancing functionality to improving documentation and bug reporting - it's all good.

For more details about the Contributor's guidelines, refer to the [contributor guide](https://github.com/CreatorKit/creator-docs/blob/master/ContributorGuide.md).
