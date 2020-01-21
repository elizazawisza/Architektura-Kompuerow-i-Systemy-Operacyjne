#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_LENGTH 1024
typedef unsigned char *my_va_list;
//oczytuje kolejne argumenty i przekształca je do odpowiedniego typu danych
#define my_va_arg(my_args, type) (*(type *)((my_args += sizeof(type)) - sizeof(type)))
#define my_va_start(my_args, variable) (my_args = ((my_va_list)&variable) + sizeof(variable))
#define my_va_end(my_args) (my_args = NULL)


//konwersja int na string
char* conv(unsigned int n, int base){
    static char values[] = "0123456789ABCDEF";
    char tmp_Number[50];
    int i=0;
    //printf("%d\n", n);
    while(n != 0){
        tmp_Number[i] = values[n%base];
        n/=base;
        i++;
    }
    //printf("%d\n", i);
    char* final_Number = malloc(i*sizeof(char));
    for(int j = i-1; j >=0 ; j-- ){
      //printf("będzie dobrze %d\n", j);
      final_Number[i-j-1] = tmp_Number[j];
    }
    final_Number[i]='\0';
    //printf("Ostetcznie i definitywnie: %s \n\n", final_Hex);
    return final_Number;
}

int conv2(char *string, int base){
    int i, sign, offset, n;
    //sprawdzenie znaku liczby
    if(string[0]=='-'){
      sign = -1;
    }
    //ustalenie pozycji do konwersji
    if(sign == -1){
      offset = 1;
    }
    else{
      offset = 0;
    }

    n = 0;
    for(i = offset; i<strlen(string)-1; i++){
      if(string[i]>='A' && string[i]<='F'){
          n = n * base + (string[i] - 'A'+ 10);
      }
      else
      {
        n = n * base + string[i] - '0';
      }
    }

    if(sign == -1){
      n = -n;
    }
    return n;
}


void my_printf(char *format, ...){
    int numbers;
    int length;
    char *strings;
    my_va_list arg;
    my_va_start(arg, format);
    while(*format){
        if(*format == '%'){
            format++;
            if(*format=='s'){
                //Ciąg znaków
                strings = my_va_arg(arg, char *);
                length = strlen(strings);
                strings[length-1]='\0';
                write(1, strings, length);
            }
            else if(*format == 'd'){
                //Liczba dziesiętna
                numbers = my_va_arg(arg, int);
                length = strlen(conv(numbers,10));
                write(1, conv(numbers,10), length);
            }
            else if(*format == 'x'){
                //Liczba w systemie szesnastkowym
                numbers = my_va_arg(arg, unsigned int);
                length = strlen(conv(numbers,16));
                write(1, conv(numbers,16), length);
            }
            else if(*format == 'b'){
                //Liczba w systemie binarnym
                numbers = my_va_arg(arg, unsigned int);
                length = strlen(conv(numbers,2));
                write(1, conv(numbers,2), length);
            }
            format++;
        }
        else{
            write(1, (void *)format, sizeof(*format));
            format++;
        }

    }

    my_va_end(arg);
}

void my_scanf(char *format, ...){
  int *numbers;
  char **strings;
  my_va_list arg;
  my_va_start(arg, format);
  while(*format){
      if(*format == '%'){
          format++;
          if(*format=='s'){
              //Ciąg znaków
              strings = my_va_arg(arg, char **);
              char *input = malloc(BUFF_LENGTH);
              read(0, input, BUFF_LENGTH);
              *strings = input;
          }
          else if(*format == 'd'){
              //Liczba dziesiętna
              numbers = my_va_arg(arg, int*);
              char *input = malloc(BUFF_LENGTH);
              read(0, input, BUFF_LENGTH);
              *numbers = conv2(input, 10);
              //printf("% \n", *numbers);

          }
          else if(*format == 'x'){
              //Liczba w systemie szesnastkowym
              numbers = my_va_arg(arg, int*);
              char *input = malloc(BUFF_LENGTH);
              read(0, input, BUFF_LENGTH);
              *numbers = conv2(input, 16);
          }
          else if(*format == 'b'){
              //Liczba w systemie binarnym
              numbers = my_va_arg(arg, int*);
              char *input = malloc(BUFF_LENGTH);
              read(0, input, BUFF_LENGTH);
              *numbers = conv2(input,2);
          }
          format++;
      }
  }

  my_va_end(arg);
}
int main(int argc, char ** argv){
  int test, binary, hex;
  char *slowo;
    my_printf("%d, %x, %b, %s\n\n%s\n", 123,255,7, "Czy ty na pewno działasz?", "Chyba jednak nie");
    // my_scanf("%d", &test);
    // my_printf("Liczba: %d\n", test);
    // my_scanf("%s", &slowo);
    // my_printf("Wyraz: %s\n", slowo);
    // my_scanf("%b", &binary);
    // my_printf("Binarna: %b\n", binary);
    // my_scanf("%x", &hex);
    // my_printf("Szesnastkowa: %x\n", hex);


  //  my_printf("%x", 255);
  //  my_printf("%b", 7);
    return 0;
}
