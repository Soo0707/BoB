#ifndef BOB_THREAD_POOL
#define BOB_THREAD_POOL

#include <vector>
#include <thread>
#include <cassert>
#include <cstddef>
#include <condition_variable>
#include <mutex>

namespace bob
{
	class thread_pool;
	struct job_wrapper
	{
		void* data;
		void* lambda;
		uint32_t begin;
		uint32_t end;
		void (thread_pool::*pmf)(const job_wrapper&) const noexcept;
	};

	class thread_pool
	{
		public:
			thread_pool(const size_t n) :
				m_Barrier(n + 1), // +1 here because we're counting the main thread
				m_ReleaseWorkers(false),
				m_Halt(false)
			{
				static_assert(n > 1 && "BOB [thread_pool][thread_pool()]: n must be larger than 1");

				for (size_t i = 0; i < n; i++)
					this->m_Workers.emplace_back(std::thread(TODO));
				
				this->m_Jobs.reserve(n);
			}

			~thread_pool()
			{

			}

			thread_pool(const thread_pool&) = delete;
			thread_pool(thread_pool&&) = delete;

			thread_pool& operator=(const thread_pool&) = delete;
			thread_pool& operator=(thread_pool&&) = delete;

			template <typename T, typename F>
			void parallelise(std::vector<T>& data, F&& callback, const size_t grain = 20000) noexcept
			{
				if (data.size() > grain)
				{
					void* data_ptr = static_cast<void*>(&data);
					void* lambda_ptr = static_cast<void*>(&callback);
					void (thread_pool::*)(const job_wrapper&) target_pmf = &thread_pool::m_ProcessChunk<T, F>;

					const size_t n_threads = this->m_Workers.size();
					// +1 bc we actually have 1 more thread, the one executing this function
					const uint32_t job_size = static_cast<uint32_t>(data.size()) / (n_threads + 1);

					for (size_t i = 0; i < n_threads; i++)
					{
						const uint32_t begin = job_size * i;
						const uint32_t end = begin + job_size;
						this->m_Jobs.emplace_back(data_ptr, lambda_ptr, target_pmf, begin, end);
					}

				}
				else
					m_ProcessSingle(data, std::forward<F>(callback), 0, data.size());
			}

		private:
			template <typename T, typename F>
			void m_ProcessChunk(const job_wrapper& job) const noexcept
			{
				std::vector<T>& data = *static_cast<std::vector<T>*>(job.data);
				F& callback = *static_cast<F*>(job.callback);
				
				assert(data.size() < begin && "BOB [thread_pool][m_ProcessChunk()]: begin is larger than vector size");
				assert(data.size() < end &&	"BOB [thread_pool][m_ProcessChunk()]: end is larger than vector size");

				for (size_t i = static_cast<size_t>(job.begin), n = static_cast<size_t>(job.end); i < n; i++)
					callback(data[i], i);
			}

			template <typename T, typename F>
			void m_ProcessSingle(std::vector<T>& data, F&& callback, const size_t begin, const size_t end) const noexcept
			{
				assert(data.size() < begin && "BOB [thread_pool][m_ProcessSingle()]: begin is larger than vector size");
				assert(data.size() < end &&	"BOB [thread_pool][m_ProcessSingle()]: end is larger than vector size");

				for (size_t i = begin; i < end; i++)
					callback(data[i], i);
			}

			void m_WorkerFunction(const uint32_t index) noexcept
			{
				thread_local uint32_t thread_id = index;

				while (true)
				{
					{
						unique_lock<std::mutex> lock(this->m_Mutex);
						this->m_ConditionVar.wait(lock, [this](){ return (this->m_ReleaseWorkers || this->m_Halt); });

						if (this->m_Halt)
							return;
					}

					const job_wrapper& current_job = this->m_Jobs[index];
					(this->*current_job.pmf)(current_job);

					this->m_Barrier.arrive_and_wait();
				}
			}

			std::condition_variable m_ConditionVar;
			std::mutex m_Mutex;
			std::barrier m_Barrier;
			std::vector<std::thread> m_Workers;
			std::vector<job_wrapper> m_Jobs;
			bool m_ReleaseWorkers;
			bool m_Halt;
	};
};
#endif
