Contributing
============

Patches
-------

Wir nehmen gerne Patches für die AusweisApp an.
Bitte gehen Sie dafür wie folgt vor:

#. Forken des `Repository`_ in das eigene GitHub-Konto.
#. Aufbauen aller Änderungen auf dem  ``community`` branch.
#. Erstellen von vollständigen Changesets (amend anstatt weiterer commits).
#. Push in einen topic branch im eigenen GitHub-Konto.
#. Erstellen eines Pull-Requests mit dem Zielbranch ``community``.

Erstellte Pull-Requests werden von Governikus in das interne Code-Review
übernommen und mittels des internen "Continuous Integration"-Systems geprüft.
Bei erfolgreicher Prüfung wird die Änderung in den aktuellen Entwicklungszweig
aufgenommen und mit dem nächsten Major-Release veröffentlicht. In der Zwischenzeit
wird die Änderung ebenfalls in dem ``community`` branch angenommen und beim
offiziellen Release synchronisiert.

.. _`Repository`: https://github.com/Governikus/AusweisApp2



Code Style
----------

Wir verwenden `uncrustify` um den Quellcode automatisch zu formatieren.
Mittels CMake wird das Build-Target "format" im jeweiligen "Makefile"
generiert, welches die Formatierung für alle Dateien vornimmt.

.. code-block:: bash

   $ make format

Hierfür muss "uncrustify" im jeweiligen $PATH von CMake vorhanden sein.

.. _`uncrustify`: https://github.com/uncrustify/uncrustify



Lizenz
------

Wir benötigen eine persönliche CLA (contributor license agreement) für
die eingereichten Änderungen.

Siehe `Governikus_CLA.pdf`_ für Details.

.. _`Governikus_CLA.pdf`: Governikus_CLA.pdf
