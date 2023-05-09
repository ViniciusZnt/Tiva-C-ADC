#include <stdint.h>
#include <stdbool.h>
#include "C:\ti\TivaWare_C_Series-2.2.0.295\inc\hw_memmap.h"
#include "C:\ti\TivaWare_C_Series-2.2.0.295\inc\hw_types.h"
#include "C:\ti\TivaWare_C_Series-2.2.0.295\driverlib\debug.h"
#include "C:\ti\TivaWare_C_Series-2.2.0.295\driverlib\sysctl.h"
#include "C:\ti\TivaWare_C_Series-2.2.0.295\driverlib\adc.h"  //For ADC driver


int main (void){
    //Array for read data from ADC 0 FIFO, will be used sequencer 1
    uint32_t ui32ADC0Value[4];

    //Varibles to storage and calculate the temperature from the sensor
    volatile uint32_t ui32TempAvg;
    volatile uint32_t ui32TempValueC;
    volatile uint32_t ui32TempValueF;

    //Clock Setup
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    //Enable ADC0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    //ADC sequencer configuration
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

    //Config steps on sequencer 1
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);

    //Sample the temperature sensor and configure interrupt flag when the sample is done
    ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);

    //Enable ADC sequence 1
    ADCSequenceEnable(ADC0_BASE, 1);

    while(1){
        //ADC interrupt status flag clearing and ADC trigger
        ADCIntClear(ADC0_BASE, 1);
        ADCProcessorTrigger(ADC0_BASE, 1);

        //Wait to conversion to complete
        while(!ADCIntStatus(ADC0_BASE, 1, false)){
        }

        //Get the ADC sample values from sequencer 1 FIFO
        ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

        //Calculate the average of temperature sensor data adding 2 for rounding
        ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2) / 4;

        //Calculate the average of Temperature in Celsius
        ui32TempValueC = (1475-((2475 * ui32TempAvg)) / 4096) / 10;

        //Calculate the average of Temperature in Fahrenheit
        ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
    }
}
