#include <iostream>
#include <cmath>
#include <omp.h>
#include <map>
#include <functional>

using namespace std;

int main() {
    // Словарь доступных функций
    map<string, function<double(double)>> functions = {
        {"sin(x)", [](double x) { return sin(x); }},
        {"cos(x)", [](double x) { return cos(x); }},
        {"exp(x)", [](double x) { return exp(x); }},
        {"log(x)", [](double x) { return log(x); }},
        {"x^2",    [](double x) { return x*x; }},
        {"x^3",    [](double x) { return x*x*x; }},
        {"sqrt(x)", [](double x) { return sqrt(x); }}
    };

    cout << "Доступные функции:" << endl;
    int index = 1;
    for (const auto& func : functions) {
        cout << index++ << ". " << func.first << endl;
    }

    // Выбор функции
    int choice;
    cout << "\nВыберите функцию (1-" << functions.size() << "): ";
    cin >> choice;

    if (choice < 1 || choice > functions.size()) {
        cerr << "Неверный выбор. Используется sin(x) по умолчанию." << endl;
        choice = 1;
    }

    auto it = functions.begin();
    advance(it, choice - 1);
    function<double(double)> f = it->second;

    // Ввод параметров
    double a, b;
    int n;
    cout << "\nВведите начало отрезка a: ";
    cin >> a;
    cout << "Введите конец отрезка b: ";
    cin >> b;
    cout << "Введите количество точек n (>=3): ";
    cin >> n;

    if (n < 3) {
        cerr << "Количество точек должно быть >=3. Установлено n=100." << endl;
        n = 100;
    }

    const double h = (b - a) / n;
    double* x = new double[n];
    double* df = new double[n];

    // Инициализация массива x
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        x[i] = a + i * h;
    }

    double start_time = omp_get_wtime();

    // Вычисление производной
    #pragma omp parallel for
    for (int i = 1; i < n - 1; i++) {
        df[i] = (f(x[i+1]) - f(x[i-1])) / (2 * h);
    }

    // Граничные точки
    df[0] = (-3*f(x[0]) + 4*f(x[1]) - f(x[2])) / (2 * h);
    df[n-1] = (3*f(x[n-1]) - 4*f(x[n-2]) + f(x[n-3])) / (2 * h);

    double end_time = omp_get_wtime();

    // Вывод результатов
    cout << "\nПроизводная функции " << it->first << " на отрезке [" 
         << a << ", " << b << "]:" << endl;
    cout << "Точка\t\tПроизводная" << endl;
    for (int i = 0; i < n; i += max(1, n/10)) {
        printf("%.4f\t\t%.4f\n", x[i], df[i]);
    }

    cout << "\nВремя вычисления: " << end_time - start_time << " секунд" << endl;

    delete[] x;
    delete[] df;

    return 0;
}
