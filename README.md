# Sakta

24-hour one-hand watchface for Pebble Time 2 (emery, 200x228), inspired by
the slow Jo 17 from slow watches. Cream dial, numerals 0-23 along the edge
with 12 (noon) at the top and 0 (midnight) at the bottom, quarter-hour ticks
inside the numerals, and one thin grey hand that turns once a day.

![](screenshots/emery.png)

No settings.

## Build and install

```
pebble build
pebble install --emulator emery
pebble install --phone <phone ip>     # developer connection on in the Pebble app
```

The bundle is `build/sakta.pbw`; it can also be sideloaded from the phone.

## Store listing

The appstore description is in `DESCRIPTION.txt`.
