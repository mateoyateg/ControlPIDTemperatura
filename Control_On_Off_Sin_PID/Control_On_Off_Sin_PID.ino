//Control On-Off de temperatura dado un "setPoint" arbitrario
#define PIN_LM35 1     //Pin del sensor LM35 analógico
#define PIN_OUT 3      //Pin de salida PWM digital

double input, output;                     //Linea auxiliar para la grafica
double setPoint;                          //Setpoint de la temperatura deseada
double estadoPinOut;                      //Linea auxiliar para la grafica
unsigned long tiempoActualGraficar;       //Constantes para guardar el tiempo actual
unsigned long tiempoPrevioGraficar = 0;   //Constantes para guardar el tiempo previo a un ciclo del loop
double intervalo = 1000;                  //Graficar cada 1s

void setup() {
  output = 180;
  analogReference(INTERNAL);    //Referencia de Arduino a 1 voltio, así el sensor LM35 da una mejor precision
  Serial.begin(9600);           //Transmitir por la banda de 9600 baudios
  analogWrite(PIN_OUT, output); //
  setPoint = 40;                //Setear el setPoint en la temperatura deseada
}

void loop() { 
  tiempoActualGraficar = millis();          //Capturar el tiempo actual en ms para graficar
  int value = 0;                          //Variable para el calculo de sensor LM35
  for (int i = 0; i < 30; i++){
    value = value + analogRead(PIN_LM35); //Tomar 30 muestras para una mayor precisión y sumarlas
  }
  value = value/30;                       //Promedio del valor de las 30 iteraciones anteriores
  float mv  = (1100 / 1024.0) * value;    //Valor leido a mV (el "tope" es de 1100mV)
  float cel = mv / 10;                    //Pasar de mV a grados celcius (Cº)

  input = cel;                            //Almacenar los grados Cº en la variable input
    
  if (input < (setPoint+2)){              //Si la temperatura es menor al setPoint menos 0.5, encender la salida
    output = output + 10;                 //Aumentar la salida al MOSFET
    output = constrain(output,0,180);     //Restringir la salida en el rango permitido y optimo para evitar sobresaltos
    analogWrite(PIN_OUT, output);         //Escribir el voltaje en el pin de salida
    estadoPinOut = 15;                    //Graficar una línea en 15 indicando que está encendida la salida
    
  } else if (input > (setPoint-2)) {     //Si en cambio, la temperatura es mayor al setPoint mas 0.5, apagar la salida
    output = output - 10;
    output = constrain(output,0,180);
    analogWrite(PIN_OUT, output);            //Le damos todo el voltaje al PIN de salida
    estadoPinOut=10;                        //Graficar en 35 indicando que está apagada la salida
  } else {
    //Casualmente, la temperatura esta en el setpoint
    //Do nothing
  }

  if (tiempoActualGraficar - tiempoPrevioGraficar >= intervalo){    //Graficar
    tiempoPrevioGraficar = tiempoActualGraficar;    //Guardar el tActual como previo para graficar
    Serial.print((setPoint + 1));    //Pintar una línea de referencia a +1 Cº del setPoint
    Serial.print(",");
    Serial.print(setPoint);         //Pintar la linea de setPoint para dar una referencia
    Serial.print(",");
    Serial.print((setPoint - 1));   //Pintar una línea de referencia a -1 Cº del setPoint
    Serial.print(","); 
    Serial.print(output/10);
    Serial.print(",");
    Serial.println(input);          //Pintar la línea de lectura del sensor LM35
  }
  
}
