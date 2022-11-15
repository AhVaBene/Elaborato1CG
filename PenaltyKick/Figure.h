#include "Hermite.h"
#pragma once

void crea_VAO_Vector(Figura* fig);
void crea_VAO_CP(Figura* fig);
void costruisci_cerchio(vec4 color_top, vec4 color_bot, Figura* cerchio);
void costruisci_prato(vec4 color_top, vec4 color_bot, Figura* prato, int width, int height);
void costruisci_cielo(vec4 color_top, vec4 color_bot, Figura* cielo, int width, int height);
void costruisci_porta(vec4 color_top, vec4 color_bot, Figura* porta, int width, int height);
void costruisci_portiere(Figura* piede, Figura* gamba, Figura* corpo, Figura* mano, Figura* testa, Figura* occhio, Figura* bocca);