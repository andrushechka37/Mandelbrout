## МОСКОВСКИЙ ФИЗИКО-ТЕХНИЧЕСКИЙ ИНСТИТУТ (НАЦИОНАЛЬНЫЙ ИССЛЕДОВАТЕЛЬСКИЙ УНИВЕРСИТЕТ)

## Физтех-школа Радиотехники и Компьютерных технологий
<br/>


# ![1](/images/2.png) 

##  Отчёт о выполнении лабораторной работы 2.2.1
#  SIMD-оптимизация расчета множества Мандельброта
<br/>
<br/>
<br/>
<br/>
<br/>
<br/>

## Автор: Житников Андрей Павлович, Б01-303
<br/>
<br/>
<br/>
<br/>
<br/>
<br/>
<br/>
<br/>
<br/>

## Долгопрудный
## 23 марта, 2024
<br/>
<br/>
<br/>
<br/>
<br/>

# `1 Введение`: 

## Для нагрузки процессора был использован алгоритм  расчета множества Мандельброта, а для наглядности при помощи sfml расчет был визуализирован.

![3](/images/3.png)

# *Цель работы*: 
* Исследовать прирост эффективности от использования Intrinsic функций, сравнить результаты с наивной версией программы и с ее векторизованной версией с использованием развертывания цикла. 
* Сравнить производительность с -O3 и без него



# *В работе используется*: 
* Сервер с конфигурацией:

```
H/W path        Device     Class          Description
=====================================================
                           system         Standard PC (i440FX + PIIX, 1996)
/0                         bus            Motherboard
/0/0                       memory         96KiB BIOS
/0/400                     processor      QEMU Virtual CPU version 4.2.0
/0/401                     processor      QEMU Virtual CPU version 4.2.0
/0/1000                    memory         4GiB System Memory
/0/1000/0                  memory         4GiB DIMM RAM
/0/100                     bridge         440FX - 82441FX PMC [Natoma]
/0/100/1                   bridge         82371SB PIIX3 ISA [Natoma/Triton II]
/0/100/1/0                 system         PnP device PNP0b00
/0/100/1/1                 input          PnP device PNP0303
/0/100/1/2                 input          PnP device PNP0f13
/0/100/1/3                 storage        PnP device PNP0700
/0/100/1/4                 communication  PnP device PNP0501
/0/100/1.1                 storage        82371SB PIIX3 IDE [Natoma/Triton II]
/0/100/1.2                 bus            82371SB PIIX3 USB [Natoma/Triton II]
/0/100/1.2/1    usb1       bus            UHCI Host Controller
/0/100/1.2/1/1  input5     input          QEMU QEMU USB Tablet
/0/100/1.3                 bridge         82371AB/EB/MB PIIX4 ACPI
/0/100/2        /dev/fb0   display        bochs-drmdrmfb
/0/100/3                   network        Virtio network device
/0/100/3/0      eth0       network        Ethernet interface
/0/100/4                   communication  Virtio console
/0/100/4/0                 generic        Virtual I/O device
/0/100/5                   generic        Virtio memory balloon
/0/100/5/0                 generic        Virtual I/O device
/0/100/6                   storage        Virtio block device
/0/100/6/0      /dev/vda   disk           53GB Virtual I/O device
/0/100/6/0/1    /dev/vda1  volume         49GiB EXT4 volume
/0/100/7                   storage        Virtio block device
/0/100/7/0      /dev/vdb   volume         15EiB Windows FAT volume
/1              input0     input          Power Button
/2              input1     input          AT Translated Set 2 keyboard
/3              input3     input          VirtualPS/2 VMware VMMouse
/4              input4     input          VirtualPS/2 VMware VMMouse
```
* Компиллятор:
```
Ubuntu clang version 14.0.0-1ubuntu1.1
Target: x86_64-pc-linux-gnu
Thread model: posix
InstalledDir: /usr/bin
```
# `2 Экспериментальная установка`:
## Удаленный сервер
![1](/images/1.png)
# `3 Ход работы`:

<br/>
<br/>
<br/>

# mand1.cpp
## Это самый первый и наивный вариант программы, от которого мы будем мерить прирост производительности. Ниже для каждой версии представлена функция `get_color`  для высчитывания  цвета каждого пикселя.

```C
inline int get_color(float x0, float y0) {

    float x  = 0.0f;
    float y  = 0.0f;
    float xx = 0.0f;
    float yy = 0.0f;
    float xy = 0;

    int i = 0;

    while (xx + yy <= RADIUS && i < N_MAX) {
        xx = x*x;
        yy = y*y;
        xy = x*y;
        
        y = 2*xy + y0;
        x = xx - yy + x0;
        i++;
    }

    if (i == N_MAX) {
        return 0;       
    } else {
        return i;
    }
}
```

# mand2.cpp
## Вторая версия, использующая развертывание цикла с коэффициентом 4
```C
inline void get_color(float * X0, float * Y0, volatile int * color) {

    float  X[LEN_OF_VECTOR] = {X0[0], X0[1], X0[2], X0[3]};
    float  Y[LEN_OF_VECTOR] = {Y0[0], Y0[1], Y0[2], Y0[3]};
    float XX[LEN_OF_VECTOR] = {0};
    float YY[LEN_OF_VECTOR] = {0};
    float XY[LEN_OF_VECTOR] = {0};

    for (int n = 0; n < N_MAX; n++) {

        for (int j = 0; j < LEN_OF_VECTOR; j++) {
            XX[j] = X[j] * X[j];
            YY[j] = Y[j] * Y[j];
            XY[j] = Y[j] * X[j];
        }

        int cmp[LEN_OF_VECTOR] = {0};
        for (int j = 0; j < LEN_OF_VECTOR; j++) {
            if (XX[j] + YY[j] <= RADIUS) {
                cmp[j] = 1;
            }
        }

        int mask = 0;
        for (int j = 0; j < LEN_OF_VECTOR; j++) {
            mask |= (cmp[j] << j);
        }

        if (!mask) break;

        for (int j = 0; j < LEN_OF_VECTOR; j++) {
            color[j] += cmp[j];
            X[j] = XX[j] - YY[j] + X0[j];
            Y[j] = XY[j] + XY[j] + Y0[j];
        }
    }
}
```

# mand3.cpp
## Третья версия программы, которая использует intrinsic функции
```C
inline void get_color(__m128 X0, __m128 Y0, volatile  __m128 *  color) {

    __m128 X =  _mm_set_ps1(0); // X = {0, 0, 0, 0}
    __m128 Y =  _mm_set_ps1(0); // Y = {0, 0, 0, 0}

    _mm_storeu_si128((__m128i*)&X, (__m128i)X0); // X = X0
    _mm_storeu_si128((__m128i*)&Y, (__m128i)Y0); // Y = Y0

    for (int n = 0; n < MAX_ITERATIONS; n++) {

        __m128 XX = _mm_mul_ps(X,X); // XX == X*X
        __m128 YY = _mm_mul_ps(Y,Y);
        __m128 XY = _mm_mul_ps(X,Y);
        __m128 R2 = _mm_add_ps(YY,XX);

        __m128 cmp = _mm_cmple_ps(R2, RADIUS);      // compare R2[i] and Radius[i]
                                                    // a <= b   =>  -1 || 0 = cmp[i]

        int mask = _mm_movemask_ps((__m128)cmp);    // 0000|4321 where 1234 are floats from cmp

        if (!mask) break;

        *color =  _mm_sub_epi32(*color, (__m128i)cmp); // color[i] = color[i] - (-1 || 0)

        _mm_storeu_si128((__m128i*)&X, (__m128i)_mm_add_ps(_mm_sub_ps(XX,YY), X0));
        _mm_storeu_si128((__m128i*)&Y, (__m128i)_mm_add_ps(_mm_add_ps(XY,XY), Y0));
    }
}
```

<br/>
<br/>
<br/>

# Способ измерения времени:
## Была использована функция `__rdtsc`, "которая возвращает количество циклов часов процессора с момента последнего сброса" [[1]](https://learn.microsoft.com/ru-ru/cpp/intrinsics/rdtsc?view=msvc-170). Рассчитывается разность этих возвращаемых значений и суммируется для вычисления среднего. Время на прорисовку изображения не учитывается, так как оно выключается при подсчете производительности.


## Была использована volatile переменная, чтобы оптимизатор не выкидывал нужные участки программы


|     version    |    name    | Compilation flags | ticks * $10^6$ | Speed Growth calculated with rdtsc|
|:--------------:|:---------:|:-----------------:|:--------------:|:----------------------------------:|
|      base      | mand1.cpp |         -         |       539      |                  1                 |
|      base      | mand1.cpp |        -O3        |       239      |                2.25                |
| vectorized(4x) | mand2.cpp |         -         |       813      |                0.66                |
| vectorized(4x) | mand2.cpp |        -O3        |       83       |                6.43                |
| SIMD optimized | mand3.cpp |         -         |       359      |                1.50                |
| SIMD optimized | mand3.cpp |        -O3        |       70       |                7.71                |

# `5 Обсуждение результатов и выводы`:

## Оптимизации компиллятора -O3 очень сильно повышают производительность независимо от кода (в 2 - 7 раз). Развертывание цикла 4x лишь замедлило выполнение программы, тк для компиллятора было сложно уловить пути оптимизации, но при компилировании с O3 скорость выполнения значительно возросла (в ~ 6 раз). Intrinsic функции дали значительный прирост производительности (в ~7.5 раз).

# `6 Источники`:
1. [Изображение моего сервера ](https://www.google.com/url?sa=i&url=https%3A%2F%2Ffishki.net%2F4052473-server-na-kartoshke-i-drugie-udachnye-popytki-dobyty-jelektrichestvo-iz-ovowa.html&psig=AOvVaw1-CA8mwoAHKHGixGGslGQn&ust=1711799436932000&source=images&cd=vfe&opi=89978449&ved=0CBIQjRxqFwoTCKjk5PyzmYUDFQAAAAAdAAAAABAE)
