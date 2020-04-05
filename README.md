# COVID-19 Hue lightshow for support

- [COVID-19 Hue lightshow for support](#covid-19-hue-lightshow-for-support)
  - [Intro](#intro)
  - [How it works](#how-it-works)
  - [Requirement](#requirement)
    - [Required](#required)
    - [Optional (but used in this code)](#optional-but-used-in-this-code)
  - [How to use](#how-to-use)

## Intro

Lightshow using Philips Hue lightbulbs to support every worked close to the COVID-19 virus. Every night at 21h (9 PM), we applause medical, security, retail, law and every workers exposed or fighting against the virus.

This lightshow is triggered by Alexa, warning everyone at home that the lightshow is about to start, thus leaving us time to prepare to applause people at the windows or outside.

When triggered, all the lights turns on for 2 minutes and 30 seconds, using the default "Bright" scene. Then, all lights turn off, 30 seconds before starting.

The lightshow consists of two colors: red and white, first because it's the two colors of the Swiss flag and secondly: because I live in Switzerland.

The lightshow runs until Alexa triggers the end of it at another specific time, stopping the show and restoring the lights previous state.

## How it works

I used Alexa to trigger the lightshow. At a specific time, Alexa will alert everyone at the house that it's time to get ready to cheer all the fantastic people working against or exposed to the Coronavirus. The NodeMCU is triggered via Sinric, as it appears as a switch for Alexa. She will send an "On" signal to Sinric, that will start the timer for the show.

The lightshow turns every lights off 30 seconds before firing, thus Alexa is still warning us about that too.

After a couple of minutes of lightshow and cheering, Alexa will send an "Off" signal to the Sinric switch. This will trigger the end of the lightshow and restore the lights in the house to the normal "Bright" scene from Philips Hue default scenes.

## Requirement

In order to use this code, you'll need the following hardware and API keys:

### Required

1. NodeMCU board
2. Philips Hue API keys (see [here](https://developers.meethue.com/develop/get-started-2/) how to get your keys)
3. Obviously: a Wi-Fi network

### Optional (but used in this code)

1. Sinric API keys
2. Sinric Skill for Alexa

## How to use

Simply fill all Philips Hue and Sinric API keys, enter your Wi-Fi credential, as well as your Hue bridge IP.

Compile the code and upload it to the NodeMCU, and voilà !
