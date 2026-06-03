
Houd je aan de STYLE_GUIDE.md.
Lees ook DIDAKTIEK.md en gebruik die uitgangspunten bij ontwerp- en codekeuzes.
Lees PATTERNS.md voordat je C++-code schrijft in dit pakket.

Voor alle seriële communicatie gebruik je Boost.Asio als transportlaag.
Modelleer de robotsoftware met een high-level driver boven low-level drivers:
high-level code coördineert gedrag en use-cases, low-level code beheert protocol,
framing, parsing, retries en device-specifieke details.
