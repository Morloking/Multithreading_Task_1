#include <cstddef>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
namespace {
	constexpr std::size_t kMaxClients = 10;
	constexpr auto kArrivalInterval = std::chrono::seconds(1);
	constexpr auto kProcessInterval = std::chrono::seconds(2);

	constexpr const char* kClientAddedMsg = "Client was added. Clients in queue: ";
	constexpr const char* kAllClientsAddedMsg = "All clients was added. ";

	constexpr const char* kClientProcessedMsg = "Client was processed. Clients in queue: ";
	constexpr const char* kQueueEmptyAndWaitingMsg = "Queue is empty... Waiting";
	constexpr const char* kWorkFinishedMsg = "Queue is empty and there will be no more clients. The work is complete.";

	std::mutex my_mutex;
	std::size_t queue_size = 0;
	bool all_clients_added = false;

	void OnClientArrival() {
		for (std::size_t i = 0; i < kMaxClients; ++i) {
			std::this_thread::sleep_for(kArrivalInterval);

			std::lock_guard<std::mutex> lock(my_mutex);
			++queue_size;
			std::cout << kClientAddedMsg << queue_size << "\n";
		} 
		{
			std::lock_guard<std::mutex> lock(my_mutex);
			all_clients_added = true;
			std::cout << kAllClientsAddedMsg << "\n";
		}
	}

	void ProcessClient() {
		while (true) {
			std::this_thread::sleep_for(kProcessInterval);

			std::lock_guard<std::mutex> lock(my_mutex);
			if (all_clients_added && queue_size == 0) {
				std::cout << kWorkFinishedMsg << "\n";
				return;
			}

			if (queue_size > 0) {
				--queue_size;
				std::cout << kClientProcessedMsg << queue_size << "\n";
			}
			else {
				std::cout << kQueueEmptyAndWaitingMsg << "\n";
			}
		}
	}
}

int main() {
	std::thread client_thread(OnClientArrival);
	std::thread process_thread(ProcessClient);

	client_thread.join();
	process_thread.join();
	return 0;
}