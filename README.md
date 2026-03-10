# This is the README
Fragestellung: Kann die Propagierung eines Signals zu der Synchronisation einer Population führen. 

## Infos über Albtross
Ca. alle zwei Jahre brüten
ca. 3 Monate Brutzeit
Kolonie ca. 5000 Brutpaare

## First ideas
Now, I proceed in German - consequently as I am :

Albatross-Projekt 

- Eine Brut Kolonie als Gitter
- Zwei Gitter
- In jedem Gridpoint ein Brutpaar
- Hat eine gewisse Brutindex/Brutlust X
- Ein Gridpoint ob es Brutpaar brütet oder nicht
- Ein Albatross brütet alle 2 Jahre
  -- Brutzylus: 24 nicht brüten, dann 3 Monate brüten 
- Wir machen einen Threshold , wenn der Überschritten wird, fängt es an zu brüten
- Ein hoher Brutindex erhöht die Wahrscheinlichkeit das im zweiten Gitter der Gridpoint auf Brüten gesetzt wird

(1) Initialisierung:
    o	Eine zufällige Brutverteiling im Brutindex Gitter von 1-27
    
(2) Mit jedem Zeitschritt (1 Monat)
-  Erhöht sich die Wahrscheinlichkeit des Brutindex um 1
-  Wenn der Threshold 24 überschritten wird - dann brüten Sie -> Gitter zwei wird auf 1 gesetzte
-  alle die auf 28 gesetzt werden, werden auf null gesetzte -> wenn sie auf null gesetzt werden, dann auch Brutgitter auf null 


-  Nachbarschaftbeziehung


ToDo: 
Johannes: Indexgitter 

Damian: Code aufräumen, zweite Nachbarschaftlogik 
- Schieber
- Parameter während der Laufzeit ändern
- Code aufräumen und kommentieren 
- zweites Indexgitter für richtige aktualsierung

- Weitere 
- zweite Option einbauen für Nachbarschaftsbeziehung
- Ideen sammeln für Wahrscheinlichkeitsbasiertes brüten
- Populations
- 









