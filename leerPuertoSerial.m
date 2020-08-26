%Programa para recolectar los datos del sensor LM35 de Arduino
clc;
clear;
delete(instrfind({'Port'},{'COM3'}));   %Borrar todo lo anterior del puerto COM3.

try
    delete(s);                          %Limpiar el objeto de tipo Serial en caso de que este exista
catch ME
    %No hacer nada, dado que no fue necesario borrar el objeto serial.
end

numeroMuestras = 200;                           %Definir el numero de muestras a tomar
disp("Numero de muestras: " + numeroMuestras);  %Mostrar el numero de muestrar a tomar

s = serialport("COM3",9600);                                %Crear un objeto serialPort para escuchar al Arduino
warning('off','MATLAB:serial:fscanf:unsuccessfulRead');     %Mostrar los errores en caso de que ocurran

rate = 33;              %Velocidad de capturas por segundo estimado

f = figure('Name','Captura');                 %Crear un objeto de tipo figura
a = axes('XLim',[0 10],'YLim',[0 60]);          %Definir los limites de ambos ejes (10 segundos y 60ºC)
l1 = line(nan,nan,'Color','b','LineWidth',2);   %Linea del setpoint + 1
l2 = line(nan,nan,'Color','c','LineWidth',2);   %Linea del setpoint
l3 = line(nan,nan,'Color','g','LineWidth',2);   %Linea del setpoint - 1
l4 = line(nan,nan,'Color','y','LineWidth',2);   %Linea del output
l5 = line(nan,nan,'Color','r','LineWidth',2);   %Linea del sensor LM35DZ

xlabel('Tiempo (s)')                    %Etiqueta del eje x                   
ylabel('Grados centrigrados (Cº)')      %Etiqueta del eje y
title('Captura de Temperatura en tiempo real con Arduino')  %Titulo de trafigo
grid on      %Graficar
hold on      %Mantener la grafica abierta

c1 = zeros(1,numeroMuestras);    %Vector para la linea de setpoint + 1
c2 = zeros(1,numeroMuestras);    %Vector para la linea de setpoint
c3 = zeros(1,numeroMuestras);    %Vector para la linea de setpoint - 1
c4 = zeros(1,numeroMuestras);    %Vector para la linea del output
c5 = zeros(1,numeroMuestras);    %Vector para la linea del sensor LM35DZ

contadorMuestras = 0;       %Contador de las muestras tomadas
i = 1;                      %Contador auxiliar

%Datos para el ident
datosIdent = [];            %Datos guardados leidos del sensor
tiempoPromedio = [];        %Tiempo entre cada iteracion
tiempoIdent = [];           %Arreglo con el escalon unitario

tiempoAnterior = 0;         %Tiempo de la iteracion anterior
tiempoActual = 0;           %Tiempo de la iteracion actual

tic                         %Iniciar un cronometro

%Iterar mientras se llegue al numero de muestras pedido
while contadorMuestras<numeroMuestras
    t = toc;                                                %Marcar el tiempo actual
    tiempoActual = t;                                       %Guardar el tiempo actual
    tiempoPromedio(i) = tiempoActual - tiempoAnterior;      %Calcular la diferencia entre las iteraciones
    a = readline(s);                                        %Leer del puerto serial
    datosRecibidos = split(a,",");                          %Separar los datos recibidos por comas
    disp("Datos recibidos: " + datosRecibidos);                     %Mostrar los datos recibidos
    
    c1(i)=datosRecibidos(1);        %Guardar los datos del setpoint + 1
    c2(i)=datosRecibidos(2);        %Guardar los datos del setpoint
    c3(i)=datosRecibidos(3);        %Guardar los datos del setpoint - 1
    c4(i)=datosRecibidos(4);        %Guardar los datos del output
    c5(i)=datosRecibidos(5);        %Guardar los datos del sensor LM35
    
    dataIdent(i) = str2double(datosRecibidos(5));           %Añadir los datos leidos del sensor a un arreglo
    
    %Dibujar en la figura
    x = linspace(0,i/rate,i);
    set(l1,'YData',c1(1:i),'XData',x);
    set(l2,'YData',c2(1:i),'XData',x);
    set(l3,'YData',c3(1:i),'XData',x);
    set(l4,'YData',c4(1:i),'XData',x);
    set(l5,'YData',c5(1:i),'XData',x);
    drawnow
    
    %Avanzar a la siguiente iteracion
    contadorMuestras = contadorMuestras+1;
    i = i + 1;
    tiempoAnterior = t;
    disp("Numero de muestra: " + contadorMuestras);
end
% resultado del cronometro
fprintf('%g s de captura a %g cap/s \n',t,i/t);

%Construir un objeto que represente al escalon unitario
[m,n] = size(dataIdent);
for k=1:n
    tiempoIdent(k) = 1;
end

%Transponer todos los arreglos
dataIdentFinal = transpose(dataIdent);
tiempoIdentFinal = transpose(tiempoIdent);
tiempoPromedio = transpose(tiempoPromedio);

%Calcular un tiempo promedio entre las iteraciones
ts = mean(tiempoPromedio);

%Crear el objeto iddata
data = iddata(dataIdentFinal,tiempoIdentFinal,ts);

% Limpiar la escena del crimen
delete(s);
clear s;






