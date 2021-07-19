#include <iostream>
#include <filesystem>
#include <chrono>

#include "threadpool.h"

namespace fs = std::filesystem;

C_thread_pool pool(std::thread::hardware_concurrency());
const fs::path exeName = fs::path("FileOrganizer.exe");
const fs::path curPath = fs::current_path();

void recurse_directory(fs::path folder);
void sort_files(fs::path file);

int main()
{

	std::chrono::seconds elapsed, startTime = std::chrono::seconds(
		std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()
			));

	//INIT THINGS
	elapsed = std::chrono::seconds(
		std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()
			)) - startTime;
	std::cout << "Thread pool successfully initialized, posting jobs... " <<
		elapsed.count() << " seconds." << std::endl;

	//ITERATE THROUGH DIRECTORY
	recurse_directory(curPath);

	elapsed = std::chrono::seconds(
		std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()
			)) - startTime;
	std::cout << "Finished iterating through directory, waiting for queue to finish..." <<
		elapsed.count() << " seconds." << std::endl;

	//WAIT FOR QUEUE TO EMPTY
	const int timeToExit = 15;
	std::chrono::seconds lastTime(-1);
	std::chrono::seconds timeNoJobs(0);
	while (timeNoJobs.count() < timeToExit)
	{
		if (pool.current_jobs() > 0) {
			lastTime = std::chrono::seconds(-1);
			timeNoJobs = std::chrono::seconds(0);
		}
		else {
			if (lastTime.count() == -1) {
				lastTime = std::chrono::seconds(
					std::chrono::duration_cast<std::chrono::seconds>(
						std::chrono::system_clock::now().time_since_epoch()
						));
			}
			timeNoJobs = std::chrono::seconds(
				std::chrono::duration_cast<std::chrono::seconds>(
					std::chrono::system_clock::now().time_since_epoch()
					)) - lastTime;
		}
	}

	elapsed = std::chrono::seconds(
		std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()
			)) - startTime;
	std::cout << "Went " << timeToExit << " seconds without a job, assuming done! " <<
		elapsed.count() << " seconds." << std::endl;

	return 0;

}

void recurse_directory(fs::path folder)
{
	try {
		for (const auto& it : fs::directory_iterator(folder)) {
			if (!it.is_directory()) {
				if (it.path().filename() != exeName) {
					pool.add_job(std::bind(sort_files, it.path()));
				}
			}
			else {
				recurse_directory(it);
			}
		}
	} catch (fs::filesystem_error& e) {
		std::cout << e.what() << std::endl;
	}
}

void sort_files(fs::path file)
{
	try {
		fs::path path;
		std::string ext = file.extension().string();
		if (ext != "") {
			path = curPath / ("fsr" + ext);
		}
		else {
			path = curPath / "fsrNoExtension";
		}

		fs::create_directory(path);

		fs::rename(file, (path / file.filename()));
	} catch (fs::filesystem_error& e) {
		std::cout << e.what() << std::endl;
	}
}