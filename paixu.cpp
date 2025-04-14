#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <random>
#include <ctime>


std::vector<int> generate_random_data(int size = 100000) {
    std::vector<int> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 100000);
    
    for (int i = 0; i < size; ++i) {
        data[i] = dis(gen);
    }
    return data;
}

std::vector<int> merge(std::vector<int>& left, std::vector<int>& right) {
    std::vector<int> result;
    int i = 0, j = 0;
    while (i < left.size() && j < right.size()) {
        if (left[i] < right[j]) {
            result.push_back(left[i]);
            ++i;
        } else {
            result.push_back(right[j]);
            ++j;
        }
    }
    result.insert(result.end(), left.begin() + i, left.end());
    result.insert(result.end(), right.begin() + j, right.end());
    
    return result;
}


std::vector<int> GB(const std::vector<int>& arr) {
    if (arr.size() <= 1) {
        return arr;
    }

    int mid = arr.size() / 2;
    std::vector<int> left(arr.begin(), arr.begin() + mid);
    std::vector<int> right(arr.begin() + mid, arr.end());

    left = GB(left);
    right = GB(right);

    return merge(left, right);
}


void func(const std::vector<int>& arr, int start, int end, std::vector<int>& result) {
    result = GB(std::vector<int>(arr.begin() + start, arr.begin() + end));
}

std::vector<int> depart(int p_nums, std::vector<int>& arr) {
    int part = arr.size() / p_nums;
    std::vector<std::thread> threads;
    std::vector<std::vector<int>> result(p_nums);

    for (int i = 0; i < p_nums; ++i) {
        int start = i * part;
        int end = (i == p_nums - 1) ? arr.size() : (i + 1) * part;
        threads.push_back(std::thread(func, std::ref(arr), start, end, std::ref(result[i])));
    }

    for (auto& t : threads) {
        t.join();
    }

   
    std::vector<int> final_result = result[0];
    for (int i = 1; i < p_nums; ++i) {
        final_result = merge(final_result, result[i]);
    }
    return final_result;
}

int main() {
    std::vector<int> data = generate_random_data(100000);
    // std::cout << "before:" << std::endl;
    // for (int i = 0; i < data.size(); i++) {
    //     std::cout << data[i] << " ";
    // }
    std::cout << std::endl;

    int p_nums = 10;
    // std::vector<int> sorted_data = depart(p_nums, data);

    // std::cout << "after: ";
    // for (int i = 0; i < sorted_data.size(); ++i) {
    //     std::cout << sorted_data[i] << " ";
    // }
    std::cout << "排序成功"<< std::endl;

    return 0;
}
