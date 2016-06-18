#include "ModuleDebug.hpp"
#include "../ElDorito.hpp"
#include "../Patch.hpp"
#include "../Utils/Logger.hpp"

namespace
{
	void* Debug_MemcpyHook(void* dst, void* src, uint32_t len)
	{
		auto srcFilter = Modules::ModuleDebug::Instance().VarMemcpySrc->ValueInt;
		auto dstFilter = Modules::ModuleDebug::Instance().VarMemcpyDst->ValueInt;
		uint32_t srcAddress = (uint32_t)src;
		uint32_t dstAddress = (uint32_t)dst;

		if (srcFilter >= srcAddress && srcFilter < srcAddress + len)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Trace, "Memcpy Filter Triggered - Source Address: 0x%08X, Destination Address: 0x%08X, Size: 0x%08X, CallStack: %s",
				srcAddress, dstAddress, len, Utils::GetStackTraceString(1, 5).c_str());

			// TODO: proper reset
			Modules::ModuleDebug::Instance().VarMemcpySrc->ValueInt = 0;
		}

		if (dstFilter >= dstAddress && dstFilter < dstAddress + len)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Trace, "Memcpy Filter Triggered - Source Address: 0x%08X, Destination Address: 0x%08X, Size: 0x%08X, CallStack: %s",
				srcAddress, dstAddress, len, Utils::GetStackTraceString(1, 5).c_str());

			// TODO: proper reset
			Modules::ModuleDebug::Instance().VarMemcpyDst->ValueInt = 0;
		}

		return memcpy(dst, src, len);
	}

	bool MemcpySrcFilterUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto src = Modules::ModuleDebug::Instance().VarMemcpySrc->ValueInt;

		std::stringstream ss;
		ss << "Memcpy source address filter set to " << src;
		returnInfo = ss.str();

		return true;
	}

	bool MemcpyDstFilterUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto dst = Modules::ModuleDebug::Instance().VarMemcpyDst->ValueInt;

		std::stringstream ss;
		ss << "Memcpy destination address filter set to " << dst;
		returnInfo = ss.str();

		return true;
	}

	void* Debug_MemsetHook(void* dst, int val, uint32_t len)
	{
		uint32_t dstFilter = Modules::ModuleDebug::Instance().VarMemsetDst->ValueInt;
		uint32_t dstAddress = (uint32_t)dst;

		if (dstFilter >= dstAddress && dstFilter < dstAddress + len)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Trace, "Memset Filter Triggered - Destination Address: 0x%08X, Size: 0x%08X, CallStack: %s",
				dstAddress, len, Utils::GetStackTraceString(1, 5).c_str());

			// TODO: proper reset
			Modules::ModuleDebug::Instance().VarMemsetDst->ValueInt = 0;
		}

		return memset(dst, val, len);
	}

	bool MemsetDstFilterUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto dst = Modules::ModuleDebug::Instance().VarMemsetDst->ValueInt;

		std::stringstream ss;
		ss << "Memset destination address filter set to " << dst;
		returnInfo = ss.str();

		return true;
	}
}

namespace Modules
{
	ModuleDebug::ModuleDebug() : ModuleBase("Debug")
	{
		VarMemcpySrc = AddVariableInt("MemcpySrc", "memcpy_src", "Allows breakpointing memcpy based on specified source address filter.", eCommandFlagsHidden, 0, MemcpySrcFilterUpdate);
		VarMemcpyDst = AddVariableInt("MemcpyDst", "memcpy_dst", "Allows breakpointing memcpy based on specified destination address filter.", eCommandFlagsHidden, 0, MemcpyDstFilterUpdate);
		VarMemsetDst = AddVariableInt("MemsetDst", "memset_dst", "Allows breakpointing memset based on specified destination address filter.", eCommandFlagsHidden, 0, MemsetDstFilterUpdate);

		Hook(0x7EF260, Debug_MemcpyHook).Apply();
		Hook(0x7EF2E0, Debug_MemsetHook).Apply();
	}
}