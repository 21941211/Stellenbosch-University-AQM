#include "ADC_Driver.h"



//     // Declare and initialize the float array with the given values
//     float voltageTop[NUMBER_OF_NODES] = {1781.69, 1843.27, 1873.42, 1740.74, 1806.45, 1863.63, 1775.97, 1823.59, 1852.15, 1755.34};

//   float voltageBottom[NUMBER_OF_NODES] = {1274.13, 1322.7, 1342.06, 1249.0, 1295.35, 1336.03, 1275.08, 1310.32, 1316.35, 1250.71};



// float correctVoltage(float v_measured) {
//   // Constants for the linear correction function

// const float vBottomNormal = 0.00;
// const float vTopNormal = 2600.00;
// float VoltageCalTop = 1868.0;
//   float VoltageCalBottom = 1333.0;

//  float m; 
//  float b;




// if (v_measured <= VoltageCalBottom) 
// {
//  m = (VoltageCalBottom - vBottomNormal) / (voltageBottom[NODE_NUMBER-1] - vBottomNormal);
// b = 0; 

// }else if((v_measured >VoltageCalBottom)&&(v_measured <= VoltageCalTop)){
//  m = (VoltageCalTop - VoltageCalBottom) / (voltageTop[NODE_NUMBER-1] - voltageBottom[NODE_NUMBER-1]);
// b = VoltageCalTop - m * voltageTop[NODE_NUMBER-1]; 
// } else{
//   m = (vTopNormal - VoltageCalTop) / (vTopNormal - voltageTop[NODE_NUMBER-1]);
// b = vTopNormal - m * vTopNormal;
// }
//   // Apply the linear correction
//   float v_corrected = m * v_measured + b;

// // Serial.println("Measured Voltage: ");
// // Serial.println(v_measured);

// // Serial.println("Corrected Voltage: ");
// // Serial.println(v_corrected);

//   return v_corrected;
// }



