#include <iostream>
#include <thread>
#include <random>
#include <future>

std::mutex g_mutex;

void counting_points(int N, int& in_circle, int n) {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, n);
    std::uniform_int_distribution<> dis_(1, n);

    int count = 0;
    for (auto j = 0; j < N; j++) {
        int x = dis(gen);
        int y = dis_(gen);

        if (x * x + y * y <= n * n) {
            count++;
        }
    }

    g_mutex.lock();
    std::lock_guard <std::mutex> lock(std::mutex);

    in_circle += count;

    g_mutex.unlock();

}
int main() {

    int in_circle = 0;
    int n;


    std::cout << "enter the number of points: ";
    std::cin >> n;

    int m = std::thread::hardware_concurrency() != 0 ? std::thread::hardware_concurrency() : 4;
    int N = n / m;

    std::vector <std::thread> threads(std::thread::hardware_concurrency());
    for (int i = 0; i < threads.size(); i++)
    {
        threads[i] = std::thread(counting_points, N, std::ref(in_circle), n);
    }


    for (int i = 0; i < threads.size(); i++)
    {
        threads[i].join();
    }

    std::cout << in_circle << std::endl;

    std::cout << "The value of pi: " << 4.0 * float(in_circle) / float(n) << std::endl;

    return 0;
}