// * Max telegram length
const int P1_MAXLINELENGTH= 1050;
char telegram[P1_MAXLINELENGTH];

//init
unsigned int currentCRC = 0;

// * Set to store the data values read
long CONSUMPTION;
long CONSUMPTION_REACT;

long RETURNDELIVERY;
long RETURNDELIVERY_REACT;

long ACTUAL_CONSUMPTION;
long ACTUAL_CONSUMPTION_REACT;

long ACTUAL_RETURNDELIVERY;
long ACTUAL_RETURNDELIVERY_REACT;

long L1_INSTANT_POWER_USAGE;
long L1_INSTANT_POWER_DELIVERY;
long L2_INSTANT_POWER_USAGE;
long L2_INSTANT_POWER_DELIVERY;
long L3_INSTANT_POWER_USAGE;
long L3_INSTANT_POWER_DELIVERY;

long L1_REACT_POWER_USAGE;
long L1_REACT_POWER_DELIVERY;
long L2_REACT_POWER_USAGE;
long L2_REACT_POWER_DELIVERY;
long L3_REACT_POWER_USAGE;
long L3_REACT_POWER_DELIVERY;

long L1_INSTANT_POWER_CURRENT;
long L2_INSTANT_POWER_CURRENT;
long L3_INSTANT_POWER_CURRENT;
long L1_VOLTAGE;
long L2_VOLTAGE;
long L3_VOLTAGE;
