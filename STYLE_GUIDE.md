## Code Style Guidelines

### Algemeen

- Formatting: zie `.clang-format` in de projectroot.
- Formatter: `clang-format -i <file>`
- Het gebruik van de formatter is verplicht
- Line length: 100 karakters.
- Indentation: 4 spaties, geen tabs.
- Encoding: UTF-8.

### C++ specifiek

- Variabelen en methods: `camelCase`.
- Classes, types en namespaces: `PascalCase`.
- Functieparameters krijgen een `a` of `an` prefix en gebruiken daarna `PascalCase`,
  bijvoorbeeld `aTick`, `aRequestId`, `aFloor` en `anElevator`.
- Lokale variabelen en membervariabelen gebruiken `camelCase` zonder prefix.
- Geen pre- en postfixes in de naam van variabelen, dus bijvoorbeeld geen `m_`, `s_`, `g_`,
  `p`, `str` of vergelijkbare naamdelen die alleen het soort variabele aanduiden.
- Gebruik `const` voor elke lokale variabele en parameter die na initialisatie niet meer wijzigt.
- Gebruik geen `const` op return-by-value types.
- Gebruik `nullptr` voor null pointers, nooit `0` of `NULL`.
- Gebruik `auto` alleen in deze gevallen:
  1. het type staat letterlijk rechts in de initializer, zoals bij iterators uit STL-calls;
  2. de initializer is te lang of te complex om het expliciete type leesbaar te houden;
  3. een lambda-type moet worden opgeslagen.
- Gebruik geen `auto` voor primitieve types, strings, of user-defined value types als het
  expliciete type kort en duidelijk is.
- Gebruik smart pointers alleen voor ownership. Gebruik `std::unique_ptr` bij exclusief ownership
  en `std::shared_ptr` alleen als meer dan één object de lifetime van hetzelfde object beheert.
- Als een pointer niet met `nullptr` geïnitialiseerd mag worden, initialiseer die pointer uit een
  bestaande reference met `&aReference`.
- Als een pointer ook geldig `nullptr` mag zijn, geldt deze regel niet.
- Alle publieke functies, constructors, conversion operators en vrije functies in publieke headers
  krijgen Doxygen-comments in de header.
- Een class-level Doxygen-comment vervangt geen functieniveau-commentaar.
- Werk Doxygen-comments bij als het gedrag, de parameters of de returnwaarde van een functie
  veranderen.
- Doxygen in publieke headers geldt als contractspecificatie, niet alleen als beschrijving.
- Documenteer bij publieke functies standaard expliciet:
  1. `@brief` voor de bedoeling van de functie;
  2. `@pre` voor voorwaarden waaraan de caller moet voldoen;
  3. `@post` voor garanties na succesvolle uitvoering;
  4. `@throws` voor exceptions bij ongeldige invoer, contractschendingen of foutcondities;
  5. `@param` en `@return` waar van toepassing.
- Als een publieke functie geen relevante precondities, postcondities of foutcondities heeft,
  moet dat een bewuste keuze zijn; beschrijf dan alsnog het waarneembare contract zo expliciet
  mogelijk.
- Beschrijf contracten in termen van observeerbaar gedrag en publieke toestand, niet in termen van
  implementatiedetails of private helpers.
- Leg waar relevant ook class-invarianten vast in de class-level Doxygen als die belangrijk zijn
  voor correct gebruik of verificatie.
- Houd de volgorde van functiedeclaraties in de header en functiedefinities in de source hetzelfde.
- Vrije helperfuncties in een anonymous namespace vallen niet onder deze regel.
- Gebruik platformspecifieke code alleen achter expliciete `#if defined(...)` guards.
- Geen inline functies in de headers
- Geen #pragma once maar guards
- Gebruik C++ standaardheaders zoals `<cstddef>`, `<cstdint>` en `<cstdio>`. Gebruik geen
  C-headers zoals `<stddef.h>`, `<stdint.h>` of `<stdio.h>`.
- Geen using namespace in headers
- Gebruik geen C-style casts
- De scope van variabelen moet zo klein mogelijk zijn
- Declareer een variabele zo laat mogelijk, vlak voor het eerste gebruik.
- Initialiseer elke variabele direct bij declaratie.
- Geen meerdere declaraties van variabelen in 1 statement, i.e. int i,j; mag niet
- Gebruik anonieme namespaces voor internal linkage, geen static.
- Gebruik [[nodiscard]] alleen bij functie-declaraties, niet bij functie-definities.

### Unittests

- Elke test bevat direct boven de `TEST`-macro een comment dat vermeldt welk contractelement
  geverifieerd wordt: `@pre`, `@post`, `@throws` of `@invariant`, letterlijk overgenomen uit de
  bijbehorende Doxygen in de header.

### Oplevering van nieuwe functionaliteit

Een functie of klasse is pas volledig als alle drie de volgende onderdelen aanwezig zijn:

1. De header beschrijft het contract met `@brief`, `@pre`, `@post` en `@throws`.
2. De implementatie realiseert dat contract.
3. De unittests tonen aan dat het contract geldt, inclusief randgevallen en foutpaden.

### Testbestandenindeling

| Bestand               | Verantwoordelijkheid                             |
|-----------------------|--------------------------------------------------|
| `test_floor.cpp`      | `Floor`: constructie, conversie, grenzen         |
| `test_building.cpp`   | `Building`: constructie, validatie, opzoeken     |
| `test_elevator.cpp`   | `Elevator`: toestandsmachine, contracten         |
| `test_scheduler.cpp`  | `Dispatcher`: nearest-suitable toewijzing |
| `test_simulation.cpp` | `Simulation`: scenario's, constructiefouten      |
| `test_scenario_io.cpp`| `loadScenarioFromFile`, `generateRandomScenario` |
