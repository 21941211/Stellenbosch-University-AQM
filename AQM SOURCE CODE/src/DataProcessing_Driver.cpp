
#include "DataProcessing_Driver.h"


#include <stdlib.h> // Include the standard library for atof() function

// Bubble sort function for float array
void bubbleSort(float array[], int size)
{
    int i, j;
    float temp;
    for (i = 0; i < size - 1; i++)
    {
        for (j = 0; j < size - i - 1; j++)
        {
            if (array[j] > array[j + 1])
            {
                // Swap array[j] and array[j + 1]
                temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

void bubbleSort16(uint16_t array[], int size)
{
    int i, j;
    float temp;
    for (i = 0; i < size - 1; i++)
    {
        for (j = 0; j < size - i - 1; j++)
        {
            if (array[j] > array[j + 1])
            {
                // Swap array[j] and array[j + 1]
                temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

float trimmedMean(float arr[], int size, int trimCount)
{
    // Calculate the start index of the center values
    int start_index = size / 2 - trimCount / 2;

    // Calculate the end index of the center values
    int end_index = start_index + trimCount - 1;

    // Calculate the sum of the center values
    float sum = 0;
    for (int i = start_index; i <= end_index; ++i)
    {
        sum += arr[i];
    }

    // Calculate the average of the center values
    float average = sum / trimCount;

    return average;
}

uint16_t trimmedMean16(uint16_t arr[], int size, int trimCount)
{
    // Calculate the start index of the center values
    int start_index = size / 2 - trimCount / 2;

    // Calculate the end index of the center values
    int end_index = start_index + trimCount - 1;

    // Calculate the sum of the center values
    int sum = 0;
    for (int i = start_index; i <= end_index; ++i)
    {
        sum += arr[i];
    }

    // Calculate the average of the center values
    int average = sum / trimCount;

    return average;
}



void dataToBuff(char buff[], float data, uint8_t buffSize)
{
    int integerPart = data;
    
    snprintf(buff, buffSize, "%07.2f", data);
    Serial.print("");
    for (size_t i = 0; i < buffSize - 1; i++)
    {
        Serial.print(buff[i]);
    }

    Serial.println("");
}

void dataToBuff16(char buff[], uint16_t data, uint8_t buffSize)
{
    int integerPart = data;
    float adjusted = float(data); // Adjust the value to 2 decimal places
    snprintf(buff, buffSize, "%07.2f", adjusted);
    Serial.print("");
    for (size_t i = 0; i < buffSize - 1; i++)
    {
        Serial.print(buff[i]);
    }

    Serial.println("");
}

void extractValuesFromStringSDI12(String &input, double *array, String &DevAddress, uint8_t bufferSize)
{

    
    uint8_t startIndex = input.indexOf('+', 1) + 1;        // Find the first '+' after the initial '.'
    uint8_t endIndex = input.indexOf('+', startIndex) - 1; // Find the first '+' after the initial '.'

    // Extract substring between '+' characters and convert it to a float
    for (uint8_t i = 0; i < bufferSize; i++)
    {
        String valueString = input.substring(startIndex, endIndex);
        array[i] = valueString.toDouble();
        // Print the value for debugging
        // Serial.println(valueString);
        // Serial.println(array[i], 10);

        // Update startIndex to the position after the current '+'
        startIndex = startIndex = input.indexOf('+', endIndex) + 1;
        ;
        // Find the next '+'
        endIndex = endIndex = input.indexOf('+', startIndex) - 1;
        ;
    }
}
