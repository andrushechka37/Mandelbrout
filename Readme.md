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
## *Цель работы* : исследовать зависимость производительности от оптимизации
## *В работе используется* : `MAC` на процессоре `M2`*
#### *для доступа к серверу
# `2 Теоретические сведения`: 
## читайте бх 3,4,5 главу
# `3 Экспериментальная установка`:
## Удаленный сервер
![1](/images/1.png)
# `4 Ход работы`:

<br/>
<br/>
<br/>

# Первый опыт: manda1.cpp
<br/>
<br/>

## Простейший вариант программы, использующий:
* inline функции
* вынос кода

# Второй опыт: manda2.cpp
## Слегла усовершенствованный вариант manda1.cpp
* развертывание цикла 4x4
  
# Третий опыт: manda3.cpp
## Это manda2.cpp но оптимизировано с помощю SIMD
* bcgj
