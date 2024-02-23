#pragma once

#include "OverrideInterface.h"

namespace OM
{

	typedef void (*AddNodeOverrideIntFunc)(RE::StaticFunctionTag*, RE::TESObjectREFR*, bool, std::string, int, int, int, bool);
	typedef void (*AddNodeOverrideFloatFunc)(RE::StaticFunctionTag*, RE::TESObjectREFR*, bool, std::string, int, int, float, bool);
	typedef void (*AddNodeOverrideStringFunc)(RE::StaticFunctionTag*, RE::TESObjectREFR*, bool, std::string, int, int, const char*, bool);

	typedef int (*GetNumOverlaysFunc)(RE::StaticFunctionTag*);

	typedef int (*GetNodeOverrideIntFunc)(RE::StaticFunctionTag*, RE::TESObjectREFR*, bool, std::string, int, int);
	typedef float (*GetNodeOverrideFloatFunc)(RE::StaticFunctionTag*, RE::TESObjectREFR*, bool, std::string, int, int);
	typedef RE::BSFixedString (*GetNodeOverrideStringFunc)(RE::StaticFunctionTag*, RE::TESObjectREFR*, bool, std::string, int, int);

	typedef bool (*HasNodeOverrideFunc)(RE::StaticFunctionTag*, RE::TESObjectREFR*, bool, std::string, int, int);

	typedef void (*RemoveNodeOverrideFunc)(RE::StaticFunctionTag*, RE::TESObjectREFR*, bool, std::string, int, int);

	typedef void (*ApplyNodeOverridesFunc)(RE::StaticFunctionTag*, RE::TESObjectREFR*);
	typedef void (*AddOverlaysFunc)(RE::StaticFunctionTag*, RE::TESObjectREFR*);

	class LegacyOverrideInterface : public OverrideInterface
	{
	public:
		static inline OverrideInterface* GetSingleton()
		{
			static LegacyOverrideInterface instance;

			static std::atomic_bool initialized;
			static std::latch latch(1);

			if (!initialized.exchange(true)) {
				// yes, this is terrible but there isn't a better option for 1.5/1.6.640 support

				HMODULE baseAddress = GetModuleHandleA("skee64.dll");

				if (baseAddress) {
					logger::info("initializing NiOverride API");
					char* func_start;

					// TODO: swap address lists depending on version

					auto addresses = instance._addresses640;
					
					const auto ver = REL::Module::get().version();
					if (ver.major() == 1 && ver.minor() == 5 && ver.patch() == 97) {
						addresses = instance._addresses97;
					}

					func_start = (char*)baseAddress + addresses[0];
					instance._GetNumBodyOverlays = (GetNumOverlaysFunc)func_start;

					func_start = (char*)baseAddress + addresses[1];
					instance._GetNumHandOverlays = (GetNumOverlaysFunc)func_start;

					func_start = (char*)baseAddress + addresses[2];
					instance._GetNumFeetOverlays = (GetNumOverlaysFunc)func_start;

					func_start = (char*)baseAddress + addresses[3];
					instance._GetNumFaceOverlays = (GetNumOverlaysFunc)func_start;

					func_start = (char*)baseAddress + addresses[4];
					instance._GetNodeOverrideInt = (GetNodeOverrideIntFunc)func_start;

					func_start = (char*)baseAddress + addresses[5];
					instance._GetNodeOverrideFloat = (GetNodeOverrideFloatFunc)func_start;

					func_start = (char*)baseAddress + addresses[6];
					instance._GetNodeOverrideString = (GetNodeOverrideStringFunc)func_start;

					func_start = (char*)baseAddress + addresses[7];
					instance._AddNodeOverrideInt = (AddNodeOverrideIntFunc)func_start;

					func_start = (char*)baseAddress + addresses[8];
					instance._AddNodeOverrideFloat = (AddNodeOverrideFloatFunc)func_start;

					func_start = (char*)baseAddress + addresses[9];
					instance._AddNodeOverrideString = (AddNodeOverrideStringFunc)func_start;

					func_start = (char*)baseAddress + addresses[10];
					instance._HasNodeOverride = (HasNodeOverrideFunc)func_start;

					func_start = (char*)baseAddress + addresses[11];
					instance._RemoveNodeOverride = (RemoveNodeOverrideFunc)func_start;

					func_start = (char*)baseAddress + addresses[12];
					instance._ApplyNodeOverrides = (ApplyNodeOverridesFunc)func_start;

					func_start = (char*)baseAddress + addresses[13];
					instance._AddOverlays = (AddOverlaysFunc)func_start;
				} else {
					logger::error("failed to find SKEE base address");
				}

				latch.count_down();
			}

			latch.wait();

			return &instance;
		}

		inline int GetNumBodyOverlays() override
		{
			return _GetNumBodyOverlays(&_base);
		}

		inline int GetNumHandOverlays() override
		{
			return _GetNumHandOverlays(&_base);
		}

		inline int GetNumFeetOverlays() override
		{
			return _GetNumFeetOverlays(&_base);
		}

		inline int GetNumFaceOverlays() override
		{
			return _GetNumFaceOverlays(&_base);
		}

		inline int GetNodeOverrideInt(RE::TESObjectREFR* a_ref, bool a_female, std::string a_node, int a_key, int a_index) override
		{
			return _GetNodeOverrideInt(&_base, a_ref, a_female, a_node, a_key, a_index);
		}

		inline float GetNodeOverrideFloat(RE::TESObjectREFR* a_ref, bool a_female, std::string a_node, int a_key, int a_index) override
		{
			return _GetNodeOverrideFloat(&_base, a_ref, a_female, a_node, a_key, a_index);
		}

		inline std::string GetNodeOverrideString(RE::TESObjectREFR* a_ref, bool a_female, std::string a_node, int a_key, int a_index) override
		{
			return std::string{ _GetNodeOverrideString(&_base, a_ref, a_female, a_node, a_key, a_index).c_str() };
		}

		inline void AddNodeOverrideInt(RE::TESObjectREFR* a_ref, bool a_female, std::string a_node, int a_key, int a_index, int a_value, bool a_persist) override
		{
			return _AddNodeOverrideInt(&_base, a_ref, a_female, a_node, a_key, a_index, a_value, a_persist);
		}

		inline void AddNodeOverrideFloat(RE::TESObjectREFR* a_ref, bool a_female, std::string a_node, int a_key, int a_index, float a_value, bool a_persist) override
		{
			return _AddNodeOverrideFloat(&_base, a_ref, a_female, a_node, a_key, a_index, a_value, a_persist);
		}

		inline void AddNodeOverrideString(RE::TESObjectREFR* a_ref, bool a_female, std::string a_node, int a_key, int a_index, std::string a_value, bool a_persist) override
		{
			return _AddNodeOverrideString(&_base, a_ref, a_female, a_node, a_key, a_index, a_value.c_str(), a_persist);
		}

		inline bool HasNodeOverride(RE::TESObjectREFR* a_ref, bool a_female, std::string a_node, int a_key, int a_index) override
		{
			return _HasNodeOverride(&_base, a_ref, a_female, a_node, a_key, a_index);
		}

		inline void RemoveNodeOverride(RE::TESObjectREFR* a_ref, bool a_female, std::string a_node, int a_key, int a_index) override
		{
			return _RemoveNodeOverride(&_base, a_ref, a_female, a_node, a_key, a_index);
		}

		inline void AddOverlays(RE::TESObjectREFR* a_ref) override
		{
			return _AddOverlays(&_base, a_ref);
		}

		inline void ApplyNodeOverrides(RE::Actor* a_actor) override
		{
			return _ApplyNodeOverrides(&_base, a_actor);
		}

		GetNumOverlaysFunc _GetNumBodyOverlays;
		GetNumOverlaysFunc _GetNumHandOverlays;
		GetNumOverlaysFunc _GetNumFeetOverlays;
		GetNumOverlaysFunc _GetNumFaceOverlays;

		GetNodeOverrideIntFunc _GetNodeOverrideInt;
		GetNodeOverrideFloatFunc _GetNodeOverrideFloat;
		GetNodeOverrideStringFunc _GetNodeOverrideString;

		AddNodeOverrideIntFunc _AddNodeOverrideInt;
		AddNodeOverrideFloatFunc _AddNodeOverrideFloat;
		AddNodeOverrideStringFunc _AddNodeOverrideString;

		HasNodeOverrideFunc _HasNodeOverride;

		RemoveNodeOverrideFunc _RemoveNodeOverride;

		ApplyNodeOverridesFunc _ApplyNodeOverrides;
		AddOverlaysFunc _AddOverlays;

		std::vector<int> _addresses97{}; // TODO: fill in 1.5 addresses
		std::vector<int> _addresses640{
			0xA5EF0,
			0xA5F00,
			0xA5F10,
			0xA5F20,
			0xCA230,
			0xCA160,
			0xCA3C0,
			0xC9AD0,
			0xC9860,
			0xC9D20,
			0xA5940,
			0xA5CA0,
			0xA5930,
			0xA5350
		};

		RE::StaticFunctionTag _base;
	};
}