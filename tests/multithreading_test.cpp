#include "../build/metadata.h"
#include "../build/plain_text_extractor.h"
#include <iostream>
#include "pthread.h"
#include <string>
#include <vector>

using namespace doctotext;

bool verbose;

void* thread_func(void* data)
{
	std::string* file_name = (std::string*)data;
	PlainTextExtractor extractor;
	extractor.setVerboseLogging(verbose);
	Metadata meta;
	if (!extractor.extractMetadata(*file_name, meta))
		return new bool(false);
	std::cout << "Author: " << meta.author() << std::endl;
	std::cout << "Last modified by: " << meta.lastModifiedBy() << std::endl;
	std::string text;
	if (!extractor.processFile(*file_name, text))
		return new bool(false);
	std::cout << text << std::endl;
	pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
	std::vector<std::string> file_names;
	verbose = false;
	if (argc >= 3 && std::string(argv[1]) == "--verbose")
	{
		verbose = true;
		for (int i = 2; i < argc; i++)
			file_names.push_back(argv[i]);
	}
	else if (argc >= 2)
	{
		for (int i = 1; i < argc; i++)
			file_names.push_back(argv[i]);
	}
	else
		return 1;
	std::vector<pthread_t> threads;
	pthread_t thread;
	for (int i = 0; i < file_names.size(); i++)
	{
		std::cerr << "Starting thread " << i << std::endl;
		int res = pthread_create(&thread, NULL, thread_func, (void*)&file_names[i]);
		if (res)
		{
			std::cerr << "Error creating thread: " << res << "\n";
			return 1;
		}
		threads.push_back(thread);
	}
	bool all_ok = true;
	for (int i = 0; i < threads.size(); i++)
	{
		void *status;
		int res = pthread_join(threads[i], &status);
		if (!res)
			std::cerr << "Thread " << i << " finished successfully." << std::endl;
		else
		{
			std::cerr << "Thread " << i << " finished with error." << std::endl;
			all_ok = false;
		}
	}
	return (all_ok ? 0 : 1);
}
