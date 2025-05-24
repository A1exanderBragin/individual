#include <iostream>
using namespace std;

int main() {
    cout << "Введите натуральное число N: ";
    int N;
    cin >> N;
    
    int isPrime = 1;  // 1 - простое, 0 - составное
    int sumMultiples = 0;
    int oddCount = 0;
    int temp = N;
    
    _asm {
        // Проверка на простоту
        mov eax, N
        cmp eax, 1
        jle NotPrime  // Числа <=1 не простые
        
        mov ebx, 2    // Делитель
        mov ecx, eax  // Сохраняем N в ecx
        
    PrimeLoop:
        mov eax, ebx
        mul ebx       // ebx*ebx
        cmp eax, ecx  // Сравниваем ebx^2 с N
        jg EndPrimeCheck
        
        mov eax, ecx  // Восстанавливаем N
        xor edx, edx
        div ebx       // Делим N на ebx
        
        cmp edx, 0    // Проверяем остаток
        je FoundDivisor
        
        inc ebx
        jmp PrimeLoop
        
    FoundDivisor:
        mov isPrime, 0
        jmp EndPrimeCheck
        
    NotPrime:
        mov isPrime, 0
        
    EndPrimeCheck:
        // Анализ цифр числа
        mov eax, temp
        mov ebx, 10   // Для деления на 10
        mov sumMultiples, 0
        mov oddCount, 0
        
    DigitLoop:
        xor edx, edx
        div ebx       // Делим на 10, в edx - последняя цифра
        
        // Проверка на кратность 3
        push eax
        mov eax, edx
        xor edx, edx
        mov ecx, 3
        div ecx
        cmp edx, 0
        pop eax
        jne NotMultiple
        
        // Если кратно 3, добавляем к сумме
        add sumMultiples, [esp+4]  // Добавляем цифру (бывшую в edx)
        
    NotMultiple:
        // Проверка на нечетность
        test byte ptr [esp+4], 1
        jz EvenDigit
        
        // Если нечетная, увеличиваем счетчик
        inc oddCount
        
    EvenDigit:
        // Продолжаем цикл, пока есть цифры
        cmp eax, 0
        jne DigitLoop
        
        // Выбираем что выводить
        cmp isPrime, 1
        jne OutputOddCount
        
        // Выводим сумму кратных 3
        mov eax, sumMultiples
        jmp DisplayResult
        
    OutputOddCount:
        mov eax, oddCount
        
    DisplayResult:
        // Результат уже в eax
    }
    
    if (isPrime) {
        cout << "Число простое. Сумма цифр, кратных 3: " << sumMultiples << endl;
    } else {
        cout << "Число не простое. Количество нечетных цифр: " << oddCount << endl;
    }
    
    return 0;
}
