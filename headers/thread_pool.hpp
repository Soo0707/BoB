#ifndef BOB_THREAD_POOL
#define BOB_THREAD_POOL

#include <vector>
#include <thread>
#include <cassert>
#include <cstddef>

namespace bob
{
	class thread_pool
	{
		public:
			thread_pool(const size_t n)
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
			void parallelise(std::vector<T>& data, F&& callback) noexcept
			{

			}

		private:
			template <typename T, typename F>
			void m_ProcessChunk(std::vector<T>& data, const size_t begin, const size_t end, F&& callback) const noexcept
			{
				assert(data.size() < begin && "BOB [thread_pool][m_ProcessChunk()]: begin is larger than vector size");
				assert(data.size() < end &&	"BOB [thread_pool][m_ProcessChunk()]: end is larger than vector size");

				for (size_t i = begin; i < end; i++)
					callback(data[i], i);
			}

			std::vector<std::thread> m_Workers;
			// TODO: the idea is to have a job struct that each thread looks into their slot
			// for. the struct will contain the info for process chunk.
			std::vector<job> m_Jobs;
	};
};
#endif
