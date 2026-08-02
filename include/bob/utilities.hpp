#ifndef BOB_UTILITIES
#define BOB_UTILITIES

#include "bob/entity_handle.hpp"

namespace bob
{
	template <typename T, void(T::*F)() noexcept>
	void execute(void* context)
	{
		T* concrete_context = static_cast<T*>(context);
		(concrete_context->*F)();
	}

	template <typename T, void(T::*F)(const bob::entity_handle) noexcept>
	void execute(void* context, const bob::entity_handle handle)
	{
		T* concrete_context = static_cast<T*>(context);
		(concrete_context->*F)(handle);
	}

	struct add_proxy
	{
		void* context = nullptr;
		void(*callback)(void* context, const bob::entity_handle) = nullptr;

		void operator()(const bob::entity_handle handle) noexcept
		{
			this->callback(this->context, handle);
		}
	};

	struct remove_proxy
	{
		void* context = nullptr;
		void(*callback)(void* context) = nullptr;

		void operator()() noexcept
		{
			this->callback(this->context);
		}
	};
};
#endif
