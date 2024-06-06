RtcDS1302 object provides access to all the functions of the DS1302 module.  Along with date and time, this also includes retrieving and setting data into the non-volatile memory and defining what the charging settings are set at.

# Constructors

### template\<class T_WIRE_METHOD\> RtcDS1302(T_WIRE_METHOD& wire) 
Construct a Rtc object using the provided WIRE method.  
_T_WIRE_METHOD_ - the typename of the class to use for the wire method.  `ThreeWire` is the normal hardware method class.  
_wire_ - the instance of the T_WIRE_METHOD to use.  You must define an instance of ThreeWire and pass it in here.  
Below is an example of how to create an instance of the object using the provided ThreeWire software Wire support.  The pins used are arbitrary and defined by the sketch author as there is no hardware support.  

```
#include <ThreeWire.h> 
#include <RtcDS1302.h>

ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
```

# Methods
### void Begin()  
The normal begin method that should be called within `Setup()`

### bool GetIsWriteProtected()
\<return\>, if true the RTC has been write protected and will fail all calls that will write to it.

### void SetIsWriteProtected(bool isWriteProtected)
_isWriteProtected_ - sets the RTC to be either write protected or write enabled.

### bool GetIsRunning()  
\<return\>, the actual clock is running on the RTC.  

### void SetIsRunning(bool isRunning)
_isRunning_ - set if the clock is running. If false then the time value will not progress and less power is consumed 

### void SetDateTime(const RtcDateTime& dt)
_dt_ - the date and time to set the RTC to.  

### RtcDateTime GetDateTime()
\<return\>, the current date and time in the RTC.  

### uint8_t GetTrickleChargeSettings()
\<return\>, the trickle charge settings, a combination of flags from `DS1302TcrResistor`, `DS1302TcrDiodes`, and `DS1302TcrStatus`.  

### void SetTrickleChargeSettings(uint8_t setting)
_setting_ - the settings to use for the trickle charge support.  A combination of flags from `DS1302TcrResistor`, `DS1302TcrDiodes`, and `DS1302TcrStatus`.  

### void SetMemory(uint8_t memoryAddress, uint8_t value)
_memoryAddress_ - (0-30) the address within the memory of the RTC to store a value.  
_value_ - the 8 bit value to store.  

### uint8_t GetMemory(uint8_t memoryAddress)
_memoryAddress_ - (0-30) the address within the memory of the RTC to retrieve a value.  
\<return\>, the value of that memory address  

### uint8_t SetMemory(const uint8_t* pValue, uint8_t countBytes)
This will always use the starting address of zero due to limitations of the hardware API.  
_pValue_ - the pointer to a buffer of bytes  
_countBytes_ - the number of bytes to copy into the RTC memory  
\<return\>, the number of bytes actually copied.  This maybe smaller than countBytes due to reaching the end of the available memory address.  

### uint8_t GetMemory(uint8_t* pValue, uint8_t countBytes)
This will always use the starting address of zero due to limitations of the hardware API.  
_pValue_ - the pointer to a buffer of bytes to copy into  
_countBytes_ - the number of bytes to copy from the RTC memory  
\<return\>, the number of bytes actually copied.  This maybe smaller than countBytes due to reaching the end of the available memory address.  
 