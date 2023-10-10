// Button Initialization
OneButton btn = OneButton(BTN_PIN, true, true);

uint8_t stored_bri;
uint8_t TargetBri; // when the button is pressed, this is the new brightness to transition to

void buttonSwitches()
{

    CurrentBri = briRamp.update();
    switch (switchBrightness)
    {
    case 0:
        TargetBri = Bri1;
        break;
    case 1:
        TargetBri = Bri2;
        break;
    case 2:
        TargetBri = Bri3;
        break;
    }

    switch (switchArea)
    {
    case 0:
        lower = 0;
        upper = NUM_LEDS;
        break;
    case 1:
        lower = 40;
        upper = 30;
        break;
    case 2:
        lower = 80;
        upper = 100;
        break;
    }
}

// Picks new colors, then triggers color blending and goes dark,
// to signal that the palette has been changed
void paletteButton()
{

    base_hue1 = random(0, 255);
    base_hue2 = base_hue1 + random(50, 205);
    range = random(5, 15);

    palette_changed = true;
    grant_blend = true;
    speed1 = 2000;
    speed2 = 2000;
    stored_bri = CurrentBri;
    briRamp.go(0, 50, LINEAR);
}

// Activates smooth blending to new brightness
void brightnessButton()
{
    switchBrightness = (switchBrightness + 1) % 3;
    briRamp.go(TargetBri, 750, CIRCULAR_INOUT);
}

// Activates smooth blending to new area
void areaButton()
{
    switchArea = (switchArea + 1) % 3;
    lowerRamp.go(lower, 750, CIRCULAR_INOUT);
    upperRamp.go(upper, 750, CIRCULAR_INOUT);
}