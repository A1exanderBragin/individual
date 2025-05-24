import mpi.*;
import java.io.*;
import java.nio.file.*;
import java.util.*;

public class FileSumMPI {
    public static void main(String[] args) throws Exception {
        MPI.Init(args);

        int rank = MPI.COMM_WORLD.Rank();
        int size = MPI.COMM_WORLD.Size();

        // Массив имен файлов (можно передавать как аргументы)
        String[] filenames = {"file1.txt", "file2.txt", "file3.txt", "file4.txt"};
        int n = filenames.length;

        if (n < size) {
            if (rank == 0) {
                System.err.println("Number of files (" + n + ") is less than number of processes (" + size + ")");
            }
            MPI.Finalize();
            return;
        }

        // Распределение файлов по процессам
        int filesPerProc = n / size;
        int extraFiles = n % size;

        int startFile, endFile;
        if (rank < extraFiles) {
            startFile = rank * (filesPerProc + 1);
            endFile = startFile + filesPerProc + 1;
        } else {
            startFile = extraFiles * (filesPerProc + 1) + (rank - extraFiles) * filesPerProc;
            endFile = startFile + filesPerProc;
        }

        // Каждый процесс вычисляет сумму для своих файлов
        double localSum = 0.0;
        for (int i = startFile; i < endFile; i++) {
            localSum += calculateFileSum(filenames[i]);
            if (rank == 0) {
                System.out.println("Process " + rank + " processed file " + filenames[i]);
            }
        }

        // Сбор результатов на процессе 0
        double[] totalSum = new double[1];
        MPI.COMM_WORLD.Reduce(new double[]{localSum}, 0, totalSum, 0, 1, MPI.DOUBLE, MPI.SUM, 0);

        // Процесс 0 записывает результат в файл
        if (rank == 0) {
            try (PrintWriter out = new PrintWriter("res.txt")) {
                out.println(totalSum[0]);
                System.out.println("Total sum: " + totalSum[0] + " written to res.txt");
            } catch (IOException e) {
                System.err.println("Error writing to res.txt: " + e.getMessage());
            }
        }

        MPI.Finalize();
    }

    private static double calculateFileSum(String filename) {
        try {
            return Files.lines(Paths.get(filename))
                    .flatMap(line -> Arrays.stream(line.split("\\s+")))
                    .filter(s -> !s.isEmpty())
                    .mapToDouble(Double::parseDouble)
                    .sum();
        } catch (IOException | NumberFormatException e) {
            System.err.println("Error processing file " + filename + ": " + e.getMessage());
            return 0.0;
        }
    }
}
