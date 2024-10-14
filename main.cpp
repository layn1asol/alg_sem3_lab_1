#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <queue>
#include <filesystem>

void splitFile(const std::string& filename) {
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::vector<int> data;
    std::string content;
    while (inputFile >> content) {
        data.push_back(std::stoi(content));
    }
    inputFile.close();

    // split into 10 parts
    int size = data.size() / 10;
    for (int i = 0; i < 10; ++i) {
        std::ofstream tempFile("C:/Users/User/Desktop/alg1/temp_part_" + std::to_string(i) + ".txt");
        auto start = data.begin() + i * size;
        auto end = (i == 9) ? data.end() : start + size;

        std::vector<int> tempData(start, end);

        std::sort(tempData.begin(), tempData.end());

        for (const auto& num : tempData) {
            tempFile << num << " ";
        }
        tempFile.close();
    }
}

// merge 10 parts into one file
void mergeFiles() {
    std::ofstream outputFile("C:/Users/User/Desktop/alg1/temp_sort.txt");
    for (int i = 0; i < 10; ++i) {
        std::ifstream tempFile("C:/Users/User/Desktop/alg1/temp_part_" + std::to_string(i) + ".txt");
        if (!tempFile.is_open()) {
            std::cerr << "Error opening temp file: temp_part_" << i << std::endl;
            return;
        }

        std::string content;
        while (tempFile >> content) {
            outputFile << content << " ";
        }
        tempFile.close();
    }
    outputFile.close();
}

class PolyphaseSort {
public:
    void PolyphaseSorting(const std::string& inputFile, const std::string& outputFile);
private:
    int diskInteractions = 0;
    std::vector<int> generateFibonacci(int n);
    void distributeRuns(const std::string& inputFile, std::vector<std::ofstream>& tapes, int numTapes);
    void mergeRuns(std::vector<std::ifstream>& inputTapes, std::ofstream& outputTape);
};

std::vector<int> PolyphaseSort::generateFibonacci(int n) {
    std::vector<int> fib(n, 1);
    for (int i = 2; i < n; ++i) {
        fib[i] = fib[i - 1] + fib[i - 2];
    }
    return fib;
}

void PolyphaseSort::distributeRuns(const std::string& inputFile, std::vector<std::ofstream>& tapes, int numTapes) {
    std::ifstream inFile(inputFile);
    if (!inFile) {
        std::cerr << "Error opening input file." << std::endl;
        return;
    }
    diskInteractions++;

    std::vector<int> data;
    int num;
    while (inFile >> num) {
        data.push_back(num);
        diskInteractions++;
    }

    std::sort(data.begin(), data.end());

    int fibIndex = 0;
    auto fibonacci = generateFibonacci(numTapes);
    int runSize = data.size() / fibonacci[numTapes - 1];
    for (int i = 0; i < data.size(); ++i) {
        tapes[fibIndex] << data[i] << " ";
        diskInteractions++;
        if ((i + 1) % runSize == 0) {
            fibIndex = (fibIndex + 1) % numTapes;
        }
    }
}

void PolyphaseSort::mergeRuns(std::vector<std::ifstream>& inputTapes, std::ofstream& outputTape) {
    auto cmp = [](std::pair<int, int> a, std::pair<int, int> b) { return a.first > b.first; };
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(cmp)> minHeap(cmp);

    for (int i = 0; i < inputTapes.size(); ++i) {
        int value;
        if (inputTapes[i] >> value) {
            minHeap.push({ value, i });
            diskInteractions++;
        }
    }

    while (!minHeap.empty()) {
        auto [value, tapeIndex] = minHeap.top();
        minHeap.pop();
        outputTape << value << " ";
        diskInteractions++;

        if (inputTapes[tapeIndex] >> value) {
            minHeap.push({ value, tapeIndex });
            diskInteractions++;
        }
    }
}

void PolyphaseSort::PolyphaseSorting(const std::string& inputFile, const std::string& outputFile) {
    int numTapes = 3; // number of tapes

    std::vector<std::ofstream> outputTapes(numTapes);
    for (int i = 0; i < numTapes; ++i) {
        outputTapes[i].open("C:/Users/User/Desktop/alg1/tape" + std::to_string(i) + ".txt");
        diskInteractions++;
    }

    distributeRuns(inputFile, outputTapes, numTapes);

    for (auto& tape : outputTapes) {
        tape.close();
    }

    std::vector<std::ifstream> inputTapes(numTapes);
    for (int i = 0; i < numTapes; ++i) {
        inputTapes[i].open("C:/Users/User/Desktop/alg1/tape" + std::to_string(i) + ".txt");
        diskInteractions++;
    }

    std::ofstream outFile(outputFile);
    diskInteractions++;
    mergeRuns(inputTapes, outFile);

    for (auto& tape : inputTapes) {
        tape.close();
    }

    outFile.close();

    for (int i = 0; i < numTapes; ++i) {
        std::filesystem::remove("C:/Users/User/Desktop/alg1/tape" + std::to_string(i) + ".txt");
    }

    std::ofstream outFileAppend(outputFile, std::ios_base::app);
    outFileAppend << "\nSorting method used: Polyphase Sort";
    outFileAppend << "\nNumber of disk interactions: " << diskInteractions;
    outFileAppend.close();
}

int main() {
    std::string inputFilename = "C:/Users/User/Desktop/alg1/input2.txt";

    // MODIFICATION //
    //split the file into 10 parts and sort them with std::sort
    splitFile(inputFilename);
    //merge the sorted parts into a new file
    mergeFiles();

    //sort the merged file using the polyphase sorting algorithm
    PolyphaseSort sorter;
    sorter.PolyphaseSorting("C:/Users/User/Desktop/alg1/temp_sort.txt", "C:/Users/User/Desktop/alg1/output.txt");

    for (int i = 0; i < 10; ++i) {
        std::filesystem::remove("C:/Users/User/Desktop/alg1/temp_part_" + std::to_string(i) + ".txt");
    }
    std::filesystem::remove("C:/Users/User/Desktop/alg1/temp_sort.txt");

    std::cout << "Sorting complete. The sorted data is in output.txt" << std::endl;
    return 0;
}

