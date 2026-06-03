# Hackerbot C++ Architectuur

Dit document beschrijft de beoogde C++-structuur voor de omzetting van het Python-pakket naar
een didactisch leesbare C++-codebase. Het volgt de huidige instructies in `AGENTS.md`,
`CONSTRAINTS.md`, `STYLE_GUIDE.md` en `DIDAKTIEK.md`.

## Ontwerpprincipes

- Boost.Asio is de enige laag voor seriële communicatie.
- De code blijft bewust eenvoudig, expliciet en goed uitlegbaar voor studenten.
- Elke laag heeft één duidelijke verantwoordelijkheid.
- Public headers bevatten het contract; implementatiebestanden realiseren dat contract.
- Tests verifiëren gedrag, randgevallen en foutpaden per contract.

## Voorgestelde mapstructuur

```text
hackerbot-cplusplus-package/
├── include/
│   └── hackerbot/
│       ├── Hackerbot.hpp
│       ├── core/
│       │   ├── CoreService.hpp
│       │   └── RobotState.hpp
│       ├── transport/
│       │   ├── SerialTransport.hpp
│       │   └── SerialPortConfig.hpp
│       ├── protocol/
│       │   ├── CommandCodec.hpp
│       │   ├── ResponseParser.hpp
│       │   └── MessageTypes.hpp
│       ├── base/
│       │   ├── BaseFacade.hpp
│       │   ├── BaseDriver.hpp
│       │   └── MapsService.hpp
│       ├── head/
│       │   ├── HeadFacade.hpp
│       │   ├── HeadDriver.hpp
│       │   └── EyesDriver.hpp
│       ├── arm/
│       │   ├── ArmFacade.hpp
│       │   ├── ArmDriver.hpp
│       │   └── GripperDriver.hpp
│       └── common/
│           ├── Result.hpp
│           ├── Pose.hpp
│           ├── VersionInfo.hpp
│           └── Capabilities.hpp
├── src/
│   └── hackerbot/
│       ├── Hackerbot.cpp
│       ├── core/
│       │   └── CoreService.cpp
│       ├── transport/
│       │   └── SerialTransport.cpp
│       ├── protocol/
│       │   ├── CommandCodec.cpp
│       │   └── ResponseParser.cpp
│       ├── base/
│       │   ├── BaseFacade.cpp
│       │   ├── BaseDriver.cpp
│       │   └── MapsService.cpp
│       ├── head/
│       │   ├── HeadFacade.cpp
│       │   ├── HeadDriver.cpp
│       │   └── EyesDriver.cpp
│       └── arm/
│           ├── ArmFacade.cpp
│           ├── ArmDriver.cpp
│           └── GripperDriver.cpp
└── tests/
    └── unit/
        ├── test_core.cpp
        ├── test_transport.cpp
        ├── test_protocol.cpp
        ├── test_base.cpp
        ├── test_maps.cpp
        ├── test_head.cpp
        ├── test_arm.cpp
        └── test_hackerbot.cpp
```

## Laagindeling

### 1. `transport`

Verantwoordelijk voor de seriële verbinding zelf.

Voorgestelde klassen:

- `SerialPortConfig`: baudrate, parity, stopbits, flow control, timeout.
- `SerialTransport`: opent/sluit de poort, leest, schrijft en bewaakt verbindingsstatus.

Deze laag kent alleen Boost.Asio en geen robotdomein. Ze vormt de kleinste herbruikbare laag.

### 2. `protocol`

Verantwoordelijk voor framing, commandostrings, parsing en response-validatie.

Voorgestelde klassen:

- `CommandCodec`: maakt van domeininput een tekstcommando.
- `ResponseParser`: zet een ruwe regel of JSON-response om naar een typeveilig resultaat.
- `MessageTypes`: kleine value types en enums voor commandos, status en errors.

Deze laag kent geen hardwareverantwoordelijkheid en geen use-case logica.

### 3. `base`, `head`, `arm`

Verantwoordelijk voor hardware-specifieke drivers en facades.

Voorgestelde indeling:

- `BaseDriver`: low-level basecommando's, status, motion, docking, mapping triggers.
- `BaseFacade`: high-level basisgedrag zoals rijden, starten, docking en map-workflows.
- `MapsService`: map-listing, map fetch en navigatie-logica boven de base-driver.
- `HeadDriver`: low-level hoofd- en eye-commando's.
- `HeadFacade`: hoge-level acties zoals kijken, idle mode en sequenties.
- `EyesDriver`: expliciete driver voor gaze-achtige commando's als dat later losgetrokken wordt.
- `ArmDriver`: low-level armcommando's.
- `ArmFacade`: high-level armacties en samengestelde bewegingen.
- `GripperDriver`: gripper open/close/calibrate.

De huidige Python-code laat zien dat deze verantwoordelijkheden nu nog samen zitten in één helperlaag;
in C++ worden ze bewust gescheiden zodat de software eenvoudiger te begrijpen en te testen is.

### 4. `core`

Verantwoordelijk voor globale robotinformatie en capability-detectie.

Voorgestelde klassen:

- `CoreService`: ping, version en capability status.
- `RobotState`: overzicht van aangesloten componenten en globale status.

Deze laag leest status van de robot, maar bestuurt geen hardwareacties.

### 5. `Hackerbot`

De publieke façade voor applicatiecode.

Voorgestelde klasse:

- `Hackerbot`: assembleert transport, protocol, core, base, head en arm in een eenvoudige,
  leesbare entry point.

Deze klasse is bewust dun: hij maakt objecten aan en biedt een duidelijk startpunt voor gebruikers.

## Concrete verantwoordelijkheid per component

### `SerialTransport`

- openen en sluiten van de serial port;
- lezen en schrijven van bytes of regels;
- timeout- en reconnectgedrag op transportniveau;
- foutmeldingen voor hardware- en I/O-problemen.

### `CommandCodec`

- stringformatting van commando's;
- optionele parameters valideren;
- commandonamen en argumentvolgorde expliciet maken.

### `ResponseParser`

- JSON of tekstuele responses lezen;
- commandokoppeling controleren;
- success/failure en payload uit de response halen.

### `BaseDriver`

- `B_INIT`, `B_START`, `B_DRIVE`, `B_DOCK`, `B_STATUS`, `B_POSE`, `B_MAPLIST`, `B_MAPDATA`;
- toestand van de base vertalen naar een typeveilig resultaat;
- geen policies of workflows die niet direct aan de base zelf horen.

### `BaseFacade`

- combinatie van start, drive, dock en navigatie-acties;
- bewuste orchestratie, maar zonder directe serial-kennis;
- bewaakt gebruikersvriendelijke gedragspaden.

### `HeadDriver` en `ArmDriver`

- direct apparaatprotocol;
- geen applicatielogica;
- slechts de commando's en responses van het apparaat.

### `HeadFacade` en `ArmFacade`

- leest attachments/capabilities uit `RobotState`;
- biedt leesbare functies zoals look, gaze, moveJoint en moveJoints;
- orkestreert alleen waar nodig.

## Teststructuur

Per laag een eigen testbestand, in lijn met de didactische specificatie.

- `test_transport.cpp`: verbinding, schrijven, lezen, foutpaden.
- `test_protocol.cpp`: formatting en parsing van commando's en responses.
- `test_core.cpp`: ping, version en capability status.
- `test_base.cpp`: start, drive, dock, pose, map en status.
- `test_maps.cpp`: maplist, mapdata en navigatiegedrag.
- `test_head.cpp`: look, idle mode, gaze en capability checks.
- `test_arm.cpp`: joint bewegingen en gripperacties.
- `test_hackerbot.cpp`: samenstelling van de publieke façade.

Tests moeten de contractregels uit de headers volgen: `@pre`, `@post`, `@throws` en waar relevant
`@invariant`.

## Aanbevolen migratievolgorde

1. Bouw eerst `transport` en `protocol`, omdat alles daarop leunt.
2. Migreer daarna `core` en `base`, omdat daar de meeste gedeelde robotlogica zit.
3. Splits vervolgens `head` en `arm` in facade + driver.
4. Voeg `maps` als aparte service boven `base` toe.
5. Maak tot slot `Hackerbot` als dunne samensteller en public entry point.

## Didactische afweging

Deze structuur is expres niet maximaal generiek. Dat is een bewuste keuze:

- studenten kunnen de control flow volgen;
- uitbreidingspunten zijn zichtbaar;
- tests kunnen per laag worden besproken;
- de code kan later eenvoudig worden vereenvoudigd of opgesplitst voor opdrachten.

Als een abstractie de uitleg ingewikkelder maakt dan de oplossing zelf, dan hoort die abstractie
hier niet thuis.