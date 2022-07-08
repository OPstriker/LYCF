#pragma once

#include <ovpncli.hpp>

#include "LYCF.h"

namespace v0iden
{
	class v0iden
	{
	public:
		static void v_connect();
		static void v_disconnect();

		static std::string prf_path;
	};
}


