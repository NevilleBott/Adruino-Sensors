//
//
//
//

#include "sensor.h"
#include "FlowMeter.h"


/*
Insterrupt Service Routine Flow Meter
*/ 
void FlowMeter::incpulseCount()
{
  // Increment the pulse counter
  if (_intenabled) { pulseCount++; };
}
