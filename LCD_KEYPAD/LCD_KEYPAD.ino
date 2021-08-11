#include <Keypad.h>
#include <LiquidCrystal.h>
#include<EEPROM.h>
#define Rows 4
#define Cols 20
//FOR KEYPAD
//32767
const byte rows=4;
const byte col=4;

char keymap[rows][col]={
  {'7','8','9','A'},
  {'4','5','6','B'},
  {'1','2','3','C'},
  { '*','0','#','D'}
  };

byte Rpins[rows]={7,6,5,4};
byte Cpins[col]={3,2,1,0};
Keypad kpd=Keypad(makeKeymap(keymap),Rpins,Cpins,rows,col);

//FOR LCD
LiquidCrystal lcd(A4, A5, 11, 10, 13, 8);

//FOR CONTROL
char keypressed;
const int maxItemSize = 11;  //longest menu item..includes char \0
#define ManualMenu 0    //Manual Menu
#define AutoMenu 1    //Auto Menu
#define EditMenu 2    //EditMenu

int sensorPin = 12;    // pedal input pin
int ledPin = 9;      // pin for the LED
bool sensorValue=true; //variable to store input from sensorPin

char startMenu[][maxItemSize] = {"ON TIME", "OFF TIME", "FORCE", "QUANTITY","SAVE","RESET"};//Start or edit menu
char manualMenu[][maxItemSize]={"ON    ","FORCE ","QUANTITY"};
char autoMenu[][maxItemSize]={"ON   |","OFF  |","FCE","|QTY "};

unsigned long int Summary[] = {100, 100, 1, 2};//storage for Auto and Edit
unsigned long int SummaryManual[]={100,1,2};//Storage for manual

const int ARRAY_SIZE = 4;
const int STARTING_EEPROM_ADDRESS = 17;

unsigned long autoStart;//time for auto mode (start)
unsigned long autoCurrent;//time for auto mode (current)

//for printing graph
int prevValue=0;
int lastFullChars = 0;
byte row_1[8]={
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000
};
byte row_2[8]={
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000
};
byte row_3[8]={
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100
};
byte row_4[8]={
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110
};
byte row_5[8]={
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

//for edit menu
int cursorLine = 1;//Which line the cursor points to
int displayFirstLine = 1;//which line should be first line
int n=0;
unsigned long int qtyCounter=1;

const int itemsPerScreen =Rows ;  // max rows..
int menuItems;
char *menuSelected = NULL;
int menuOption = 0;

//Functions used by EDITMENU
void move_up()
{//display first line changes
  if ((displayFirstLine == 1) & (cursorLine == 1)) {//first line & first option
    //if (menuItems > itemsPerScreen - 1) {
      displayFirstLine = menuItems - itemsPerScreen + 1;
      //if cursor is in firstline and dispfirstline is also same.When I move up.The fourth opt displays 
   // }
  } else if (displayFirstLine == cursorLine) {
    displayFirstLine--;
    //
  }
//cursor line changes
  if (cursorLine == 1) {
   // if (menuItems > itemsPerScreen - 1) {
      cursorLine = menuItems; //roll over to last item
  //  }
  } else {
    cursorLine = cursorLine - 1;
  }
}//end move_up

void move_down()
{
  if (cursorLine == (displayFirstLine + itemsPerScreen - 1)) {
    displayFirstLine++;//if cursor line exceeds itemsperscreen(item after 4th row) ,displayfirst line is increased.
  }
  //If reached last item...roll over to first item
  if (cursorLine == menuItems) {
    cursorLine = 1;
    displayFirstLine = 1;
  } else {
    cursorLine = cursorLine + 1;
  }
}//end move_down

void returnToEditMenu()//same lines as selectionMainMenu,but without intro display
{
  displayFirstLine = 1;
  cursorLine = 1;
  menuItems = sizeof startMenu / sizeof * startMenu;
  menuSelected = &startMenu[0][0];
  menuOption = EditMenu;
  keypressed=NO_KEY;
}//end returnToEditMenu

void selectionMainMenu()   //used by all there menus while switching menu's
{
  switch(menuOption)
  {
    case 0: 
        lcd.clear();
        menuItems = sizeof manualMenu / sizeof * manualMenu;
        menuSelected = &manualMenu[0][0]; //Start Menu
        menuOption = ManualMenu; //Main Menu
        n=3;
        autoStart=millis();
        autoCurrent=millis();
        while(kpd.getKey()!='D'){
        for(;(autoCurrent-autoStart)<=1000;)
          {
            autoCurrent=millis();
            lcd.setCursor(4,1);
            lcd.print("MANUAL MODE");
            if(kpd.getKey()=='D')
               {menuOption=1;
                break;}
           }break;}
       lcd.setCursor(4,1);
       lcd.print("           ");
       if(menuOption==0)
        display_Manual(menuOption,menuSelected, menuItems, maxItemSize);
       break;
    case 1:
        lcd.clear();
        menuItems = sizeof autoMenu / sizeof * autoMenu;
        menuSelected = &autoMenu[0][0]; //Start Menu
        menuOption = AutoMenu; //Main Menu
        n=4;
        autoStart=millis();
        autoCurrent=millis();
        while(kpd.getKey()!='D'){
        for(;(autoCurrent-autoStart)<=1000;)
        {
          autoCurrent=millis();
          lcd.setCursor(4,1);
          lcd.print("AUTO MODE");
          if(kpd.getKey()=='D')
            {menuOption=2;
            break;}
          }break;}
        lcd.setCursor(4,1);
        lcd.print("         ");
          if(menuOption==1)
            display_Auto(menuOption,menuSelected, menuItems, maxItemSize);
        break;
    case 2:
        lcd.clear();
        menuItems = sizeof startMenu / sizeof * startMenu;
        menuSelected = &startMenu[0][0]; //Start Menu
        menuOption = EditMenu; //Main Menu
        n=4;
        autoStart=millis();
        autoCurrent=millis();
        while(kpd.getKey()!='D'){
        for(;(autoCurrent-autoStart)<=1000;)
          {
            autoCurrent=millis();
            lcd.setCursor(4,1);
            lcd.print("EDIT MENU");
            if(kpd.getKey()=='D')
              {menuOption=0;
              break;}
          }break;}
        break;
}
}

void selectionSubMenu()  //EditMenu options rewriting
{
  lcd.clear();
  switch (cursorLine - 1)//as seen in emenu_display cursorline-1 says where < points to
  {
    case 0:
      //lcd.setCursor(0, 0);
      lcd.print("ON TIME(ms)");
      Summary[cursorLine - 1] = GetNumber();//user input saved in Summary array..to be used later
      if(Summary[cursorLine - 1]>99999)
      {
        Summary[cursorLine - 1]=99999;
      }
      if(Summary[cursorLine - 1]<100)
      {
        Summary[cursorLine - 1]=100;
      }
      SummaryManual[cursorLine-1]=Summary[cursorLine - 1];
      returnToEditMenu();
      break;
    case 1:
      //lcd.setCursor(0, 0);
      lcd.print("OFF TIME(ms)");
      Summary[cursorLine - 1] = GetNumber();
      if(Summary[cursorLine - 1]>99999)
      {
        Summary[cursorLine - 1]=99999;
      }//user input saved in Summary array..to be used later
      if(Summary[cursorLine - 1]<100)
      {
        Summary[cursorLine - 1]=100;
      }
      returnToEditMenu();
      break;
    case 2:
     // lcd.setCursor(0, 0);
      lcd.print("Force(0-255)");
      //if (menuOption != SubMenu4 && menuOption != SubMenu6)   //this is where return option is in the refered SubMenu
        Summary[cursorLine - 1] = GetNumber(); 
        if(Summary[cursorLine - 1]>255)
      {
        Summary[cursorLine - 1]=255;
      }
        if(Summary[cursorLine - 1]<=1)
      {
        Summary[cursorLine - 1]=1;
      }
      SummaryManual[1]=Summary[cursorLine - 1];
      returnToEditMenu();
      break;
    case 3:
      //lcd.setCursor(0, 0);
      lcd.print("Quantity(2-99999)");
      //if (menuOption != SubMenu2)     //this is where return option is in the refered SubMenu
        Summary[cursorLine - 1] = GetNumber();
        if(Summary[cursorLine - 1]>9999)
      {
        Summary[cursorLine - 1]=9999;
      }//user input saved in Summary array..to be used later
      if(Summary[cursorLine - 1]<=2)
      {
        Summary[cursorLine - 1]=2;
      }
      SummaryManual[2]=Summary[cursorLine - 1];
      returnToEditMenu();
      break;
    case 4:
     writeIntoEEPROM(STARTING_EEPROM_ADDRESS, Summary, ARRAY_SIZE);
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Saved..");
     delay(800);
     returnToEditMenu();
     break;
    case 5:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Resetting");
      Reset();
      delay(1000);
      returnToEditMenu();
      break;
 }//end switch
  delay(800);
  returnToEditMenu();
}//end selectionSubMenu

long unsigned int GetNumber()  //to get values from keypad in edit menu
{
   long unsigned int num = 0;
   char key = kpd.getKey();
   while(key != 'C')
   {
      switch (key)
      {
         case 'A':case 'B':case 'D': case '*':case '#':
            break;

         case '0': case '1': case '2': case '3': case '4':
         case '5': case '6': case '7': case '8': case '9':
            num = num * 10 + (key - '0');
            lcd.setCursor(0,2);
            lcd.print(num);
            break;

      }

      key = kpd.getKey();
   }

   return num;
}

long unsigned int getDigits(long unsigned int number)//return no of gdigits in a value
{
int count=0;
if (number==0)
  number=1;
while(number!=0)
{
  number=number/10;
  ++count;
}
  return count;
}

void printGraphChar(unsigned long int presentTime,unsigned long int totalTime)
{
  byte fullChars = presentTime * Cols / totalTime;//20 cols
  byte mod = (presentTime * Cols * 5 / totalTime) % 5;
  int normalizedValue = (int)fullChars * 5 + mod;
  if(prevValue != normalizedValue) {
     lcd.setCursor(0,3);
  for(byte i=0; i<fullChars; i++) {
        lcd.write((byte)0);}//least no of blocks in lcd
  if(mod > 0) {
            lcd.write(mod); // -- index the right partial character
            ++fullChars;}//if even 1 mod value is displayed that coloumn cell can't be used .So shifting to next col
  for(byte i=fullChars;i<lastFullChars;i++) {
            lcd.write(' ');//writing remaining col cells to Null
        }
  lastFullChars = fullChars;
  prevValue = normalizedValue;
}
}

void Reset()  //whole reset(not written to eeprom
{
Summary[0]=SummaryManual[0]=100;
Summary[1]=100;
Summary[2]=SummaryManual[1]=1;
Summary[3]=SummaryManual[2]=2;
}//end Reset

void writeIntoEEPROM(int address, long unsigned int numbers[], int arraySize)
{

  int addressIndex = address;
  for (int i = 0; i < arraySize; i++) 
  {
    EEPROM.write(addressIndex, (numbers[i] >> 24) & 0xFF);
    EEPROM.write(addressIndex + 1, (numbers[i] >> 16) & 0xFF);
    EEPROM.write(addressIndex + 2, (numbers[i] >> 8) & 0xFF);
    EEPROM.write(addressIndex + 3, numbers[i] & 0xFF);
    addressIndex += 4;
    lcd.setCursor(0,1);
    lcd.print("Saving..");

  }
}
void readFromEEPROM(int address, int arraySize)
{
  long unsigned int numbers[ARRAY_SIZE]={0,0,0,0};
  int addressIndex = address;
  for (int i = 0; i < arraySize; i++)
  {
    numbers[i] = ((long)EEPROM.read(addressIndex) << 24) +
                 ((long)EEPROM.read(addressIndex + 1) << 16) +
                 ((long)EEPROM.read(addressIndex + 2) << 8) +
                 (long)EEPROM.read(addressIndex + 3);
    addressIndex += 4;
  }
     for(int m=0;m<ARRAY_SIZE;m++)
     {
      Summary[m]= numbers[m];
      if(m==0)//because first one has 4 opts and second has 3     
      {
        SummaryManual[m]=numbers[m];
      }
      if(m==2 or m==3)
      {
        SummaryManual[m-1]=numbers[m];
      }
      
     }
  lcd.setCursor(0,1);
    lcd.print("RETRIVING FROM MEM");
    delay(1000);
    lcd.clear();
}
void display_Emenu(const char *menuInput, int ROWS, int COLS) //function to display EditMenu
{
  lcd.clear();
  if (ROWS < n - 1) {
    n = ROWS + 1;
  }
  for (int i = 0; i < n; i++) {
    lcd.setCursor(1, i); //(col, row)
    for (int j = 0; j < COLS; j++) {
      if (*(menuInput + ((displayFirstLine + i - 1) * COLS + j)) != '\0') {
        //if displayfirstline=1,Prints first 4 options,else iff display firstline=2,prints from 2 to 5 options.\\ly.
        lcd.print(*(menuInput + ((displayFirstLine + i - 1) * COLS + j)));
        Serial.println(*(menuInput + ((displayFirstLine + i - 1) * COLS + j)));
      }//end if
    }//end for j
if((displayFirstLine + i)<=4){
    lcd.setCursor(12, i);
    lcd.print(Summary[displayFirstLine + i - 1]);}
  }
  lcd.setCursor(0, (cursorLine - displayFirstLine));//prints < arrow at which line 
  lcd.print("<");
//end display_Emenu
}

void display_Auto(int option,const char *menuInput, int ROWS, int COLS)  //function to display AutoMenu
{
    lcd.setCursor(4,0); //(col, row);
    lcd.print("AUTO MODE");
    lcd.setCursor(0,1); //(col, row);
    for (int i = 0; i < n; i++) {
    for (int j = 0; j < COLS; j++) {
      if (*(menuInput + (i * COLS + j)) != '\0') {
        lcd.print(*(menuInput + (i* COLS + j)));
      }//end for i
    }//end for j
 }
}
void display_AutoVal()
{
 lcd.setCursor(0,3);
 lcd.print("                    ");
 int j=0;
 for(int i=0;i<n;i++)
 {
  switch(i)
  {
    case 0:
      j=0;//insert lcd.setcursor(j,2) &lcd.rpint("spaces")
      lcd.setCursor(j,2);
      lcd.print(Summary[i]);
      break;
    case 1:
      j=6;
      lcd.setCursor(j,2);
      lcd.print(Summary[i]);
      break;
    case 2:
      j=12;
      lcd.setCursor(j,2);
      lcd.print(Summary[i]);
      break;
    case 3:
      lcd.setCursor((Cols-getDigits(Summary[3]-SummaryManual[2])),2);
      lcd.print(Summary[3]-SummaryManual[2]);
      break;
  }
  if(kpd.getKey()=='D')
  {menuOption=2;
    break;}
}
}
void display_Manual(int option,const char *menuInput, int ROWS, int COLS)  //function to display ManualMenu.Till second row.
{
  lcd.setCursor(4,0); //(col, row);
  lcd.print("MANUAL MODE");
  lcd.setCursor(0,1); //(col, row);
   for (int i = 0; i < n; i++) {
    for (int j = 0; j < COLS; j++) {
      if (*(menuInput + (i* COLS + j)) != '\0') {//i*COLS=word no+max of each word.+j=char number
        lcd.print(*(menuInput + (i* COLS + j)));
      }//end for i
    }//end for j
   }
}
void display_ManualVal()
{
 lcd.setCursor(0,2);
 lcd.print("                    ");
 lcd.setCursor(0,3);
 lcd.print("                    ");
 int j=0;
 unsigned long int presentQty;
 presentQty=Summary[3]-SummaryManual[2];
 if ((sensorValue==false) or (keypressed== '*')){
 if(qtyCounter<=Summary[3]){
 for(int i=0;i<n;i++)
 {
  lcd.setCursor(j,2);
  if(i==2){
    SummaryManual[2]=SummaryManual[2]-1;
    lcd.print(presentQty);}
  else
    lcd.print(SummaryManual[i]);
  j+=6;
  if(kpd.getKey()=='D')
    {menuOption=1;
     break;}
 }
}
else
{
  SummaryManual[2]=Summary[3];
  menuOption=2;
}
}
else{
 for(int i=0;i<n;i++)
 {
  lcd.setCursor(j,2);
  if (i==2)
   lcd.print(presentQty);
  else
   lcd.print(SummaryManual[i]);
  j+=6;
  if(kpd.getKey()=='D')
   {menuOption=1;
    break;}}
}
}

int graphON(int option,unsigned long int ontime)//Here it prints just value and turns output on
{analogWrite(ledPin,Summary[2]);
unsigned long int startMillis;
unsigned long int currentMillis;
startMillis=currentMillis=millis();

        for(;(currentMillis-startMillis)<=ontime;)
        { lcd.setCursor(0,3);
          printGraphChar(currentMillis-startMillis,ontime);//prints a single character at a time
          currentMillis=millis();
          delay(5);
        if(kpd.getKey()=='D')
        { if (option==0)
            menuOption=1;
          if (option==1)
            menuOption=2;
          break;}
        }    
analogWrite(ledPin,0);
return;
}

int graphOFF(int option,unsigned long int offtime)//Here it prints just value and turns output off
{
analogWrite(ledPin,0);
unsigned long int startMillis;
unsigned long int currentMillis;
startMillis=currentMillis=millis();

  if(option ==1)//allows only for auto timer
  {
        for(;(currentMillis-startMillis)<=offtime;)
        {
         lcd.setCursor(0,3);
         printGraphChar(offtime-currentMillis+startMillis,offtime);//prints a single character at a time
         currentMillis=millis();
         delay(5);
        if(kpd.getKey()=='D')
        {menuOption=2;
        break;}
        }
  }
return;
}

void setup() {
  // put your setup code here, to run once:
  menuItems = sizeof manualMenu / sizeof * manualMenu;
  menuSelected = &manualMenu[0][0]; //Start Menu
  menuOption = ManualMenu; //Main Menu
  
 lcd.begin(Cols,Rows);
 unsigned long int Summary[] = {0, 0, 0, 0};   //storage for Edit and Auto
 unsigned long int SummaryManual[]={0,0,0};  //storage for Manual
 lcd.createChar(1,row_1);
  lcd.createChar(2,row_2);
  lcd.createChar(3,row_3);
  lcd.createChar(4,row_4);
  lcd.createChar(0,row_5);
Serial.begin(9600);
const int ARRAY_SIZE = 4;
const int STARTING_EEPROM_ADDRESS = 17;
pinMode(ledPin,OUTPUT);
pinMode(sensorPin,INPUT_PULLUP);
 readFromEEPROM(STARTING_EEPROM_ADDRESS,ARRAY_SIZE);
 n=3;
 //display_Manual(menuOption,menuSelected, menuItems, maxItemSize);
}

void loop() {
while(menuOption==EditMenu)
  {lcd.clear();
    display_Emenu(menuSelected, menuItems, maxItemSize);
    keypressed=kpd.waitForKey();
    
    switch(keypressed)
    {
      case 'A':
       move_up();
       display_Emenu(menuSelected, menuItems, maxItemSize);
       break;
      case 'B':
        move_down();
        display_Emenu(menuSelected, menuItems, maxItemSize);
        break;
      case 'C':
        selectionSubMenu();
        display_Emenu(menuSelected, menuItems, maxItemSize);
        break;
      case 'D':
        menuOption=0;
        break;
    }
  }
keypressed=NULL;
cursorLine = 1;
displayFirstLine = 1;
selectionMainMenu();

while(menuOption==ManualMenu)
  {  
    display_ManualVal();
    if(sensorValue== true){
      lcd.setCursor(0,3);
      lcd.print("PRESS PEDAL or '*'");
      while(keypressed!='D'){
      sensorValue=digitalRead(sensorPin);
      keypressed=kpd.getKey();
      if ((sensorValue==false) or (keypressed== '*'))
          break;
      }
      if(kpd.getKey()=='D')
      {menuOption=1;
       break;}}
      if ((sensorValue==false) or (keypressed== '*'))
             {
              display_ManualVal();
              if (menuOption==0)
                graphON(menuOption,Summary[0]);
              qtyCounter++;
              }
     else
        menuOption=1;
   if(kpd.getKey()=='D')
   {menuOption=1;
    break;}
 sensorValue=true;
 keypressed=NULL;
}
qtyCounter=1;
SummaryManual[2]=Summary[3];
sensorValue=true;
keypressed=NULL;
cursorLine = 1;
displayFirstLine = 1;
analogWrite(ledPin,0);
selectionMainMenu();
 
while(menuOption==AutoMenu){
 while((kpd.getKey()!='D')and (menuOption==1)){
    lcd.setCursor(0,2);
    lcd.print("                    ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    for(unsigned long int loopVar=1;loopVar<=Summary[3];loopVar++){
    if(menuOption==1){
    display_AutoVal();
    graphON(menuOption,Summary[0]);
       if(kpd.getKey()=='D')
        {menuOption=2;
        break;}
    SummaryManual[2]=SummaryManual[2]-1;    
    display_AutoVal();
    graphOFF(menuOption,Summary[1]);
       if(kpd.getKey()=='D')
        {menuOption=2;
        break;}
    }}
 SummaryManual[2]=Summary[3];
 lcd.clear();
 delay(100);
 break;}
 analogWrite(ledPin,0);
 menuOption=2;
 // break;
 }
qtyCounter=1;
SummaryManual[2]=Summary[3];
sensorValue=true;
keypressed=NULL;
cursorLine = 1;
displayFirstLine = 1;
analogWrite(ledPin,0);
selectionMainMenu();
}
