#ifndef BOB_THREAD_POOL
#define BOB_THREAD_POOL

#include <vector>
#include <thread>
#include <cassert>
#include <cstddef>
#include <barrier>

namespace bob
{
	class thread_pool;
	// TODO: probably align this to avoid false sharing. that might be an issue. idk if vector already does.
	struct job_wrapper
	{
		void* data;
		void* lambda;
		void (thread_pool::*pmf)(const job_wrapper&) const noexcept;
		uint32_t begin;
		uint32_t end;
	};

	class thread_pool
	{
		public:
			thread_pool(const size_t n) :
				m_StartBarrier(n + 1), // +1 here because we're counting the main thread
				m_EndBarrier(n + 1),
				m_Halt(false)
			{
				assert(n > 1 && "BOB [thread_pool][thread_pool()]: n must be larger than 1");

				for (size_t i = 0; i < n; i++)
				{
					this->m_Workers.emplace_back(
							std::thread(
								[this, i](){ this->m_WorkerFunction(static_cast<uint32_t>(i)); }
								)
							);
				}

				this->m_Jobs.resize(n);
			}

			~thread_pool()
			{
				this->m_Halt = true;
				this->m_StartBarrier.arrive_and_wait();

				for (size_t i = 0, n = this->m_Workers.size(); i < n; i++)
					this->m_Workers[i].join();
			}

			thread_pool(const thread_pool&) = delete;
			thread_pool(thread_pool&&) = delete;

			thread_pool& operator=(const thread_pool&) = delete;
			thread_pool& operator=(thread_pool&&) = delete;

			template <typename T, typename F>
			void parallelise(std::vector<T>& data, F&& callback, const size_t grain = 20000) noexcept
			{
				if (data.size() < grain)
				{
					m_ProcessSingle(data, std::forward<F>(callback), 0, data.size());
					return;
				}

				void* data_ptr = static_cast<void*>(&data);
				void* lambda_ptr = static_cast<void*>(&callback);
				void (thread_pool::*target_pmf)(const job_wrapper&) const noexcept = &thread_pool::m_ProcessChunk<T, F>;

				const size_t n_threads = this->m_Workers.size();
				// +1 bc we actually have 1 more thread, the one executing this function
				const uint32_t job_size = static_cast<uint32_t>(data.size()) / (n_threads + 1);
				uint32_t last_job_end = 0;

				// this is a bitch. order matters. put it at the end and it'll crash.
				// tldr, don't touch this
				this->m_Jobs.clear(); 

				for (size_t i = 0; i < n_threads; i++)
				{
					const uint32_t begin = job_size * i;
					const uint32_t end = begin + job_size;
					last_job_end = end;
					this->m_Jobs.emplace_back(data_ptr, lambda_ptr, target_pmf, begin, end);
				}

				this->m_StartBarrier.arrive_and_wait();
				this->m_ProcessSingle(data, callback, last_job_end, data.size());
				this->m_EndBarrier.arrive_and_wait();
			}

		private:
			template <typename T, typename F>
			void m_ProcessChunk(const job_wrapper& job) const noexcept
			{
				std::vector<T>& data = *static_cast<std::vector<T>*>(job.data);
				F& callback = *static_cast<F*>(job.lambda);
				
				assert(job.begin < data.size() && "BOB [thread_pool][m_ProcessChunk()]: begin is larger than vector size");
				assert(job.end < data.size() && "BOB [thread_pool][m_ProcessChunk()]: end is larger than vector size");

				for (size_t i = static_cast<size_t>(job.begin), n = static_cast<size_t>(job.end); i < n; i++)
					callback(data[i], i);
			}

			template <typename T, typename F>
			void m_ProcessSingle(std::vector<T>& data, F&& callback, const size_t begin, const size_t end) const noexcept
			{
				assert(begin < data.size() && "BOB [thread_pool][m_ProcessSingle()]: begin is larger than vector size");
				assert(end == data.size() && "BOB [thread_pool][m_ProcessSingle()]: end does not reach the actual end");

				for (size_t i = begin; i < end; i++)
					callback(data[i], i);
			}

			void m_WorkerFunction(const uint32_t index) noexcept
			{
				thread_local uint32_t thread_id = index;

				while (true)
				{
					this->m_StartBarrier.arrive_and_wait();

					if (this->m_Halt)
						return;

					const job_wrapper& current_job = this->m_Jobs[thread_id];
					(this->*current_job.pmf)(current_job);

					this->m_EndBarrier.arrive_and_wait();
				}
			}

			std::barrier<> m_StartBarrier;
			std::barrier<> m_EndBarrier;
			std::vector<std::thread> m_Workers;
			std::vector<job_wrapper> m_Jobs;
			bool m_Halt;
	};
};
#endif
