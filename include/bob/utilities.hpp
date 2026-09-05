#ifndef BOB_UTILITIES
#define BOB_UTILITIES

#include "bob/entity.hpp"

namespace bob
{
	template <typename T, void(T::*F)(const entity) noexcept>
	void execute(void* context, const entity handle) noexcept
	{
		T* concrete_context = static_cast<T*>(context);
		(concrete_context->*F)(handle);
	}

	struct proxy
	{
		void* context = nullptr;
		void(*callback)(void* context, const entity) noexcept = nullptr;

		void operator()(const entity handle) noexcept
		{
			this->callback(this->context, handle);
		}
	};
};
#endif
