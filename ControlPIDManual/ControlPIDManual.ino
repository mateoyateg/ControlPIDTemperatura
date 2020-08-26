
//Control de temperatura con PID sin libreria
#define PIN_LM35 1      //Pin del sensor LM35 analógico
#define PIN_OUT 3       //Pin de salida PWM digital

//Constantes de PID
double kp = 4.0000;        //Constante de control Proporcional 
double ki = 10.000;       //Constante de control Integral
double kd = 0.0100;       //Constante de control Derivativo

unsigned long tiempoActual, tiempoPrevio;       //Constantes para guardar el tiempo actual y el previo a un ciclo del loop
double deltaTiempo;                             //Constante para el tiempo transcurrido entre ejecuciones
double error;                                   //Constante para guardar el valor del error
double input, output;                           //Constantes para guardar el ingreso de sensor y la salida hacia el PIN_OUT
double inputAnterior = 0;                       //Constante para guardar el error anterior
double setPoint;                                //Constante para almacenar el "setPoint" o valor deseado
double intError, derError, derInput;            //Constantes para el calculo de la integral y derivada del 
unsigned long tiempoActualGraficar;             //Constantes para guardar el tiempo actual
unsigned long tiempoPrevioGraficar = 0;         //Constantes para guardar el tiempo previo a un ciclo del loop
double intervalo = 1000;                        //Graficar cada 1s
double tiempoMuestreo = 1000;                   //Calcular el PID cada 1000ms (1s)

void setup(){
  analogReference(INTERNAL);              //Referencia de Arduino a 1 voltio, así el sensor LM35 da una mejor precision
  Serial.begin(9600);                     //Transmitir por la banda de 9600 baudios
  setPoint = 40;                          //Setear el setPoint en la temperatura deseada
}

void loop(){
  tiempoActualGraficar = millis();        //Capturar el tiempo actual en ms para graficar
  int value = 0;                          //Variable para el calculo de sensor LM35
  for (int i = 0; i < 30; i++){
    value = value + analogRead(PIN_LM35); //Tomar 30 muestras para una mayor precisión y sumarlas
  }
  value = value/30;                       //Promedio del valor de las 30 iteraciones anteriores
  float mv  = (1100 / 1024.0) * value;    //Valor leido a mV (el "tope" es de 1100mV)
  float cel = mv / 10;                    //Pasar de mV a grados celcius (Cº)

  input = cel;                            //Almacenar los grados Cº en la variable input
  output = calcularPID(input, output);            //Llamar a la función que calcula el valor del controlador PID
  
  analogWrite(PIN_OUT, output);           //Escribir el valor del PID como PWM en el pin de salida

  if (tiempoActualGraficar - tiempoPrevioGraficar >= intervalo){  
    tiempoPrevioGraficar = tiempoActualGraficar;    //Guardar el tActual como previo para graficar
    Serial.print((setPoint + 1));                   //Pintar una línea de referencia a +1 Cº del setPoint
    Serial.print(",");   
    Serial.print(setPoint);                         //Pintar la linea de setPoint para dar una referencia
    Serial.print(","); 
    Serial.print((setPoint - 1));                   //Pintar una línea de referencia a -1 Cº del setPoint
    Serial.print(",");  
    Serial.print(output/10);                        //Pintar el valor del controlador PID en PWM/10 (Para una mejor grafica)
    Serial.print(",");
    Serial.println(input);                          //Pintar la línea de lectura del sensor LM35
  }
  
}

double calcularPID(double inp, double outAct){       //Metodo para calcular el valor del controlador PID
  tiempoActual = millis();                                //Capturar el tiempo actual en ms
  deltaTiempo = (double)(tiempoActual - tiempoPrevio);    //Calcular el tiempo transcurrido desde la ultima lectura

  if (deltaTiempo >= tiempoMuestreo){                     //Solo calcular el valor de salida con PID si el tiempo transcurrido es igual o mayor al muestreo definido.
    error = setPoint - inp;                               //Calcular el valor del error e(t)
    derInput = (inp - inputAnterior);         //Dado que la derivada del setpoint (que es constante) es cero, se calcula la derivada de la entrada. (1)
  
    intError = intError + ki*(error);         //Calcular la integral del error con la regla del Rectangulo [int(f(a)) = (b-a)*f(a)] (2) y multiplicar por ki (3)
    intError = constrain(intError,-255,255);              //Limitar el calculo de la integral (Que no crezca más de 5v o 255 ciclos de trabajo PWM)
  
    double out = kp*error + intError - kd*derInput;       //Calcular la salida del controlador PID teniendo en cuenta (3) y el signo de (2)
  
    tiempoPrevio = tiempoActual;                          //Guardar el tiempo actual para una futura iteración
    inputAnterior = inp;                                  //Guardar la entrada actual para una futura iteracion
    out = constrain(out, 0, 255);                         //Limitar el valor del PID a los rangos admisibles del PWM
    return out;                                           //Retornar el valor resultante del controlador PID
    
  } else {                                                //En otro caso, retornar el valor actual de la salida
    
    return outAct;                                        //No cambiar el valor de la salida a PWM actual
    
  }
  
  
  
  
}
/* (1) Prueba de que se puede emplear la derivada de la entrada
 * Dado que e(t) = setPoint - input, por linealidad, se tiene que:
 * de(t)/dt = d(setPoint)/dt - d(input)/dt
 * Y dado que el setPoint es una cosntante, luego d(setPoint)/dt = 0, así
 * de(t)/dt = -d(input)/dt
 * 
 * (2) Regla del rectangulo para el caluclo de la integral
 * Si se tiene la integral int(f(a)) desde a hasta b,
 * así, la regla del rectangulo dice:
 * int(f(a)) a -> b = (b-a)*f(a)
 * 
 * (3) En el calculo de la integral del error, es posible multiplicar por ki 
 * de manera inmediata. Por la multiplicacion de una cosntante
 * ki * int (f(t)) = int(ki * f(t))
 * lo cual reduce errores en el calculo porque el termino ki 
 * solo se emplea en el calculo actual de la integral
 */
