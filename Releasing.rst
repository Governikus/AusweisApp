Release
=======
Folgende Schritte sind notwendig um ein Tag bzw. Release zu bauen.


Tag anbringen
-------------
#. Mittels "hg update -r stable" auf das jeweilige Changeset wechseln, welches mit einem
   Tag versehen werden soll.

#. Mittels "hg status" überprüfen, dass keine weiteren Änderungen in der Working-Copy
   vorhanden sind.

#. In der Datei CMakeLists.txt die PROJECT_VERSION im PROJECT entsprechend anpassen.

#. Mittels "hg tag x.y.z" (Beispiel: hg tag 1.0.0) den Tag anbringen.

#. Mittels "hg push" ins Main-Repository übertragen.



Tag bauen
---------
Die Release-Jobs müssen nach dem Tag manuell gestartet werden!

Jenkins erstellt das Release anhand des Bookmarks "release" oder des tags/changesets,
welcher als Parameter übergeben wird.
Nachdem die notwendigen Jobs (Windows/macOS/Docs/...) durchgelaufen sind, muss der Job
für den AppCast gestartet werden.

Im Ordner ``resources/jenkins/dsl`` sind die jeweiligen Konfigurationen der Jenkins-Jobs
hinterlegt.



Probleme & Lösungen
-------------------
- Das gebaute Package beinhaltet einen Hashwert im Namen
   - CMake hat erkannt, dass sich die aktuelle Working-Copy nicht auf dem Tag befindet und hängt
     somit den Hashwert des Changesets an. Mittels "hg id -t" kann man überprüfen, ob man sich auf
     dem Tag befindet.

   - Mittels "hg update -r x.y.z" sollte man auf den Tag wechseln.

- Hinter dem Hashwert ist ein +
   - Das bedeutet, dass "uncommitted changes" während des Builds in der Working-Copy enthalten waren.

   - Dies kann man mittels "hg status" überprüfen.

