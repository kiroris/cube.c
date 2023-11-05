#include <math.h> // math либу, для тригонометрических функций
#include <stdio.h> //стандарт либа input/output
#include <string.h> //for work with строки
#include <unistd.h> //для задержки(uslee),как time in python

//переменные для управления углами в 3-х мерном пространстве
float A, B, C;

float cubeWidth = 20; //размер стороны куба
int width = 160, height = 44; //ширина и высота всего экрана, где отображается куб
float zBuffer[160 * 44]; //массив для отслеживания глубины каждой точки куба, чтобы определить, какие точки должны быть на переднем плане
char buffer[160 * 44]; //массив для отображения символов на экране, представляющих точки куба
int backgroundASCIICode = '.'; //символ фона
int distanceFromCam = 100; //я думаю и так поннятно, расстояние от куба до камеры
float horizontalOffset; //переменная для управления горизонтальным смещением куба на экране
float K1 = 40; // переменная(для вычислений) для проекции точек куба на экран

float incrementSpeed = 0.6; //шаг инкремента

float x, y, z; //переменные для хранения координат точек куба в трехмерном пространстве
float ooz; //переменная для хранения обратной z-координаты
int xp, yp; //две переменныедля хранения координат точек на двумерном экране
int idx; //переменная используется для хранения индекса буфера символов, что позволяет правильно отображать точки куба на экране


//эти три функции используются для вычисления координат точек куба в трехмерном пространстве на основе входных параметров i, j и k.

//конкретно эта функция рассчитывает x-координату точки куба
float calculateX(int i, int j, int k) {
  return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) +      // i, j, k - координаты точек куба
         j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);   // A, B, C - углы поворота, нужны для преобразования координат
}

//эта функция вычисляет y-координату точки куба
float calculateY(int i, int j, int k) {
  return j * cos(A) * cos(C) + k * sin(A) * cos(C) -
         j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C) -
         i * cos(B) * sin(C);
}

//эта функция вычисляет z-координату точки куба
float calculateZ(int i, int j, int k) {
  return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch) {   //float cubeX, float cubeY, float cubeZ - эти параметры представляют координаты куба в 3-x мерном пространстве
  x = calculateX(cubeX, cubeY, cubeZ);  //вывзываются данные функции, которые мы описали ранее
  y = calculateY(cubeX, cubeY, cubeZ);  //чтобы вычислить 3-х мерные координаты в X и Y для точки куба
  z = calculateZ(cubeX, cubeY, cubeZ) + distanceFromCam; //вычисляеться Z точка для куба и добавляется distanceFromCam, чтобы увеличть расстояние от куба до камеры 

  ooz = 1 / z; //строка для вычисления обратной z-координаты, которая потом нужна для вычислений

  xp = (int)(width / 2 + horizontalOffset + K1 * ooz * x * 2); //вычисляются двумерные координаты
  yp = (int)(height / 2 + K1 * ooz * y);                       // XP and YP

  idx = xp + yp * width; //строка для вычисления индекса в буфере для соответствующего пикселя на экране
  if (idx >= 0 && idx < width * height) { //проверяет, что индекс находится в пределах размеров экрана
    if (ooz > zBuffer[idx]) {  //проверяет, что текущее значение ooz больше, чем значение в буфере глубины , что позволяет определить, что точка на переднем плане и должна быть отображена
      zBuffer[idx] = ooz; //обновление буфера глубины и буфера символов 
      buffer[idx] = ch;   //для каждого соответствующего пикселя
    }
  }
}

int main() {
  printf("\x1b[2J"); //строка используется для очистки экрана перед началом отрисовки куба
  while (1) {        //есконечный цикл, позволяет анимации продолжаться бесконечно
    memset(buffer, backgroundASCIICode, width * height); //строки используются для очистки буфера символов и 
    memset(zBuffer, 0, width * height * 4);              //буфера глубины перед каждым новым кадром
    cubeWidth = 20;
    horizontalOffset = 0; // Set horizontalOffset to 0 for centering

    for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {                 //ЭТА 
      for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed) {               //ФУНКЦИЯ 
        calculateForSurface(cubeX, cubeY, -cubeWidth, '@');                                      //ПРОСТО 
        calculateForSurface(cubeWidth, cubeY, cubeX, '$');                                       //КАКОЙ-ТО
        calculateForSurface(-cubeWidth, cubeY, -cubeX, '~');                                     //ПИЗДЕЦ
        calculateForSurface(-cubeX, cubeY, cubeWidth, '#');                                      //вложенный цикл используется для прохода по каждой точке куба в 3-x мерном пространстве и 
        calculateForSurface(cubeX, -cubeWidth, -cubeY, ';');                                     //вызова функции calculateForSurface, чтобы отобразить точки куба на экране
        calculateForSurface(cubeX, cubeWidth, cubeY, '+');                                       //я вот именно это, украл, признаюсь, просто ужас
      }
    }

    printf("\x1b[H"); //перемещает курсор в начало экрана перед отображением нового кадра
    for (int k = 0; k < width * height; k++) { //цикл для отображения буфера символов на экране
      putchar(k % width ? buffer[k] : 10);
    }

    A += 0.05; //изменения углов поворота
    B += 0.05; //создаёт впечатлении анимации
    C += 0.01; 
    usleep(8000 * 2); //задержка кадров анимации
  }
  return 0;
}
