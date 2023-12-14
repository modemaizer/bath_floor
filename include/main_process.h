#pragma once

void mainInit();
void mainProcess();
void parseIncomingCommand(char *topic, byte *payload, unsigned int length);
bool getHeaterState();