/**************************************************************
 * Class:: CSC-615-01 Spring 2025
 * Name:: Dammit, Bobby!
 * Student IDs:: Joseph - 923625817
 * Github-Name:: water-sucks
 * Project:: Assignment 5 - RGB Sensor
 *
 * File:: include/button.h
 *
 * Description:: Control the program with a button.
 *
 **************************************************************/

#ifndef BUTTON_H
#define BUTTON_H

#define BUTTON_GPIO -1

/*
 * Idle until a button is pressed.
 * The function expects the button to output to
 * BUTTON_GPIO. If it is -1, it exits immediately.
 */
void waitForButton();

#endif
