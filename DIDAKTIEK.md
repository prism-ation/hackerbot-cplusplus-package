# Didactische uitgangspunten

De hackerbot is in de eerste plaats een onderwijsapplicatie voor studenten C++.
De endapplicatie dient daarom niet alleen functioneel correct te zijn, maar ook bruikbaar
te blijven in een didactische setting.

## Hoofddoel

Gebruik deze codebase als een duidelijk, uitlegbaar en onderhoudbaar referentieproject.
Studenten krijgen aan het begin een uitgeklede versie waarin onderdelen ontbreken of bewust
vereenvoudigd zijn. Ontwikkelkeuzes in deze endapplicatie moeten daarom bijdragen aan:

- leerbaarheid;
- uitlegbaarheid;
- uitbreidbaarheid;
- testbaarheid;
- het eenvoudig kunnen afleiden van studentopdrachten.

## Richtlijnen voor ontwerp en implementatie

- Kies voor heldere en idiomatische C++-oplossingen boven compacte of slimme constructies die
  moeilijker uit te leggen zijn.
- Houd de code- en projectstructuur voorspelbaar, zodat studenten logisch kunnen volgen waar
  verantwoordelijkheden liggen.
- Maak uitbreidingspunten herkenbaar. Nieuwe functionaliteit moet bij voorkeur op een manier
  worden toegevoegd die ook in deelopdrachten stapsgewijs opgebouwd kan worden.
- Voorkom onnodige abstractielagen, generieke infrastructuur of optimalisaties als die de
  didactische waarde verlagen.
- Los bugs op een manier op die het onderliggende probleem inzichtelijk houdt, in plaats van
  symptomen te maskeren.
- Schrijf code zodanig dat onderdelen relatief eenvoudig verwijderd, vereenvoudigd of defect
  gemaakt kunnen worden voor onderwijsdoeleinden.
- Geef de voorkeur aan expliciete namen, duidelijke verantwoordelijkheden en goed te volgen
  control flow.
- Houd rekening met testbaarheid, zodat gedrag gecontroleerd en in opdrachten gericht besproken
  kan worden.

## Afwegingskader bij wijzigingen

Beoordeel belangrijke wijzigingen niet alleen op technische kwaliteit, maar ook op didactische
geschiktheid. Stel bij ontwerpkeuzes expliciet de volgende vragen:

1. Is deze oplossing voor studenten logisch te begrijpen?
2. Laat deze wijziging de kern van het probleem en de oplossing duidelijk zien?
3. Blijft de code geschikt als referentie of startpunt voor opdrachten?
4. Kan deze functionaliteit later eenvoudig vereenvoudigd, opgesplitst of aangepast worden voor
   onderwijsgebruik?

Als een technisch elegante oplossing ten koste gaat van begrijpelijkheid of inzetbaarheid in het
onderwijs, heeft de didactisch sterkere oplossing de voorkeur.

## Specificatie en verificatie

In deze codebase staan specificatie en verificatie centraal. Dat betekent dat drie lagen samen
een volledig beeld geven van wat software doet:

- **Header = contract.** De publieke header legt vast wat een klasse of functie garandeert:
  wat er van de aanroeper wordt verwacht (`@pre`), wat na afloop geldt (`@post`), wat er
  gooit bij contractschending (`@throws`) en welke eigenschappen een object altijd heeft
  (`@invariant`).
- **Implementatie = realisatie.** De `.cpp`-bestanden realiseren het contract. De implementatie
  mag niet nodig zijn om te begrijpen wat een functie doet; dat staat in de header.
- **Unittest = verificatie.** De unittests bewijzen dat het contract klopt, inclusief
  randgevallen en foutpaden.

Dit kader helpt studenten leren redeneren vanuit een specificatie naar een verificatie, in plaats
van vanuit een implementatie naar een beschrijving.
