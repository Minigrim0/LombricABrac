#!/bin/bash

# sudo apt-get install xdotool


KEYS_ESC=$'\e'
KEYS_F1=$'\e'[[A
KEYS_F2=$'\e'[[B
KEYS_F3=$'\e'[[C
KEYS_F4=$'\e'[[D
KEYS_F5=$'\e'[[E
KEYS_F6=$'\e'[17~
KEYS_F7=$'\e'[18~
KEYS_F8=$'\e'[19~
KEYS_F9=$'\e'[20~
KEYS_F10=$'\e'[21~
KEYS_F11=$'\e'[22~
KEYS_F12=$'\e'[23~
KEYS_HOME=$'\e'[1~
KEYS_HOME2=$'\e'[H
KEYS_INSERT=$'\e'[2~
KEYS_DELETE=$'\e'[3~
KEYS_END=$'\e'[4~
KEYS_END2=$'\e'[F
KEYS_PAGEUP=$'\e'[5~
KEYS_PAGEDOWN=$'\e'[6~
KEYS_UP=$'\e'[A
KEYS_DOWN=$'\e'[B
KEYS_RIGHT=$'\e'[C
KEYS_LEFT=$'\e'[D
KEYS_NUMPADUNKNOWN=$'\e'[G




echo -n $'^[[B'
