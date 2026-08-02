#ifndef BOB_UTILITIES
#define BOB_UTILITIES

#include "bob/entity_handle.hpp"

namespace bob
{
	struct add_callback
	{
		void* context;
		void(*proxy)(void* context, const bob::entity_handle);
	};

	template <typename T, void(T::*F)(const bob::entity_handle)>
	void add_callback_proxy(void* context, const bob::entity_handle handle)
	{
		T* concrete_context = static_cast<T*>(context);
		(concrete_context->*F)(handle);
	}

	struct remove_callback
	{
		void* context;
		void(*proxy)(void* context, const bob::entity_handle);
	};

	template <typename T, void(T::*F)()>
	void remove_callback_proxy(void* context)
	{
		T* concrete_context = static_cast<T*>(context);
		(concrete_context->*F)();
	}
};
#endif
