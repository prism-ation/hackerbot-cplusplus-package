# Concrete Takenlijst voor Migratie

Deze takenlijst volgt de voorgestelde fasering uit `MIGRATION_PROPOSAL.md` en `ARCHITECTURE.md`.

## Fase 1: Fundament

- [x] Maak een root `CMakeLists.txt` aan voor het C++-pakket.
- [x] Definieer het library-target voor de core bibliotheek.
- [x] Definieer een executable-target voor een minimale entry point of demo.
- [x] Koppel `include/` als publieke include-directory.
- [x] Koppel `src/` als broncode-structuur.
- [x] Koppel `doxyfile.in` aan CMake via `configure_file`.
- [x] Voeg een `docs` target toe voor Doxygen-generatie.
- [x] Koppel GoogleTest als testframework via CMake.
- [x] Maak een `tests/` subdirectory met een eigen `CMakeLists.txt`.
- [x] Voeg een eerste build- en test-run toe om de basis te verifiëren.

Oplevering:

- project bouwt met CMake;
- tests kunnen draaien;
- Doxygen kan gegenereerd worden.

## Fase 2: Transport en protocol

- [x] Ontwerp `SerialPortConfig` als duidelijke value type of configuratiestructuur.
- [x] Implementeer `SerialTransport` op basis van Boost.Asio.
- [x] Zorg voor openen, sluiten, schrijven en lezen via de transportlaag.
- [x] Voeg timeout- en foutafhandeling toe op transportniveau.
- [x] Implementeer `CommandCodec` voor commandostrings.
- [x] Implementeer `ResponseParser` voor tekstuele of JSON-responses.
- [x] Leg testfakes of mocks vast voor transportafhankelijke tests.
- [x] Schrijf unittests voor succesvolle I/O en foutpaden.

Oplevering:

- seriële communicatie werkt via Boost.Asio;
- protocolvorming en parsing zijn geïsoleerd;
- transport kan los getest worden.

## Fase 3: Core en state

- [x] Definieer `RobotState` voor componentstatus en globale toestand.
- [x] Implementeer `CoreService::ping`.
- [x] Implementeer `CoreService::version`.
- [x] Vertaal attachment- en capability-informatie naar expliciete statusvelden.
- [x] Leg foutgedrag vast voor ontbrekende of ongeldige responses.
- [x] Schrijf tests voor alle combinaties van attached/not attached.

Oplevering:

- robotstatus is centraal en expliciet beschikbaar;
- core-gedrag is contractueel getest.

## Fase 4: Base en navigatie

- [ ] Ontwerp `BaseDriver` voor low-level basecommando's.
- [ ] Implementeer `BaseDriver` voor init, start, drive, dock, status en pose.
- [ ] Ontwerp `BaseFacade` voor gebruikersgerichte acties.
- [ ] Implementeer `MapsService` voor maplist, mapdata en pose-workflows.
- [ ] Splits map- en navigatielogica uit de low-level driver.
- [ ] Definieer value types voor positie, status en mapinformatie.
- [ ] Schrijf tests voor rijden, docken, status en foutafhandeling.

Oplevering:

- base-logica is opgesplitst in driver en facade;
- maps/navigatie is apart testbaar.

## Fase 5: Head en eyes

- [ ] Ontwerp `HeadDriver` voor low-level headcommando's.
- [ ] Ontwerp `EyesDriver` als aparte driver als de scheiding nuttig blijft.
- [ ] Implementeer `HeadFacade` voor look, idle mode en coördinatie.
- [ ] Koppel capability checks aan `RobotState`.
- [ ] Definieer foutmeldingen voor ontbrekende componenten.
- [ ] Schrijf tests voor look, gaze, idle mode en capability checks.

Oplevering:

- hoofd- en ooggedrag zijn gescheiden en uitlegbaar;
- gedrag is per laag getest.

## Fase 6: Arm en gripper

- [ ] Ontwerp `ArmDriver` voor directe joint-commando's.
- [ ] Ontwerp `GripperDriver` voor calibrate, open en close.
- [ ] Implementeer `ArmFacade` voor samengestelde armbewegingen.
- [ ] Leg validatie vast voor joint ranges en snelheidswaarden.
- [ ] Schrijf tests voor elke jointactie en gripperactie.

Oplevering:

- arm en gripper zijn modulair;
- contracten zijn expliciet en getest.

## Fase 7: Publieke façade

- [ ] Implementeer `Hackerbot` als dunne samensteller.
- [ ] Koppel `CoreService`, `BaseFacade`, `HeadFacade` en `ArmFacade` aan de façade.
- [ ] Zorg voor duidelijke initialisatievolgorde.
- [ ] Voeg een minimale demo of voorbeeld toe.
- [ ] Schrijf integratietests voor samenstelling en basisgebruik.

Oplevering:

- er is één duidelijk instappunt voor gebruikerscode;
- subsystemen worden correct samengesteld.

## Fase 8: Documentatie en afronding

- [ ] Voeg Doxygen-commentaar toe aan alle publieke headers.
- [ ] Verifieer dat elke publieke functie `@brief`, `@pre`, `@post` en `@throws` heeft waar van toepassing.
- [ ] Voeg `@invariant` toe waar objecteigenschappen dat vereisen.
- [ ] Controleer of testnamen en testcommentaar de contractelementen volgen.
- [ ] Werk de migratiedocumentatie bij op basis van de uiteindelijke implementatie.
- [ ] Voeg een korte gebruikershandleiding of build-instructie toe als dat ontbreekt.

Oplevering:

- documentatie, tests en code volgen dezelfde contractstijl;
- het project is referentie- en onderwijswaardig.

## Aanbevolen uitvoeringsvolgorde

1. CMake, Boost.Asio transport en testinfrastructuur.
2. Protocol parsing en commandocodering.
3. Core status en versioning.
4. Base en maps.
5. Head en eyes.
6. Arm en gripper.
7. Publieke façade.
8. Doxygen en afronding.

## Minimale eerste sprint

Als je klein wilt starten, pak dan deze taken eerst:

- [x] Root `CMakeLists.txt` opzetten.
- [x] `SerialPortConfig` en `SerialTransport` ontwerpen.
- [x] `CommandCodec` en `ResponseParser` schetsen.
- [x] Eerste GTest testtarget toevoegen.
- [x] Eerste Doxygen target laten werken.
