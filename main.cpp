#include <iostream>
#include <cstdio>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <ctime>
#include <queue>

int TOTAL_MEM = 80 * 1024 * 1024; // 80 MB

struct HeapNode {
    std::string sentence;
    int index;

    HeapNode(std::string a, int b) : sentence(std::move(a)), index(b) {}

    bool operator<(const HeapNode &compare_node) const {
        return (sentence.compare(compare_node.sentence) > 0);
    }
};

int input(const std::string &input_name) {
    std::ifstream input(input_name.c_str());

    if (!input.good()) {
        std::cout << "File input is not found!\n Exit program!\n";
        exit(-1);
    }

    int chunks_count = 0;
    int chunk_memory_size = 0;

    std::ofstream output;
    std::vector<std::string> sentences;
    sentences.clear();

    while (!input.eof()) {
        std::string sentence;
        getline(input, sentence);
        if (sentence.empty()) {
            continue;
        }
        if (chunk_memory_size + sentence.size() < TOTAL_MEM) {
            chunk_memory_size += sentence.size() + 1;
        } else {
            sort(sentences.begin(), sentences.end());

            chunks_count++;
            std::stringstream ss;
            ss << "run_" << chunks_count << ".txt";
            output.open(ss.str());

            int data_size = sentences.size();
            for (int i = 0; i < data_size - 1; i++) {
                output << sentences[i] + "\n";
            }

            if (data_size > 0) {
                output << sentences[data_size - 1];
            }
            output.close();
            sentences.clear();
            chunk_memory_size = sentence.size();
        }
        sentences.push_back(sentence);
    }


    if (!sentences.empty()) {
        sort(sentences.begin(), sentences.end());

        chunks_count++;
        std::stringstream ss;
        ss << "run_" << chunks_count << ".txt";
        output.open(ss.str());

        int data_size = sentences.size();
        for (int i = 0; i < data_size - 1; i++) {
            output << sentences[i] + "\n";
        }
        input.close();
        output << sentences[data_size - 1];
        output.close();
    }

    return chunks_count;
}

void merge(int start, int end, int location) {

    int chunks_count = end - start + 1;

    std::ifstream input[chunks_count];
    for (int i = 0; i < chunks_count; i++) {
        std::stringstream ss;
        ss << "run_" << start + i << ".txt";
        input[i].open(ss.str());
    }

    std::priority_queue<HeapNode, std::vector<HeapNode> > heap;

    std::ofstream output;
    std::stringstream ss;
    ss << "run_" << location << ".txt";
    output.open(ss.str());

    for (int i = 0; i < chunks_count; i++) {
        std::string sentence;
        if (!input[i].eof()) {
            getline(input[i], sentence);
            heap.push(HeapNode(sentence, i));
        }
    }

    while (!heap.empty()) {
        std::string sentence = heap.top().sentence;
        int index = heap.top().index;
        heap.pop();

        output << sentence + "\n";

        if (!input[index].eof()) {
            getline(input[index], sentence);
            heap.push(HeapNode(sentence, index));
        }
    }

    for (int i = 0; i < chunks_count; i++) {
        input[i].close();
    }

    output.close();
}

void mergeAllChunks(int chunks_count, const std::string &output_name) {

    int start = 1;
    int end = chunks_count;
    while (start < end) {
        int location = end;
        int distance = 100;
        int time = (end - start + 1) / distance + 1;
        if ((end - start + 1) / time < distance) {
            distance = (end - start + 1) / time + 1;
        }
        while (start <= end) {
            int mid = std::min(start + distance, end);
            location++;
            merge(start, mid, location);
            start = mid + 1;
        }
        end = location;
    }

    std::stringstream ss;
    ss << "run_" << start << ".txt";
    rename(ss.str().c_str(), output_name.c_str());

    for (int i = 1; i < end; i++) {
        std::stringstream temp_ss;
        temp_ss << "run_" << i << ".txt";
        remove(temp_ss.str().c_str());
    }
}

int main(int argc, char *argv[]) {
    if (!(argc == 3 || argc == 1)) {
        std::cout << "Wrong setup: input_file, output file\n";
        return 0;
    }

    clock_t begin_time = clock();

    std::string input_name = argv[1];
    std::string output_name = argv[2];

    int chunks_size = input(input_name);

    mergeAllChunks(chunks_size, output_name);

    std::cout << "Success!\nExecution time:" << float(clock() - begin_time) / CLOCKS_PER_SEC * 1000 << " msec\n";
    return 0;
}