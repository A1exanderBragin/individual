
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <mpi.h>

using namespace std;

double calculate_file_sum(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return 0.0;
    }

    double sum = 0.0;
    double num;
    while (file >> num) {
        sum += num;
    }
    file.close();
    return sum;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Массив имен файлов (в реальной программе можно считать из входных данных)
    const char* filenames[] = {"file1.txt", "file2.txt", "file3.txt", "file4.txt"};
    const int n = sizeof(filenames) / sizeof(filenames[0]);

    if (n < size) {
        if (rank == 0) {
            cerr << "Number of files (" << n << ") is less than number of processes (" << size << ")" << endl;
        }
        MPI_Finalize();
        return 1;
    }

    // Распределяем файлы по процессам
    int files_per_proc = n / size;
    int extra_files = n % size;

    int start_file, end_file;
    if (rank < extra_files) {
        start_file = rank * (files_per_proc + 1);
        end_file = start_file + files_per_proc + 1;
    } else {
        start_file = extra_files * (files_per_proc + 1) + (rank - extra_files) * files_per_proc;
        end_file = start_file + files_per_proc;
    }

    // Каждый процесс вычисляет сумму для своих файлов
    double local_sum = 0.0;
    for (int i = start_file; i < end_file; i++) {
        local_sum += calculate_file_sum(filenames[i]);
        if (rank == 0) {
            cout << "Process " << rank << " processed file " << filenames[i] << endl;
        }
    }

    // Собираем результаты на процессе 0
    double total_sum = 0.0;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Процесс 0 записывает результат в файл
    if (rank == 0) {
        ofstream outfile("res.txt");
        if (outfile.is_open()) {
            outfile << total_sum;
            outfile.close();
            cout << "Total sum: " << total_sum << " written to res.txt" << endl;
        } else {
            cerr << "Error opening output file res.txt" << endl;
        }
    }

    MPI_Finalize();
    return 0;
}
