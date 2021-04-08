#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <future>
#include <algorithm>

std::vector <int> V;

std::string String() {
	auto randchar = []() -> char
	{
		const char charset[] = "AGTC";
		const size_t max_index = (sizeof(charset) - 1);
		std::random_device rd;
		return charset[rd() % max_index];
	};
	std::string str(1000, 0);
	generate_n(str.begin(), 1000, randchar);
	return str;
}

std::string DNA = String();

class Threads_Guard
{
public:

	explicit Threads_Guard(std::vector < std::thread >& threads) :
		m_threads(threads)
	{}

	Threads_Guard(Threads_Guard const&) = delete;

	Threads_Guard& operator=(Threads_Guard const&) = delete;

	~Threads_Guard() noexcept
	{
		try
		{
			for (std::size_t i = 0; i < m_threads.size(); ++i)
			{
				if (m_threads[i].joinable())
				{
					m_threads[i].join();
				}
			}
		}
		catch (...)
		{
			// std::abort();
		}
	}

private:

	std::vector < std::thread >& m_threads;
};

struct Searcher
{
	void operator()(int first, int last, std::string element,
		std::promise < std::vector<int> >& result, std::atomic < bool >& flag) noexcept
	{
		try
		{
			int position = first;
			//  for (position = DNA.find(element, position++); position != pos  ; position = DNA.find(element, position+1) )
			//  { 
			//     cout << position << endl;
			//     //V.push_back(position);
			// 	pos = position;
			// }
			int pos = last;
			while (position != pos) {
				pos = position;
				position = DNA.find(element, position++);
				int n = 0;
				if (position != -1 && n == 0) {
					//V.push_back(position);
					std::cout << position << std::endl;
				}
			};

			result.set_value(V);
			flag.store(true);
		}
		catch (...)
		{
			try
			{
				result.set_exception(std::current_exception());
				flag.store(true);
			}
			catch (...)
			{
				// ...
			}
		}
	}
};

std::vector<int> parallel_find(std::string main, std::string element)
{
	const size_t length = main.size();

	if (!length) {
		std::vector <int> v;
		return v;
	}

	const size_t min_per_thread = 25;
	const size_t max_threads = (length + min_per_thread - 1) / min_per_thread;
	const size_t hardware_threads = std::thread::hardware_concurrency();
	const size_t num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
	const size_t block_size = length / num_threads;
	std::promise <std::vector<int>> result;
	std::atomic < bool > flag(false);
	std::vector < std::thread > threads(num_threads - 1);

	{
		Threads_Guard guard(threads);

		int block_start = 0;

		for (std::size_t i = 0; i < (num_threads - 1); ++i)
		{
			int block_end = block_start;
			block_end += block_size + element.size() - 1;

			threads[i] = std::thread(Searcher(),
				block_start, block_end, element, std::ref(result), std::ref(flag));

			block_start = block_end;
		}

		Searcher()(block_start, main.size(), element, result, flag);
	}

	if (!flag.load())
	{
		std::vector <int> v;
		return v;
	}
	return result.get_future().get();
}


int main() {

	std::string search_DNA;
	std::cout << "enter the DNA code you want to find" << std::endl;
	std::cin >> search_DNA;
	parallel_find(DNA, search_DNA);
	// for (auto i : V) {
	//     cout << i << endl;
	// }
	return 0;
}