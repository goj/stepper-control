/*
 * This code is based on hid-custom-rq example by Christian Starkjohann from V-USB samples.
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH (hid-custom-rq)
 * Copyright: (c) 2012 by Krzysztof Goj (later changes)
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 */

#define STEPPER_X_PORT_OUTPUT     PORTC
#define STEPPER_X_PORT_DDR        DDRC
#define STEPPER_X_DDR_MASK        (_BV(5) | _BV(4) | _BV(3) | _BV(2))
#define STEPPER_X_SHIFT           2u

#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#include <avr/pgmspace.h>   /* required by usbdrv.h */

#include "usbdrv.h"
#include "oddebug.h"        /* This is also an example for using debug macros */
#include "requests.h"       /* The custom request numbers we use */

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

const PROGMEM char usbHidReportDescriptor[22] = {    /* USB report descriptor */
    0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x09, 0x00,                    //   USAGE (Undefined)
    0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
    0xc0                           // END_COLLECTION
};
/* The descriptor above is a dummy only, it silences the drivers. The report
 * it describes consists of one byte of undefined data.
 * We don't transfer our data through HID reports, we use custom requests
 * instead.
 */

static int request_count = 0;
static volatile int adc_count = 0;
static volatile int temperature = 0;
static char *last_error = "no error";

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
    usbRequest_t    *rq = (void *)data;
    static char dataBuffer[100];
    int size;
    /* default for not implemented requests: return no data back to host */
    if((rq->bmRequestType & USBRQ_TYPE_MASK) != USBRQ_TYPE_VENDOR)
        return 0;
    ++request_count;
    switch (rq->bRequest) {
        case REQ_GET_STATUS:
            dataBuffer[0] = (request_count >> 8) & 0xFF;
            dataBuffer[1] = (request_count >> 0) & 0xFF;
            dataBuffer[2] = STEPPER_X_PORT_OUTPUT >> STEPPER_X_SHIFT;
            dataBuffer[3] = 0; // STEPPER_Y_PORT_OUTPUT >> STEPPER_Y_SHIFT;
            dataBuffer[4] = (temperature >> 8) & 0xFF;
            dataBuffer[5] = (temperature >> 0) & 0xFF;
            usbMsgPtr = (uchar*) dataBuffer;
            return 6;
        case REQ_DEBUG:
            size = sprintf(dataBuffer,
                           "Request No: %d\n"
                           "Stepper X: %d\n"
                           "ADCH: %d\n"
                           "ADCL: %d\n"
                           "temperature: %d\n"
                           "adc_count: %d\n"
                           "Last error: %s\n",
                           request_count,
                           STEPPER_X_PORT_OUTPUT,
                           ADCH,
                           ADCL,
                           temperature,
                           adc_count,
                           last_error);
            usbMsgPtr = (uchar*) dataBuffer;
            return size + 1;
        case REQ_SET_X:
            STEPPER_X_PORT_OUTPUT = rq->wValue.word << STEPPER_X_SHIFT;
            return 0;
        case REQ_SET_Y:
            return 0;
        default:
            return 0;
    }
}

#if 1
ISR(ADC_vect) {
    temperature = ((ADCH & 0x3) << 8) | (ADCL & 0xFF);
    ++adc_count;
}
#endif


uchar usbFunctionWrite(uchar *data, uchar len)
{
    return 1; // nothing more to write
}

/* ------------------------------------------------------------------------- */

int __attribute__((noreturn)) main(void)
{
    uchar i;

    wdt_enable(WDTO_1S);
    /* Even if you don't use the watchdog, turn it off here. On newer devices,
     * the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
     */
    /* RESET status: all port bits are inputs without pull-up.
     * That's the way we need D+ and D-. Therefore we don't need any
     * additional hardware initialization.
     */
    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    i = 0;
    while (--i) {             /* fake USB disconnect for > 250 ms */
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();

    // set output ports
    STEPPER_X_PORT_DDR |= STEPPER_X_DDR_MASK;
    ADMUX = 0x0; // input on ADC0, use AREF, right adjust the result
    ADCSRA = _BV(ADEN) | _BV(ADFR) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    // enable ADC, free running mode, set prescaler to 128
    ADCSRA |= _BV(ADIE); // enable interrupts
    ADCSRA |= _BV(ADSC); // turn on the conversion

    sei();

    for (;;) {
        wdt_reset();
        usbPoll();
    }
}

/* ------------------------------------------------------------------------- */
