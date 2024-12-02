#pragma once

template <typename Derived>
class Renderer
{
public:
	void Initialize()
	{
		static_cast<Derived*>(this)->InitializeImpl();
	}

	void Render()
	{
		static_cast<Derived*>(this)->RenderImpl();
	}

	void Shutdown()
	{
		static_cast<Derived*>(this)->ShutdownImpl();
	}
};