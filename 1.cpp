#include <iostream>
#include <thread>
#include <random>
#include <future>


std::mutex g_mutex;

void counting_points(int& in_circle, int n) {

    std::lock_guard<std::mutex> lock(g_mutex);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, n);
    int x = dis(gen);
    int y = dis(gen);
    if (x * x + y * y <= n * n) {
        in_circle++;
    }
}
int main() {

    int in_circle = 0;
    int n;


    std::cout << "enter the number of points: ";
    std::cin >> n;

    std::vector <std::thread> threads;
    for (int i = 0; i < n; i++)
    {
        threads.push_back(std::thread(counting_points, std::ref(in_circle), n));
    }


    for (int i = 0; i < n; i++)
    {
        threads[i].join();
    }

    std::cout << "the value of pi: " << 4.0 * float(in_circle) / float(n) << std::endl;

    return 0;
}