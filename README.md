# sunset

*Let there be light.*

![image info](img/sketch.png)

Ich möcchte in jede Ecke meines Zimmers einen kleinen Motor stellen welcher jeweils von einem Raspberry Pi gesteuert wird.
jeder Raspberry Pi folgt dabei den Befehlen des *master*-Pi welcher zufällig gewählt wird. Der *master* erstellt einen Pfad und die Laufzeit in welcher das fliegebde Objekt von Punkt `P_start` bis zu `P_ziel` kommt.
Dabei wird allen Raspberry Pi's die nächste Koordinate und Zeit geschickt bis diese erreicht sein muss. Die Pi's müssen daraufhin von ihrer momentanen Position die folgenden Schritte berechnen und ihre jeweiligen Motoren so bedinenen, dass das Objekt so nah wie möglich am berechneten Pfad entlangfliegt.
Die Pi's wissen nichts voneinander und nehmen nur Befehle des Master-Pi entgegen.

Ich habe mir auf Amazon ein zufälliges set von [Motoren](https://www.amazon.de/dp/B0BFDYXYFJ?ref=ppx_yo2ov_dt_b_product_details&th=1) gekauft und hoffe, dass diese reichen werden. Ich nehme nicht an, dass das zu hebende Objekt eine große last darstellt, kleine Motoren sollten reichen.
Die Motoren drehen sich jedoch mit `7000-16000 / m` viel zuschnell um eine präzise Steuerung zu erreichen. Deswegen muss eine [Gearbox](https://en.wikipedia.org/wiki/Transmission_(mechanical_device)) (Transmission) gebaut werden welche die hohe Drehzahl in eine niedrige übersetzt.
Hier hoffe ich mit dem 3D-Drucker einen Prototyp zu bauen und zu testen. Wenn die Gearbox funktioniert, kümmere ich mich darum den Motor per Programmiersprache anzusprechen und zu steuern.

Ich kann mir vorstellen, bis zum 20.04. ein- oder mehrere funktionierende Gearbox(es) zu haben welche ich mit nach Chemnitz nehme. Dann können wir zusammen versuchen den Mathe Teil zu lösen (wie viel muss ein Motor ziehen, damit das Objekt sich an die gewünschte stelle bewegt).
