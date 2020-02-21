# Projekt: Programmieraufgabe UDG: CSV Workflow

## 1) Einleitung
Über allem Stand der Gedanke eines schnellen Prototypings und einer effizienten Entwicklungweise.

Unter der Berücksichtigung dieser Gesichtspunkte wurden bevorzugt Bibliotheken und Komponenten gewählt, welche Browserübergreifende Kompabilitäten besitzen, und weiträumig getestet sind. In diesem Prozess wurden 3rd Party Libraries verworfen, wenn es so schien, dass diese entweder veraltet, zu umfangreich oder nicht besagten Vorraussetzungen entsprechen würden.
So wie die Aufgabe verstanden wurde und am Telefon besprochen wurde, wird eine einzige Datei mit Server-Side-Logik verlangt, sowie eine einzige "tatsächliche" Seite, die auch Paginationsmöglichkeiten zur Aufteilung des Datensatzes besitzen darf.

### Vorgehen, Aufgabenerfassung und Aufgabenstellung
Am Anfang stand die Evaluierung der Aufgabe und Teilaufgaben, sowie die Analyse auf mögliche Hürden, die Klärung von Rückfragen als auch die Wahl der Programmiersprachen. Hier legte ich mich auf **PHP 7.2** für die Server-Side-Logik fest.


### Recherche und 3rd Party Libraries
Darauf folgte die Recherche nach bereits vorhandenen oder ähnlichen Lösunge und nutzbaren Frameworks oder Fremd-Bibliotheken. Neben den Standards **HTML5** und **CSS3**, fiel die Wahl außerdem auf einige **3rd Party Libraries** auf die ich eingehen will:


###### Bootstrap3 (Standard-Build, Standard-Theme)
**Bootstrap3** bietet out-of-the-box Responsive Web Design. Grids, Tabellen und andere UI-Elemente wie Dropdowns, Suchmasken, Navigationen und Paginations und bildet für, die vom Projekt angeforderte Fragestellung, eine solide Arbeitsgrundlage. Somit entfällt ebenfalls ein Großteil des Testings von und mit browserübergreifenden Layouts und UI-Komponenten.

  
###### JQuery <= 1.9.2
Diese spezifische Version, kompatibel mit IE6+, Chrome 12+, Firefox 39+, wurde unter anderem für die Fremd-Bibliothek **X-Editables** ausgewählt. Im weiteren ist JQuery weit verbreitet, geprüft und bietet zusätzliche und nützliche Funktionen für Selektoren, DOM-Traversal und vielem anderen auch Vereinfachungen für **AJAX-Request**.

JQuery ist im weiteren auch voll zu **Bootstrap3** kompatibel und dient im Projekt ebenso für Request-Forging. Im Detail für die verwendeten **AJAX-Calls** beziehungweise **XmlHttpRequests**.

Zusätzlich dazu stehen mittels **JQuery** in Kombination mit **Bootstrap3** weitere UI-Komponenten zu Verfügung.


###### X-Editables 1.5.0 (Standard Bootstrap3-Build)
Dies ist eine **Drop-In**-Erweiterung für Inline- und Popup-Eingaben und benutzt **JQuery <= 1.9.2** und **Bootstrap3**-Theming.

Sie wird in dem Projekt genutzt um das Editieren der tabellarischen Daten zu ermöglichen. Folgend werden dann über JQuery's **XmlHttpRequest-Funktionen** entsprechende Änderungen an eine speziell designte API übergeben, welche die Daten validiert und speichert.


## 2) Arbeitsbeschreibung

##### Versionierung
Versionierung, im speziellen mit Git, nutze ich aktiv auf Github.com unter der Addresse: https://github.com/Darksider3 

Die dort sichtbaren Projekte zeigen, dass mir das Thema nicht fremd ist und ich bereits umfangreiche Erfahrungen mit der Arbeit mit Git besitze, unter anderem mit Feature-Branches, Commits, Checkouts, Diffs und Patches - sowohl über Gits eigene Funktionen als auch die traditionellen Unix-Tools.

Im Zuge dieses Projekts und mangelnder Arbeitszeit habe ich die Arbeit mit einem lokalen Git-Repository vernachlässigt. Ich bin mir aber bewusst, dass Versionierung ein, sinnvoller, de-facto Standard ist und eine zentrale Rolle in der Softwareentwicklung spielt.


##### Strukturierung
Die Strukturierung des Frontends ergab sich aus der Fragestellung in der Aufgabe zu dem Projekt. Im Gegenzug dazu war bei der Strukturierung des Codes das Prinzip, dass die gesamte Server-Side-Logic möglichst innerhalb einer einzelnen Datei, jedoch so übersichtlich wie möglich, realisiert ist, was im folgenden als **One-File-Design** bezeichnet wird.

Der Code innerhalb der Datei verfolgt die folgende Struktur:
Im oberen Abschnitt sind Funktionen enthalten die unter anderem auch die Kernfunktionen bereitstellen um die CSV-Datei zu importieren, zu validieren und zu exportieren.

Darauf folgend die Funktionen zur Erzeugung der Statistiken und Visualisierungen, welche **CanvasJS** nutzen. Abschließend für die Logik folgt die Verarbeitung, die Serialisierung und Deserialisierung der **AJAX-Requests** um Eingaben und Änderungen zu ermöglichen.

Die letzte Kette im Glied bildet der Seitenaufbau mittels HTML und Funktionen welche die Benutzerinteraktion ermöglichen.


## 3) Hinweise zu den Teilaufgaben

###### 3.1) Dokumentation
Dokumentation des Codes selbst erfolgt immer oberhalb des jeweiligen Elements mit **PHPDoc**.


###### 3.2) Grafiken
Wie bereits erwähnt wurden die Grafiken auf der Seite im Zuge der Entwicklung mit **CanvasJS** erstellt. Die Library bietet verschiedene Darstellungsformen, Manipulationsmöglichkeiten und Animierungsvariationen.

Mit minimalem Aufwand könnte man noch leicht mehr statistische Grafiken erstellen. Zum Beispiel zu der Verteilung der Geschlechterkategorien im Sortiment, popularität von Kragen bei Oberteilen und der Typ von Hosen. 

Interessante Datenpunkte sind weiterhin auch die Distribution der Hauptkategorie-Nr., der Anteil der Fair & Umweltfreundlich-Herstellungsform in Relation zu dem gesamten Sortiment und natürlich eine Landkarte die markiert woher welche Anzahl von Produkten stammen, insbesondere ob der "unbekannt"/"Leere"-Kontinent größer wäre als Amerika und Australien zusammen!

Letztere Visualisierungen könnten ebenfalls eine wertvolle Ergänzung zu der bestehenden Aufgabestellung darstellen, um mehr Informationen aus den Daten zu gewinnen und wäre mit **CanvasJS** mit äußerst wenig Aufwand umsetzbar, widersprach allerdings der Aufgabe.

###### 3.3) Zeit und Vorgehen
Der Arbeits- und Zeitaufwand gliedert sich wie folgt auf:
* Recherche (**3 Stunden**)
  * **1 Stunde**: Evaluierung von möglichen Technologien.
    * **Hinweis**: Entfallen sind Python, Plot.ly-Dart, Flask, Pandas und PyQt5
  * **1,5 Stunden**: Suche nach einer möglichen Fremd-Biblitohek für Inline-Edits
  * **0,5 Stunden**: Abstimmung von und auf, nach Möglichkeit neuste, Software-Versionen


* Umsetzung (**5 Stunden**)
  * **4 Stunden**: PHP (Backend) Logik für CSV Import, Export und Datenverarbeitung
  * **0,5 Stunden**: Planung und Erstellung eines HTML Grundgerüsts
  * **0,5 Stunden**: Javascript und CSS Einbindung und Anpassung


* Dokumentation (**5 Stunden**)
  * **1 Stunde**: Code Dokumentation
  * **4 Stunden**: diese README.md
  
 
Die gesamte Arbeitszeit umfasst **13 Stunden**.


## 4) Fazit und Reflektion

##### Limitierungen und mögliche Verbesserungen
In diesem Projekt gibt es offensichtliche und weniger offensichtliche Verbesserungsmöglichkeiten und Limitierung, die während der Entwicklung aufgefallen sind. Auf diese würde ich gern eingehen.


###### 4.1) One-File-Design
Die Anforderung alle Inhalte auf einer Seite darstellen zu lassen hat sich für die Entwicklung und rasches Prototyping als sehr sinnvoll herausgestellt. Dieser Aufbau läuft jedoch Gefahr bei umfangreicheren Projekten und Aufgaben unübersichtlich zu werden. Speziell wenn mehr Server-Side-Logik oder UI-Funktionalität erforderlich wird.

Verbesserungen stellt die Verwendung eines etablierten MVC/MVVC-Frameworks dar. Durch die Nutzung dessen ließe sich auch die Logik von der Darstellung und der API trennen. Klassen zum Caching, zum Serialisierungshandling und für Request-Objekte bieten sich insofern ebenfalls an. 

Insbesondere möchte ich auf die Verwendung von Verzeichnissen für Funktionen und Hilfsklassen, sowie Daten und Statischen als auch dynamischen Templates hinweisen. Ein mögliches Strukturkonzept für Dateien wäre:

```
├─ Base Directory
├─ composer.json
├─ composer.lock
├─ app.php/index.php
├─ app
│  ├── view
│  ├── model
│  ├── templates
│  └── controller
├─ data
│  ├── UserContent
│  └── OrData
├─ static
│  ├── js
│  │   ├── main.js
│  │   └── js.things.js
│  ├── css
│  │   ├── main.scss
│  │   └── includeFor.scss
├─ include
│  ├── RDBMS
│  │   ├── example.php
│  │   ├── RDBMS.php
│  │   └── ORM.php
│  ├── utility
│  │   ├── CSV.class.php
│  │   ├── serialization.wrapper.class.php
│  │   └── RDBM
```

###### 4.2) Visualisierung der Charts
Durch geschickte Wahl der Komponenten **ChartJS** waren schnelle und professionelle Visualisierung mit wenig Handgriffen möglich. Sollten sich Anforderungen ändern bietet diese Biblitohek außerdem weitere Optionen.

###### 4.3) Serialisierung
Für das Prototyping war die Serialisierung überaus hilfreich und stellte sich als sehr performant heraus. Jedoch, sollte man das Projekt über die aktuelle Dimension erweitern, wären ein anderes Vorgehen sehr wahrscheinlich deutlich sinnvoller. Beispielsweise die Verwendung einer RDBM und ein ORM. Dies sollte aufgrund der relativ wenigen Referenzierungen und relevanten Funktionen keine größere Hürde darstellen und bietet sich daher an.


###### 4.4) Bootstrap3 und JQuery 1.9.2
Bei der Recherche nach möglichen Frameworks fiel die Wahl auf **Bootstrap3** und die UI Komponente **X-Editables**. Diese worden aufgrund weniger bzw. keiner bekannter Fehler gewählt und ermöglichten es außerdem effizent an die Aufgabenstellung heran zu gehen.

Jedoch wäre es mir auch möglich die Funktionalität von X-Editables in einer neueren JavaScript-Versionen (ES6+) zu verwirklichen, ohne eine Fremd-Bibliothek zu nutzen. Wie bereits beschrieben, ist dies allerdings unter dem Gesichtspunkt des Rapid-Prototypings nicht sinnvoll gewesen.


###### 4.5) AJAX/XmlHttpRequests/(Server-Side-) API
Interaktivität mittels **JQuery** und **XmlHttpRequests** stellten sich als sehr performant heraus und liesen sich leicht implementieren, trotz der Verarbeitungszeiten des Server. Die Serialisierung der Array-Objekte zeigte durchaus Performance-Vorteile, die jedoch durch Einsatz einer relationellen Datenbank noch gesteigert werden könnte.


### Fazit

Eine relativ einfache Aufgabe, wenn man bereits Kenntnisse im Umgang mit Programmiersprachen im allgemeinen hat.
Tatsächlich auch eine gute Herausforderung, wenn man es weit genug mit dem Aufwand treibt, da auch gut erweiterbar in vielen Aspekten.
Insgesamt hat das Projekt Spaß gemacht, die Dokumentation hier war aber ingesamt das aufwändigste, obwohl ich normalerweise viel mehr Erfahrung mit Back-End habe als Front-End, und das auch als Arbeitspunkt ggü. JavaScript, CSS und HTML im Front-End bevorzugen würde. Ich habe einfach ein besseres Verständnis davon, als von UI/UX-Themen, Designs zur Useransicht und ähnliches. Dennoch konnte ich mit diesem Projekt wieder PHP nutzen, was ich seit einigen Jahren nicht mehr gemacht hatte, da ich mich in der Zwischenzeit hauptsächlich auf C/C++(11, 14, 17, 20) und Python beschäftigt habe. 
