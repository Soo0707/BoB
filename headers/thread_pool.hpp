#ifndef BOB_THREAD_POOL
#define BOB_THREAD_POOL

#include <vector>
#include <thread>
#include <atomic>
#include <cassert>
#include <cstddef>

namespace bob
{
	class thread_pool;
	struct job
	{
		uint32_t start;
		uint32_t end;
		void (thread_pool::*pmf)(void*, void*, const size_t, const size_t) const noexcept;
	};

	class thread_pool
	{
		public:
			thread_pool(const size_t n) :
				m_Halt(false)
			{
				static_assert(n > 1 && "BOB [thread_pool][thread_pool()]: n must be larger than 1");

				for (size_t i = 0; i < n; i++)
					this->m_Workers.emplace_back(std::thread(TODO));
			}

			~thread_pool()
			{

			}

			thread_pool(const thread_pool&) = delete;
			thread_pool(thread_pool&&) = delete;

			thread_pool& operator=(const thread_pool&) = delete;
			thread_pool& operator=(thread_pool&&) = delete;

			template <typename T, typename F>
			void parallelise(std::vector<T>& data, F&& callback, const size_t grain = 15000) noexcept
			{
				if (data.size() > grain)
					;
				else
					m_ProcessChunk(data, std::forward<F>(callback));
			}

		private:
			template <typename T, typename F>
			void m_ProcessChunk(void* abstract_data, void* abstract_callback, const size_t begin, const size_t end) const noexcept
			{
				std::vector<T>& data = *static_cast<std::vector<T>*>(abstract_data);
				F* callback = static_cast<F*>(abstract_callback);
				
				assert(data.size() < begin && "BOB [thread_pool][m_ProcessChunk()]: begin is larger than vector size");
				assert(data.size() < end &&	"BOB [thread_pool][m_ProcessChunk()]: end is larger than vector size");

				for (size_t i = begin; i < end; i++)
					callback(data[i], i);
			}

			template <typename T, typename F>
			void m_ProcessSingle(std::vector<T>& data, F&& callback) const noexcept
			{
				for (size_t i = 0, n = data.size(); i < n; i++)
					callback(data[i], i);
			}

			void m_WorkerFunction(const uint32_t index) noexcept
			{
				thread_local const uint32_t thread_id = index;

				while (!pool.m_Halt)
				{

				}
			}

			std::vector<std::thread> m_Workers;
			// TODO: the idea is to have a job struct that each thread looks into their slot
			// for. the struct will contain the info for process chunk.
			std::vector<job> m_Jobs;
			std::atomic<bool> m_Halt;
	};
};
#endif
