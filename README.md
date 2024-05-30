# RAID6 реализация (RAID6 implemenation)

## Описание ([Eng. version](#description))

### Математическое описание

RAID (Redundant Array of Independent Disks) - это технология, позволяющая объединять несколько физических дисков в один логический модуль для улучшения отказоустойчивости и производительности системы. RAID6 - это шестой уровень RAID, который обеспечивает высокую степень надежности за счет двойной четности.

### RAID6

RAID6 использует чередование данных с двойной распределенной четностью, что позволяет массиву оставаться доступным даже после двух последовательных сбоев. Для функционирования RAID6 необходимо минимум четыре диска, включая два для данных и два для управления четностью.

Распределение блоков данных и блоков избыточности для RAID 6 на 5 дисках в приложении происходит в соответсвии с *рис. ниже*. Таким образом, на представленной схеме: блоки данных - $A1-A3$, $B1-B3$, $C1-C3$, $D1-D3$, $E1-E3$ и блоки избыточности - $A_p$, $A_q$, $B_p$, $B_q$, $C_p$, $C_q$, $D_p$, $D_q$, $E_p$, $E_q$.

![scheme1](https://github.com/StanleyStanMarsh/RAID6/assets/96591356/36271c45-9692-44ca-b7ab-ac48195c6dad)

## Полином для генерации блоков избыточности

Для расчета блока избыточности используется полиномиальное выражение. Данные преобразуются в полином, где каждый блок данных становится подставляемым в полином. Восстановление данных происходит через решение системы полиномиальных уравнений, созданных на основе сохраненных данных и избыточных блоков.

### Особенности реализации

#### Представление дисков

В симуляции диски представлены текстовыми файлами. Один диск - один файл. Файлы хранятся в рабочем каталоге приложения и доступны в процессе работы.

#### Запись и чтение данных

Для записи и чтения данных используются команды write и read.

Для записи информации на диски пользователь использует команду:

`<address> write <data>`,

где *address* - адрес (строка) от 0 до 63, по которому пользователь хочет записать сообщение, *data* - строка-сообщение, которое должно состоять строго из 14 символов и которое должно содержать символы только шестнадцатеричной системы счисления.

Определение расположения блоков данных и избыточности зависит от заданного адреса - на основе деления адреса по модулю от числа дисков. На условной матрице 5x5 будет выбираться номер строки - остаток от деления адреса на число 5 - в которой содержатся номера избыточных дисков. Сообщение разделяется на три части: по 5 байтов в первых двух и 4 байта в последней. Каждая часть затем записывается в отдельный блок данных.

Подсчет блоков избыточности происходит с помощью двух полиномов и записывается в соответствующие ячейки:

$$
A + B + C = X
$$

$$
A + 2B + 3C = Y
$$

Переменные полинома $A$, $B$, $C$ последовательно соответствуют блокам данных. Например, для вычисления избыточности p по адресу 0, значение из блока $A1$ конвертируется из шестнадцатеричной в десятичную систему и помещается в переменную $A$, блок $A2$ записывается в переменную $B$, а блок $A3$ - в переменную $C$.

Для чтения информации с дисков пользователь использует команду:

`<address> read`,

где *address* - адрес (строка) от 0 до 63, из которого пользователь хочет прочитать сообщение. Сначала следует проверить, произошла ли потеря дисков. Если диски на месте, то с указанного адреса происходит чтение частей сообщения из блоков данных, которые затем собираются в единую строку и выводятся пользователю. Если же один или два диска пропали, тогда необходимо анализировать разные варианты: утеряны ли блоки данных или избыточности, согласно адресу, указанному пользователем. В данной реализации потерянные диски не восстанавливаются, а восстанавливаются лишь конкретные данные по запрашиваемому адресу.

Рассматриваются всевозможные комбинации потери блоков, восстанавливаются данные и пользователю выводится строка:

1. Если утеряны два и меньше блока контрольных сумм и не утеряны блоки данных, то достаточно просто объединить части сообщения из блоков данных в строку и вывести пользователю. Блоки контрольных сумм подсчитываются с помощью полиномов и сохраненных данных и записываются на заново созданные диски.
   - Если утерян блок данных A, то решается уравнение вида: $$A = X - B - C$$
   - Если утерян блок данных B, то решается уравнение вида: $$B = X - A - C$$
   - Если утерян блок данных C, то решается уравнение вида: $$C = X - A - B$$

2. Если не утеряны 2 диска избыточности и утеряны 2 диска данных, то опять необходимо в зависимости от комбинации блоков решить уже систему из двух полиномиальных уравнений. 
   
   Утеряны 2 блока данных:
   - Если утеряны блок А и блок B:
      
     $$A = X - C$$
     $$B = Y - 2C - A$$
     
   - Если утеряны блок А и блок C:
     
      $$C = (Y - B - A) / 2$$
      $$A = X - B - C$$
     
   - Если утеряны блок B и блок C: 
      $$B = 3X - 2A - Y$$
      $$C = (Y - A - 2B) / 3$$

   Утеряны 1 блок данных и 1 блок избыточности:
   - Если утеряны блок А и блок X:
     
     $$A = Y - 2B - 3C$$

   - Если утеряны блок А и блок Y: 
     
     $$A = X - B - C$$

   - Если утеряны блок B и блок X: 
     
     $$B = (Y - A - 3C) / 2$$

   - Если утеряны блок B и блок Y: 
     
     $$B = X - A - C$$

   - Если утеряны блок C и блок X: 

     $$C = (Y - A - 2B) / 3$$

   - Если утеряны блок C и блок Y: 
     
     $$C = X - A - B$$

### Результаты работы

Приложение предоставляет пользователю возможность выполнять операции чтения и записи данных по указанным адресам. В случае потери дисков, приложение позволяет восстановить данные по запрашиваемому адресу.

![pic2](https://github.com/StanleyStanMarsh/RAID6/assets/96591356/a34ce1fd-7da5-444b-ba39-5830f6005a26)

![pic5](https://github.com/StanleyStanMarsh/RAID6/assets/96591356/a4803417-75ca-459b-a6d8-a32d28097018)

## Description

### Mathematical description

RAID (Redundant Array of Independent Disks) is a technology that allows you to combine multiple physical disks into one logical module to improve system fault tolerance and performance. RAID6 is the sixth RAID level, which provides a high degree of reliability due to double parity.

### RAID6

RAID6 uses data interleaving with double distributed parity, which allows the array to remain accessible even after two consecutive failures. A minimum of four disks are required for RAID6 to function, including two for data and two for parity control.

The distribution of data blocks and redundancy blocks for RAID 6 on 5 disks in the application occurs in accordance with *fig. below*. Thus, in the presented diagram: data blocks - $A1-A3$, $B1-B3$, $C1-C3$, $D1-D3$, $E1-E3$ and redundancy blocks - $A_p$, $A_q$, $B_p$, $B_q$, $C_p$, $C_q$, $D_p$, $D_q$, $E_p$, $E_q$.

![scheme1](https://github.com/StanleyStanMarsh/RAID6/assets/96591356/36271c45-9692-44ca-b7ab-ac48195c6dad)

## A polynomial for generating redundancy blocks

A polynomial expression is used to calculate the redundancy block. The data is transformed into a polynomial, where each block of data becomes substitutable into a polynomial. Data recovery occurs through the solution of a system of polynomial equations created on the basis of stored data and redundant blocks.

### Implementation features

#### Disk representation

In the simulation, disks are represented by text files. One disk is one file. The files are stored in the working directory of the application and are available during operation.

#### Writing and reading data

The write and read commands are used to write and read data.

To write information to disks, the user uses the command:

`<address> write <data>`,

where *address* is the address (string) from 0 to 63 at which the user wants to write a message, *data* is a string-a message that must consist strictly of 14 characters and which must contain only hexadecimal characters.

Determining the location of data blocks and redundancy depends on the specified address - based on dividing the address modulo the number of disks. On a conditional 5x5 matrix, the line number will be selected - the remainder of dividing the address by the number 5 - which contains the numbers of redundant disks. The message is divided into three parts: 5 bytes in the first two and 4 bytes in the last. Each part is then written to a separate data block.

The redundancy blocks are counted using two polynomials and written to the corresponding cells:

$$
A + B + C = X
$$

$$
A + 2B + 3C = Y
$$

The variables of the polynomial $A$, $B$, $C$ correspond sequentially to the data blocks. For example, to calculate the redundancy of p at address 0, the value from the $A1$ block is converted from hexadecimal to decimal and placed in the $A$ variable, the $A2$ block is written to the $B$ variable, and the $A3$ block is written to the $C$ variable.

To read information from disks, the user uses the command:

`<address> read`,

where *address* is the address (string) from 0 to 63 from which the user wants to read the message. First, you should check if the disks have been lost. If the disks are in place, then parts of the message from the data blocks are read from the specified address, which are then collected into a single line and displayed to the user. If one or two disks are missing, then it is necessary to analyze different options: whether blocks of data or redundancy are lost, according to the address specified by the user. In this implementation, lost disks are not restored, but only specific data is restored at the requested address.

All possible combinations of block loss are considered, data is restored and a string is displayed to the user:

1. If two or fewer checksum blocks are lost and no data blocks are lost, then it is enough to simply combine parts of the message from the data blocks into a string and output to the user. Checksum blocks are calculated using polynomials and stored data and written to newly created disks.
   - If data block A is lost, then an equation of the form is solved: $$A = X - B - C$$
- If data block B is lost, then an equation of the form is solved: $$B = X - A - C$$
- If data block C is lost, then an equation of the form is solved: $$C = X - A - B$$

2. If 2 redundancy disks are not lost and 2 data disks are lost, then again, depending on the combination of blocks, it is necessary to solve a system of two polynomial equations. 
   
   2 data blocks are lost:
- If block A and block B are lost:
      
     $$A = X - C$$
     $$B = Y - 2C - A$$
     
   - If block A and block C are lost:
     
      $$C = (Y - B - A) / 2$$
      $$A = X - B - C$$
     
   - If block B and block C are lost:
$$B = 3X - 2A - Y$$
      $$C = (Y - A - 2B) / 3$$

   1 data block and 1 redundancy block are lost:
- If block A and block X are lost:
     
     $$A = Y - 2B - 3C$$

   - If block A and block Y are lost: 
     
     $$A = X - B - C$$

   - If block B and block X are lost: 
     
     $$B = (Y - A - 3C) / 2$$

   - If block B and block Y are lost: 
     
     $$B = X - A - C$$

   - If block C and block X are lost: 

     $$C = (Y - A - 2B) / 3$$

   - If block C and block Y are lost: 
     
     $$C = X - A - B$$

### Results of the work

The application provides the user with the ability to perform data reading and writing operations at specified addresses. In case of loss of disks, the application allows you to restore data to the requested address.

![pic2](https://github.com/StanleyStanMarsh/RAID6/assets/96591356/a34ce1fd-7da5-444b-ba39-5830f6005a26)

![pic5](https://github.com/StanleyStanMarsh/RAID6/assets/96591356/a4803417-75ca-459b-a6d8-a32d28097018)

## License

This repository is under [MIT License](https://github.com/StanleyStanMarsh/RAID6/blob/master/LICENSE).
