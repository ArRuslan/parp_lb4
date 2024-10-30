#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <omp.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>

/*
1 Скласти послідовну та паралельну програми для пошуку ключових даних.
2 Перший файл складається з упорядкованих в алфавітному порядку ключів, кожен записаний  в одному рядку і складається з 15 символів. Перший рядок файла — кількість ключів. Файл гарантовано поміщається в оперативній памяті.
3 Другий файл складається з ключів, які треба знайти. Файл може не поміщатися в оперативній памяті. Для кожного ключа з другого файлу треба знайти його в першому файлі. Результат записати в третій файл. Формат запису для рядків, які знайдено: ….. is found at index … . Формат запису для рядків, які не знайдені:  … is NOT FOUND.
4 Результати для обох програм повинні співпадати.
5 Порівняти програми за часом виконання.
6 Обчислити показники для паралельного режиму.
*/

char lf;

void task_ser() {
    std::ifstream file1("../file1.txt");
    size_t keys_count = file1.tellg();
    file1 >> keys_count;
    file1.read(&lf, 1);

    char (*keys)[15] = new char[keys_count][15];
    for (int i = 0; i < keys_count; i++) {
        file1.read(&keys[i][0], 15);
        file1.read(&lf, 1);
    }
    file1.close();

    std::cout << "Loaded " << keys_count << " keys from first file\n";

    int file2 = open("../file2.txt", O_RDONLY);
    struct stat file2stat;
    char* file2data;
    if (file2 < 0) {
        std::cout << "open :(\n";
        delete keys;
        return;
    }
    if (fstat(file2, &file2stat) < 0) {
        std::cout << "fstat :(\n";
        delete keys;
        return;
    }

    file2data = (char*)mmap(nullptr, file2stat.st_size, PROT_READ, MAP_SHARED, file2, 0);
    if (file2data == (char*)-1) {
        std::cout << "mmap :(\n";
        delete keys;
        return;
    }

    std::ofstream file3("../file3.txt", std::ios::trunc);

    for (int i = 0; i < file2stat.st_size / 16; i++) {
        char key_to_find[15];
        memcpy(&key_to_find[0], file2data + 16 * i, 15);

        bool found = false;
        for(int j = 0; j < keys_count; j++) {
            if(memcmp(&keys[j], &key_to_find, 15) == 0) {
                file3 << std::string(key_to_find, 15) << " is found at index " << j << "\n";
                found = true;
                break;
            }
        }

        if(!found) {
            file3 << std::string(key_to_find, 15) << " is NOT FOUND\n";
        }
    }

    /*std::ifstream file2("../file2.txt");
    file2.seekg(0, std::ios::end);
    size_t file_size = file2.tellg();
    file2.seekg(0);

    char key_to_find[15];
    for (int i = 0; i < file_size / 16; i++) {
        file2.read(&key_to_find[0], 15);
        file2.read(&lf, 1);

        bool found = false;
        for(int j = 0; j < keys_count; j++) {
            if(memcmp(&keys[j], &key_to_find, 15) == 0) {
                file3 << std::string(key_to_find, 15) << " is found at index " << j << "\n";
                found = true;
                break;
            }
        }

        if(!found) {
            file3 << std::string(key_to_find, 15) << " is NOT FOUND\n";
        }
    }

    //file2.close();*/
    file3.close();
    delete keys;
}

void task_par() {
    std::ifstream file1("../file1.txt");
    size_t keys_count = file1.tellg();
    file1 >> keys_count;
    file1.read(&lf, 1);

    char (*keys)[15] = new char[keys_count][15];
    for (int i = 0; i < keys_count; i++) {
        file1.read(&keys[i][0], 15);
        file1.read(&lf, 1);
    }
    file1.close();

    std::cout << "Loaded " << keys_count << " keys from first file\n";

    int file2 = open("../file2.txt", O_RDONLY);
    struct stat file2stat;
    char* file2data;
    if (file2 < 0) {
        std::cout << "open :(\n";
        delete keys;
        return;
    }
    if (fstat(file2, &file2stat) < 0) {
        std::cout << "fstat :(\n";
        delete keys;
        return;
    }

    file2data = (char*)mmap(nullptr, file2stat.st_size, PROT_READ, MAP_SHARED, file2, 0);
    if (file2data == (char*)-1) {
        std::cout << "mmap :(\n";
        delete keys;
        return;
    }

    std::ofstream file3("../file3.txt", std::ios::trunc);

    #pragma omp parallel for
    for (int i = 0; i < file2stat.st_size / 16; i++) {
        char key_to_find[15];
        memcpy(&key_to_find[0], file2data + 16 * i, 15);

        bool found = false;
        for(int j = 0; j < keys_count; j++) {
            if(memcmp(&keys[j], &key_to_find, 15) == 0) {
                file3 << std::string(key_to_find, 15) << " is found at index " << j << "\n";
                found = true;
                break;
            }
        }

        if(!found) {
            file3 << std::string(key_to_find, 15) << " is NOT FOUND\n";
        }
    }

    /*std::ifstream file2("../file2.txt");
    file2.seekg(0, std::ios::end);
    size_t file_size = file2.tellg();
    file2.seekg(0);

    std::ofstream file3("../file3.txt", std::ios::trunc);

    omp_set_num_threads(omp_get_max_threads());

    int i = 0;
    while(i < file_size / 16) {
        int j = 0;
        char (*keys_to_find)[15] = new char[8][15];
        for(; j < 8 && i < file_size / 16; j++) {
            file2.read(&keys_to_find[j][0], 15);
            file2.read(&lf, 1);
            i++;
        }

        #pragma omp parallel for
        for(int j1 = 0; j1 < j; j1--) {
            int32_t found_index = -1;
            for(int k = 0; k < keys_count; k++) {
                if(memcmp(&keys[k], &keys_to_find[j1], 15) == 0) {
                    found_index = k;
                    break;
                }
            }
        }
    }*/


    //#pragma omp parallel
    //{
        /*#pragma omp parallel for
        for (int i = 0; i < file_size / 16; i++) {
            char key_to_find[15];

            /*#pragma omp critical
            {
                file2.read(&key_to_find[0], 15);
                file2.read(&lf, 1);
            /*}

            int32_t found_index = -1;
            for(int j = 0; j < keys_count; j++) {
                if(memcmp(&keys[j], &key_to_find, 15) == 0) {
                    found_index = j;
                    break;
                }
            }

            #pragma omp critical
            {
                if (found_index >= 0) {
                    file3 << std::string(key_to_find, 15) << " is found at index " << found_index << "\n";
                } else {
                    file3 << std::string(key_to_find, 15) << " is NOT FOUND\n";
                }
            }
        }*/
    //}

    //file2.close();
    file3.close();
}

int main() {
    double start_time = omp_get_wtime();
    task_ser();
    double end_time = omp_get_wtime();
    std::cout << "Serial search took " << std::fixed << std::setprecision(3) << end_time - start_time << " seconds\n";

    start_time = omp_get_wtime();
    task_par();
    end_time = omp_get_wtime();
    std::cout << "Parallel search took " << std::fixed << std::setprecision(3) << end_time - start_time << " seconds\n";

    return 0;
}
