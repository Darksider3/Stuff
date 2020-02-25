## Aufgabenergebnis

#### Eingesetzte Technologien / Frameworks

Folgende Technologien setze ich in meinem Projekt ein:

- JavaScript ES6
- HTML 5
- CSS 3
- SVG

Ich habe mich für pures, selbstgeschriebenes, Javascript entschieden, da ich seit langem keines mehr geschrieben hatte und dies ja durchaus eine Challenge sein sollte.

Daher kam hier weder ein Framework noch sonstige externe Bibliotheken zum Einsatz.

#### Eingesetzte 3rd Party Libraries

Keine eingesetzt.

#### Installation / Ausführen des Projektes

Man starte einen beliebigen Webbrowser, exemplarisch nehme ich Firefox an. Navigiere zu dem Projektverzeichnis.

```console
$ git clone <linktorepository> udg-probeaufgabe
$ cd udg-probeaufgabe
$ firefox ./index.html
```

---

Folgende Komponenten müssen lokal installiert sein:
- Ein Webbrowser
 
---

#### Überblick der Programmstruktur

```

 - /
  - index.html
    * Projekt Startseite
    
  
  - js
    * Javascript Klassen/Code
    
    - main.js
      * Eintrittspunkt (vgl. aus C z.B. die main()))
    
    - SVGgrid.js
      * SVGGrid-Objekt
      * Togglen des SVG Hintergrundgrafik, Resizing
 
    -  Rects.js
      * Rects-Objekt
      * Handling von Rechtecken als Pixel
      * Funktionen zum Zeichnen, Füllen, Resizing der Rects
        bei Grid-Umstellung z.B., etc.
        
    - helpers.js
      * HTML, Rects und SVG-Helper Funktionen
      
    - events.js
      * Event handler für GUI/Mouse handling
        Downloads der erstellten Grafiken(BMP, PNG, JPEG)
    
 
  - css (Cascading style sheets, Styling)
 
    - main.css
      * CSS für den Rest der Hauptseite (Navigation, main)
  
    - buttons.css
      * Button styles und Animationen
    
 
   - svg (Grids in SVG)
    
    - 8x8.svg (8x8 Grid)
    
    - 16x16.svg (16x16 Grid)
    
    - 32x32.svg (32x32 Grid)
    
    
```

#### Zeit und Vorgehen
Der Arbeits- und Zeitaufwand gliedert sich wie folgt auf:
  * Recherce: 2 Stunden

  
  * Umsetzung: 10 Stunden
    * 30 Minuten SVGs/Grid
    * 3 Stunden Rects
    * Auffrischen von den alten Javascript Kenntnissen

  
  * Dokumentation: 4 Stunden
    * 2 Stunde Code Documentation
    * 2 Stunden Diese README
    
    
  * Testing: 30 Minuten
    * Getestet in 
      * Firefox 73.0 ESR - 75.a1 (Nightly)
      * Chrome 80.0
      * Opera 66.0
      
Die gesamte Arbeitszeit betrug ca. ***16 Stunden***, wobei ein Großteil auf das wieder vertraut machen mit Javascript entfiehl.


#### Fazit und Reflektion
###### Limitierungen und mögliche Verbesserungen

##### Limitierungen
In diesem Projekt gibt es, wie in dem vorherigen auch, einige offensichtliche und einige weniger offensichtliche Verbesserungsmöglichkeiten und Limitierungen durch das gewählte Design, die während der Entwicklung aufgefallen sind. Auf diese würde ich gerne eingehen.

Der größte Punkt, mit Abstand, ist die gewählte Form des Grids.

Da das Grid per Javascript als Hintergrundbild des Canvas bestimmt wird, lassen sich die SVGs nicht mehr mit Javascript manipulieren/ansprechen, anders als bei `iframes` oder `objects`. Was dazu führt, dass ich mehrere Grid-Versionen vorliefern muss und diese nicht mehr dynamisch ändern kann. Auch wenn das Design der Grids/SVG-Dateien selbst genau darauf ausgelegt ist, was eines der Hauptmankos dieses Projektes ist.
Eine weitere möglichkeit wäre natürlich, dynamisch eine Data-URL zu erzeugen, die alle nötigen Parameter enthält(z.B. `scale="5"` um dem User die möglichkeit zu geben das "Zeichenfeld" zu skalieren).


Aufgrund der größe des Projekts wurde kein Unit-Testing umgesetzt. Die Version wurde aber Browserübergreifend getestet und extra Zeit für diesen Schritt in anspruch genommen. Auch ein offensichtlicher Verbesserungspunkt, wobei sich natürlich sowas wie Mocha als Testing-Framework anbietet.

Das CSS könnte noch in SCSS/SASS umgeschrieben und dann konvertiert werden, wenn Änderungen gemacht wurden, allerdings hätte dies zu einer Dependency für dieses Projekt geführt, was es zu verhindern galt.

##### Verbesserungen
* Unit-Testing!
* Die Klassen/Objekte als Module exportierbar machen.
* Die Debugging-Funktion ausweiten.
  *  so das sie optional auch die vorher angesprochenen Unit-Tests automatisch durchgeführt und dem Entwickler dabei die erforderlichen Rückmeldungen, mit wenig Arbeitsaufwand, geliefert werden können.
* CSS zu SCSS/SASS migrieren
* SVG als Data-URL erzeugen oder als `<object>` einbinden, ODER als Layer mit Canvas selbst zeichnen
* Weitere Zeichentools(Linien, Rechteecke, "Kreise", Vierecke)

##### Fazit

Die Aufgabenstellung erforderte sich in Javascript, erneut, einuzarbeiten, was aber durch die Struktur der Aufgabe leicht möglich war und mir erlaubte neue Standards zu verwenden.

Die Arbeit mit Canvas war interessant, da ich Rectangulars als Pixel reinterpretieren und daher die Abstraktionsebene erhöhen musste. Das erlaubte aber auch ein effizienteren Algorithmus zum Flood Filling der "Pixel".
Das Flood-Filling war mit abstand das herausfordernste. Auf GitHub hatte ich gesehen, das einige vorgefertigte Bibliotheken dafür verwendet hatte(PaperJS stach mir ins Auge), allerdings ist die Stack-Variante des Flood-Fillings(im Vergleich zur Rekursiven) etwas, was ich noch nicht umgesetzt hatte; Daraus konnte ich eine Menge ziehen!
