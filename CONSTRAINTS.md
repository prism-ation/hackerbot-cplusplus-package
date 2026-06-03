# C++ Constraints for Hackerbot

## Target platform

- Het beoogde targetplatform is een Raspberry Pi 5.
- Houd rekening met beperkte maar voldoende embedded Linux-resources.
- Ontwerp de code zodat ze op een Pi 5 stabiel en uitlegbaar blijft, niet alleen op een desktop.
- Vermijd onnodig zware runtime-afhankelijkheden of complexe infrastructuur.

## Serial communication

- Gebruik Boost.Asio voor alle seriële communicatie.
- Houd serial-port toegang geconcentreerd in een transport- of low-level driverlaag.
- Gebruik synchrone I/O als standaard, met Boost.Asio als de onderliggende transportlaag.
- Verberg baud rate, parity, stop bits, flow control, timeouts en reconnect-logica achter een kleine API.
- Laat protocol-framing, parsing en foutafhandeling niet in high-level code lekken.

## Robot driver model

- Modelleer de robotsoftware als een high-level driver boven een of meer low-level drivers.
- High-level drivers vertalen intenties en use-cases naar robotacties, sequenties en toestandsovergangen.
- Low-level drivers spreken het apparaatprotocol, versturen commando's en verwerken ruwe antwoorden.
- High-level code mag geen directe afhankelijkheid hebben van een serial port of byte-encoding.
- Dependency flow is eenrichtingsverkeer: high-level -> abstracties/interfaces -> low-level -> Boost.Asio.

## Build system

- Gebruik CMake als enige build system voor het C++-pakket.
- Houd de CMake-structuur voorspelbaar: een root `CMakeLists.txt`, een target voor de bibliotheek,
  een target voor de applicatie of voorbeelden, en een aparte test-subdirectory.
- Zet de C++-standaard expliciet vast en forceer een consistente configuratie.
- Exporteer compile commands zodat tooling en statische analyse eenvoudig blijven.
- Gebruik `configure_file` voor gegenereerde headers of configuratiebestanden als dat nodig is.
- Houd publieke headers in een duidelijke `include/`-structuur en bronbestanden in `src/`.

## CMake pattern based on the reference project

De volgende vorm is het referentiepatroon voor dit project:

```cmake
cmake_minimum_required(VERSION 3.20)

project(Hackerbot
    VERSION 0.1.0
    DESCRIPTION "Hackerbot C++ package"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

configure_file(config.h.in generated/config.h @ONLY)

add_library(${PROJECT_NAME}_lib
    src/Hackerbot.cpp
    src/core/CoreService.cpp
    src/transport/SerialTransport.cpp
    src/protocol/CommandCodec.cpp
    src/protocol/ResponseParser.cpp
    src/base/BaseFacade.cpp
    src/base/BaseDriver.cpp
    src/base/MapsService.cpp
    src/head/HeadFacade.cpp
    src/head/HeadDriver.cpp
    src/head/EyesDriver.cpp
    src/arm/ArmFacade.cpp
    src/arm/ArmDriver.cpp
    src/arm/GripperDriver.cpp
)

target_include_directories(${PROJECT_NAME}_lib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}/generated
)

target_compile_features(${PROJECT_NAME}_lib PUBLIC cxx_std_17)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

find_package(GTest REQUIRED)
enable_testing()
add_subdirectory(tests)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN Doxyfile.in)
    set(DOXYGEN_OUT generated/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(docs ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_OUT}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Generate Doxygen API documentation"
        VERBATIM
    )
else()
    message(STATUS "Doxygen not found, 'docs' target will not be available")
endif()
```

## Testing with GTest

- Gebruik GoogleTest voor unittests.
- Houd tests per domein of laag gescheiden in een duidelijke `tests/`-subdirectory.
- Maak testtargets klein en expliciet, zodat de relatie tussen productiecode en tests zichtbaar blijft.
- Test contracten in plaats van implementatiedetails.
- Gebruik testnamen die de bedoeling van het gedrag direct tonen.
- Voeg per publieke functie of klasse de relevante randgevallen en foutpaden toe.

## Doxygen pattern

- Gebruik Doxygen voor publieke API-documentatie.
- Documenteer publieke headers als contract, niet als implementatiebeschrijving.
- Beschrijf standaard `@brief`, `@pre`, `@post`, `@throws`, `@param` en `@return` waar van toepassing.
- Voeg waar relevant `@invariant` toe voor objecteigenschappen die altijd moeten gelden.
- Houd de docs-generatie als een aparte maar eenvoudige stap in de build.
- Gebruik een `Doxyfile.in` en configureer die via CMake naar een gegenereerd bestand in de build tree.

## Practical boundaries

- Deel configuratie, commando's en statusstructuren op per laag.
- Maak interfaces klein en doelgericht, zodat tests de lagen los van elkaar kunnen verifiëren.
- Houd device-specifieke details in de laag die het apparaat direct kent.
- Gebruik duidelijke namen voor drivers, transporten en protocoltypes die de verantwoordelijkheid van de laag weerspiegelen.