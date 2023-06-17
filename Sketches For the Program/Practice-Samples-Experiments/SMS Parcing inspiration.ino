String read_sms()
{
int x=0; // sms character counter

String sms = ""; //initialize

unsigned long int prv_millis = millis(); //wait initialization

while(1)
{

if(SIM900.available() >0)
// if(Serial.available() >0)
{
char incoming_sms=SIM900.read();
//char incoming_sms=Serial.read();

sms += incoming_sms; //add up all the characters as string

prv_millis = millis();

x++; // increment sms character counter
}

unsigned long int new_millis = millis();

if((x>0)&&((new_millis - prv_millis)>200)) //if a single letter has been received and no character been
//received since last 200 msec, break
break;
}

//Serial.print("received sms =");
//Serial.println(sms);

int m = 1 + sms.lastIndexOf('"');

// find location of last ' " ' of sms format
// [ +CMGL: 1,"REC UNREAD","+XXXXXXXXXXXX",,"01/01/19,09:55:16+08"
// Test message 1 ]

int n = sms.length(); // find the length of sms

// Serial.print("actual sms =");
//Serial.println(sms.substring(m,n)); // sms text lies between the two

return(sms.substring(m,n)); //return the sms
}
