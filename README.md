![HackerBot](images/transparent_hb_horizontal_industries_.png)

## Inleiding

Hackerbot is een modulair robotplatform dat robotica toegankelijker wil maken voor ontwikkelaars,
hobbyisten en onderwijs. De nadruk ligt op code boven circuitcomplexiteit: de software abstraheert
de onderliggende robotdetails zodat je je kunt richten op gedrag, navigatie, interactie en
experimenten.

Meer informatie over het platform, de beschikbare modellen en het ecosysteem vind je op
[hackerbot.co](https://www.hackerbot.co/).

Deze repository bevat de C++-variant van Hackerbot. Het project volgt een duidelijke laagindeling
met onder meer transport, protocol, core state en een publieke façade, en is opgezet met CMake,
GoogleTest en Doxygen.

## Raspberry Pi 5 cross-compilatie

Deze repository bevat [toolchain-rpi.cmake](toolchain-rpi.cmake) voor cross-compilatie naar een
Raspberry Pi 5-target.

Benodigdheden op de hostmachine:

- een `aarch64-linux-gnu` cross-compiler (`gcc` en `g++`);
- CMake;
- optioneel een Raspberry Pi sysroot als je headers en libraries uit een gekopieerd target-bestandssysteem wilt gebruiken.

Basisstappen voor configureren en bouwen:

```bash
cmake -S . -B build-rpi -DCMAKE_TOOLCHAIN_FILE=toolchain-rpi.cmake
cmake --build build-rpi
```

Als je cross-compiler een andere prefix gebruikt, kun je die tijdens configuratie overschrijven:

```bash
cmake -S . -B build-rpi -DCMAKE_TOOLCHAIN_FILE=toolchain-rpi.cmake \
  -DRPI_TOOLCHAIN_PREFIX=arm-linux-gnueabihf
```

Als je een Raspberry Pi sysroot hebt, kun je die ook meegeven:

```bash
cmake -S . -B build-rpi -DCMAKE_TOOLCHAIN_FILE=toolchain-rpi.cmake \
  -DRPI_SYSROOT=/path/to/raspberry-pi-sysroot
```

Het resulterende executable-bestand kun je naar de Raspberry Pi kopiëren en daar uitvoeren.

### Presets

Er is ook een [CMakePresets.json](CMakePresets.json) toegevoegd zodat je snel kunt schakelen tussen
een build op je eigen machine en een cross-build voor de Raspberry Pi.

Host-build en tests:

```bash
cmake --preset host
cmake --build --preset host
ctest --preset host
```

Raspberry Pi 5 cross-build:

```bash
cmake --preset rpi
cmake --build --preset rpi
```

Het `rpi`-preset is standaard alleen voor builden. De unittests zijn bedoeld om op de hostbuild te
draaien, tenzij je later een target-side testopzet toevoegt.