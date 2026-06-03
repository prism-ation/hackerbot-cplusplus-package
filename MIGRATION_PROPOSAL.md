# Migratievoorstel: Python package naar C++

Dit voorstel beschrijft hoe de huidige Python-package stapsgewijs kan worden omgezet naar een didactisch leesbare C++-codebase. Het sluit aan op `AGENTS.md`, `PATTERNS.md`, `STYLE_GUIDE.md`, `DIDAKTIEK.md` en `ARCHITECTURE.md`.

## Doel

De migratie moet een C++-variant opleveren die:

- functioneel de huidige robotfunctionaliteit behoudt;
- Boost.Asio gebruikt voor seriële communicatie;
- duidelijk gestructureerd is voor onderwijsgebruik;
- contracten in headers en verificatie in tests zichtbaar maakt;
- per laag uitbreidbaar blijft zonder onnodige abstractie.

## Hoofdkeuzes

1. Seriële communicatie verhuist naar een kleine transportlaag op basis van Boost.Asio.
2. De huidige alles-in-één helperlaag wordt opgesplitst in transport, protocol, core state en device-specifieke drivers.
3. High-level gedrag blijft gescheiden van low-level apparaatcommando's.
4. CMake, GTest en Doxygen zijn vaste onderdelen van de C++-opzet.
5. De migratie gebeurt incrementeel, zodat gedrag per stap verifieerbaar blijft.

## Functionele mapping van Python naar C++

| Python onderdeel | Rol nu | C++ bestemming |
|---|---|---|
| `utils/serial_helper.py` | seriële poort, read loop, JSON buffering | `transport/SerialTransport` + `protocol/ResponseParser` |
| `utils/hackerbot_helper.py` | globale robotstate, JSON mode, logging | `core/RobotState`, `core/CoreService`, samenstellende context |
| `core.py` | ping en version | `core/CoreService` |
| `base/__init__.py` en `base/maps.py` | base-commando's en navigatie | `base/BaseDriver`, `base/BaseFacade`, `base/MapsService` |
| `head/__init__.py` en `head/eyes.py` | head, idle, gaze | `head/HeadDriver`, `head/HeadFacade`, `head/EyesDriver` |
| `arm/__init__.py` en `arm/gripper.py` | arm en gripper | `arm/ArmDriver`, `arm/ArmFacade`, `arm/GripperDriver` |
| `__init__.py` | publieke samenstelling | `Hackerbot` façade |

## Voorgestelde migratiefasen

### Fase 1: Fundament

Doel: het transport- en buildfundament neerzetten.

Werkpakket:

- `CMakeLists.txt` opzetten voor library, executable, tests en docs;
- `transport/SerialTransport` implementeren;
- `protocol/CommandCodec` en `protocol/ResponseParser` toevoegen;
- basis testinfrastructuur met GTest inrichten;
- Doxygen-configuratie koppelen aan CMake via `doxyfile.in`.

Resultaat:

- seriële I/O is beschikbaar;
- de C++-projectstructuur kan bouwen en testen;
- documentatie kan gegenereerd worden.

### Fase 2: Core en state

Doel: globale robotstatus en capability-detectie isoleren.

Werkpakket:

- `core/RobotState` definiëren;
- `core/CoreService` implementeren voor ping en version;
- logica voor component-attached status expliciet maken;
- tests voor success- en foutpaden schrijven.

Resultaat:

- de globale toestand van de robot wordt centraal beheerd;
- subsystemen kunnen status lezen zonder transportdetails te kennen.

### Fase 3: Base en navigatie

Doel: beweging, docking en maps structureren.

Werkpakket:

- `base/BaseDriver` maken voor directe base-commando's;
- `base/BaseFacade` maken voor gebruikersgerichte acties;
- `base/MapsService` maken voor maplist, mapdata en pose-workflows;
- posities en status teruggeven als duidelijke value types.

Resultaat:

- de complexe base-logica wordt opsplitsbaar;
- navigatie blijft begrijpelijk en testbaar;
- high-level code spreekt niet direct met de serial port.

### Fase 4: Head en eyes

Doel: head- en gaze-acties scheiden in duidelijke lagen.

Werkpakket:

- `head/HeadDriver` maken voor low-level headcommando's;
- `head/EyesDriver` toevoegen voor gaze-achtige commando's als dat logisch blijft;
- `head/HeadFacade` maken voor idle, look en coördinatie;
- capability checks koppelen aan `RobotState`.

Resultaat:

- hoofdfunctionaliteit is afzonderlijk uit te leggen;
- eye- en headgedrag kunnen los getest worden.

### Fase 5: Arm en gripper

Doel: armcommando's en gripperacties scheiden.

Werkpakket:

- `arm/ArmDriver` maken voor directe joint-commando's;
- `arm/GripperDriver` maken voor calibrate/open/close;
- `arm/ArmFacade` maken voor samengestelde armbewegingen;
- contracten voor validatie en foutafhandeling vastleggen.

Resultaat:

- arm en gripper zijn modulair;
- de API blijft begrijpelijk voor studenten.

### Fase 6: Publieke façade en afwerking

Doel: de uiteindelijke gebruikersinterface samenstellen.

Werkpakket:

- `Hackerbot` als dunne samensteller implementeren;
- import- en initialisatievolgorde vastleggen;
- voorbeeldprogramma en evt. kleine CLI toevoegen;
- testdekking uitbreiden naar integratieniveau voor de samenstelling.

Resultaat:

- een enkele, duidelijke entry point voor applicatiecode;
- systeem kan als referentieproject worden gebruikt.

## Aanpak per iteratie

Elke migratiestap volgt hetzelfde ritme:

1. bepaal het doel van de laag;
2. schrijf de publieke header met contract;
3. implementeer de laag;
4. schrijf tests voor contract en foutpaden;
5. verbind de laag met de volgende hogere laag.

Dat voorkomt dat implementatiedetails te vroeg de structuur bepalen.

## Teststrategie

- Schrijf tests per laag, niet per “grote class”.
- Gebruik mocks of fakes voor onderliggende lagen.
- Test niet alleen happy paths, maar ook tijdslimieten, afwezigheid van responses, foutmeldingen en invalid input.
- Houd tests leesbaar en didactisch: elk testbestand moet een herkenbaar onderwerp hebben.

## Risico's en aandachtspunten

- De Python-code gebruikt veel shared mutable state; dat moet in C++ expliciet worden herontworpen.
- De huidige commandostructuur is sterk string-gebaseerd; een te snelle migratie kan leiden tot verspreide parsinglogica.
- Sommige domeinen, zoals base en maps, mengen gedrag en transport; die moeten in C++ bewust worden gescheiden.
- Een te generieke driverarchitectuur zou de didactische waarde verlagen; eenvoud gaat voor abstractie.

## Aanbevolen startpunt

Begin met het transport- en protocolfundament, en migreer daarna core en base.

Dat is de beste volgorde omdat:

- vrijwel alle Python-modules daarvan afhankelijk zijn;
- transport- en protocolproblemen vroeg zichtbaar worden;
- daarna de hogere lagen relatief rechtlijnig kunnen volgen.

## Kort advies

Als de migratie klein en beheersbaar moet blijven, pak dan eerst deze drie onderdelen op:

1. `SerialTransport`
2. `ResponseParser`
3. `CoreService`

Daarna kan de rest van de robotlaag op dezelfde basis volgen.
