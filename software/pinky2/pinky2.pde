

boolean running = false;
int characterIndex = 0;
int bitIndex = 0;

color dataColor;

boolean clock = false;

String string = "PINKY";

char[] message;
char character;

int step = 0;

final int EDITING = 0;
final int UPLOADING = 1;
int mode = EDITING;

/*
boolean sketchFullScreen() {
  return true;
}
*/

void setup () {

  size(640, 480);
   //size(displayWidth, displayHeight, P3D);
  frameRate(30);

  println(setTextToWidth(36,120));
}

void draw() {
  background(0);

  if ( mode == EDITING ) {
    textAlign(CENTER, CENTER);
    text(string, width*0.5, height*0.5);
  } 
  else if ( mode == UPLOADING ) {

   

    if ( running ) {

      noStroke();

      switch ( step ) {
      case 0: // RAISE CLOCK
        clock = true;
        character = message[characterIndex];
        if ( bitIndex == 0 ) {
          println();
          println("Character: "+character);
        }
        print("^");
        break;
      case 1: // NEXT DATA, KEEP CLOCK HIGH
        clock = true;
        int bit = (character >> bitIndex ) & 1;
        print(bit);
        dataColor = bit * 255;

        bitIndex = bitIndex + 1;
        if ( bitIndex == 8 ) {
          bitIndex = 0;
          characterIndex = characterIndex + 1;
        }
        break;
      case 2: // LOWER CLOCK
        clock = false;
        print("v");

        break;
      case 3: // KEEP CLOCK LOW
        clock = false;
        if ( characterIndex == message.length) {
          running = false;
        }
        break;
      }  
      step = (step + 1) % 4;

     noStroke();


      if ( clock ) {
        fill(255);
       rect(0, 0, width*0.5, height*0.95);
      }

      fill(dataColor);
     rect(width*0.5, 0, width, height*0.95);
      
      fill(127);
     
      rect(0, height*0.95, map(characterIndex*8+bitIndex, 0, message.length*8,0,width), height*0.05);
      
     
      
    }
    
      stroke(127);
      line(width*0.5, 0, width*0.5, height*0.95);
  }
}

void armUploading() {
  
  if ( string.length() > 0 ) {
    mode = UPLOADING;
    frameRate(10);
  }
}

void startUploading() {
  
  if ( !running ) {
    running = true;
    bitIndex = 0;
    characterIndex =0 ;
    step = 0;

    message = new char[string.length()+1];

    for ( int i = 0; i < string.length() ; i++ ) {
      message[i] = string.charAt(i);
    }
    message[string.length()] = 0;
  }
}

void mousePressed() {
  
   if ( mode == EDITING ) {
     armUploading();
   } else if ( mode == UPLOADING ) {
     startUploading();
  
   }
}

void keyPressed() {

  if ( mode == EDITING ) {
    
    if ( key == BACKSPACE ) {
      if ( string.length() > 0 ) string = string.substring(0, string.length()-1);
    } 
    else if ( (key > 31 && key < 128) && string.length() < 25 ) {
      if ( key > 96 && key < 123 ) key = (char)(key - 32);
      string = string+key;
    } 
    else if ( key == RETURN || key == ENTER ) {
      armUploading();
    }
    
  } 
  else if ( mode == UPLOADING ) {
   
      startUploading();
    
  }
  
  println(setTextToWidth(36,120));
}


int setTextToWidth(int startSize, int maxSize) {
  
  if ( string.length() > 0 ) {
     textSize(startSize);
    while( (textWidth(string) < width*0.95) && ( startSize < maxSize )) {
      startSize = startSize + 1;
      textSize(startSize);
    }
    
     
      
  } 
  
  return  startSize;
  
}
