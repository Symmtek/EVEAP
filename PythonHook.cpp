#include "stdafx.h"
#include "PythonHook.h"
#include "constexpr\cx_strenc.h"

constexpr auto eve_str_cmdwarptostuffautopilot = cx_make_encrypted_string("cmdwarptostuffautopilot");
constexpr auto eve_str_mainmodule = cx_make_encrypted_string("eve");
constexpr auto eve_str_client = cx_make_encrypted_string("client");
constexpr auto eve_str_script = cx_make_encrypted_string("script");
constexpr auto eve_str_ui = cx_make_encrypted_string("ui");
constexpr auto eve_str_services = cx_make_encrypted_string("services");
constexpr auto eve_str_menuSvcExtras = cx_make_encrypted_string("menuSvcExtras");
constexpr auto eve_str_movementFunctions = cx_make_encrypted_string("movementFunctions");
constexpr auto eve_str___builtin__ = cx_make_encrypted_string("__builtin__");
constexpr auto eve_str_uicore = cx_make_encrypted_string("uicore");
constexpr auto eve_str_uilib = cx_make_encrypted_string("uilib");
constexpr auto eve_str_const = cx_make_encrypted_string("const");
constexpr auto eve_str_minWarpEndDistance = cx_make_encrypted_string("minWarpEndDistance");
constexpr auto eve_str_WarpToItem = cx_make_encrypted_string("WarpToItem");
constexpr auto eve_str_RegisterAppEventTime = cx_make_encrypted_string("RegisterAppEventTime");

auto OriginalPyEval_CallObjectWithKeywords = &PyEval_CallObjectWithKeywords;

auto set = false;

std::string recoverException() {
	PyObject * extype, *value, *traceback;
	PyErr_Fetch(&extype, &value, &traceback);
	if (!extype) return "";

	Py::Object o_extype{ extype, true };
	Py::Object o_value{ value, true };
	Py::Object o_traceback{ traceback, true };

	auto lines = Py::Module("traceback").callMemberFunction("format_exception", Py::TupleN{ o_extype, o_value, o_traceback });

	return lines.as_string();
}

int tracer_func(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg) {
	if (what == PyTrace_LINE) {
		OutputDebugStringA("tracer_func");
		if (frame == nullptr) {
			OutputDebugStringA("frame == nullptr");
		}
		else {
			if (frame->f_code == nullptr) OutputDebugStringA("frame->f_code == nullptr");
			else {
				if (frame->f_code->co_filename == nullptr) OutputDebugStringA("frame->f_code->co_name == nullptr");
				else {
					PyObject *str;
					str = PyObject_Str(frame->f_code->co_filename);
					if (str) {
						OutputDebugStringA("Yes");
						Py_DECREF(str);
					}
					else {
						OutputDebugStringA("None");
					}
				}
			}
			if (frame->f_lasti == 0) OutputDebugStringA("frame->f_lasti == 0");
		}
	}
	return 0;
}

std::string getStack() {
	return Py::Module("traceback").callMemberFunction("format_stack").repr().as_string();
}

std::string extractFullName(const Py::Object& o) {
	return o.getAttr("__module__").repr().as_string() + o.getAttr("__class__").getAttr("__name__").repr().as_string();
}

std::string toLower(const std::string& str)
{
	std::string local_string{ str };
	std::transform(local_string.begin(), local_string.end(), local_string.begin(), ::tolower);
	return local_string;
}

EVEScript::EVEScript() :
	isRecursiveCall(false),
	autoPilotDetected(false)
{

}

EVEScript::~EVEScript()
{
}

PyObject* EVEScript::onCall(PyObject* obj, PyObject* arg, PyObject* kw)
{
	static auto isExceptionOccured = false;

	try
	{
		if (!isExceptionOccured && !isRecursiveCall && obj != nullptr && arg != nullptr) {
			isRecursiveCall = true;
			const Py::Callable function{ obj };
			const Py::Tuple functionArgs{ arg };

			if (functionArgs.isTuple()) {
				if (functionArgs.size() == 2 &&
					functionArgs[1].isString() &&
					toLower(functionArgs[1].repr().as_string()).find(eve_str_cmdwarptostuffautopilot) != std::string::npos)
				{
					OutputDebugString(L"Found one");
					OutputDebugStringA(("Func" + function.as_string()).c_str());
					OutputDebugStringA(("Args" + functionArgs.as_string()).c_str());
					OutputDebugStringA(("Stacktrace" + getStack()).c_str());
					OutputDebugStringA(("Fullname" + extractFullName(function)).c_str());
					autoPilotDetected = true;
					if (!set) {
						PyEval_SetTrace(&tracer_func, nullptr);
						set = true;
					}
				}
				else if (autoPilotDetected &&
					functionArgs.size() == 1 &&
					Py::_Long_Check(functionArgs[0].ptr()) //&&
					//toLower(function.as_string()).find("MachoObjectCallWrapper") != std::string::npos
					)
				{
					if (set) {
						PyEval_SetTrace(nullptr, nullptr);
						set = false;
					}
					OutputDebugString(L"Found two");
					OutputDebugStringA(("Func" + function.as_string()).c_str());
					OutputDebugStringA(("Args" + functionArgs.as_string()).c_str());
					OutputDebugStringA(("KWArgs" + Py::Object(kw).as_string()).c_str());
					OutputDebugStringA(("Stacktrace" + getStack()).c_str());
					OutputDebugStringA(("Fullname" + extractFullName(function)).c_str());
					//isRecursiveCall = false;
					//return nullptr;
					autoPilotDetected = false;
					const auto destination = functionArgs[0];

					const auto movementFunctions = Py::Module{ eve_str_mainmodule }
						.getAttr(eve_str_client)
						.getAttr(eve_str_script)
						.getAttr(eve_str_ui)
						.getAttr(eve_str_services)
						.getAttr(eve_str_menuSvcExtras)
						.getAttr(eve_str_movementFunctions);

					const auto uilib = Py::Module{ eve_str___builtin__ }
						.getAttr(eve_str_uicore)
						.getAttr(eve_str_uilib);

					const auto minWarpEndDistance = Py::Module{ eve_str___builtin__ }
						.getAttr(eve_str_const)
						.getAttr(eve_str_minWarpEndDistance);

					const auto warpToItem = Py::Callable{ movementFunctions.getAttr(eve_str_WarpToItem) };
					const auto registerAppEventTime = Py::Callable{ uilib.getAttr(eve_str_RegisterAppEventTime) };

					registerAppEventTime.apply(Py::TupleN{});

					const Py::TupleN valueTuple{ destination, minWarpEndDistance, Py::False() };
					warpToItem.apply(valueTuple);
					isRecursiveCall = false;

					OutputDebugString(L"Done");
					return Py::new_reference_to(Py::None());
				}
			}
			isRecursiveCall = false;
		}
	}
	catch (Py::Exception &exc)
	{
		OutputDebugString(L"Exception");
		OutputDebugStringA(recoverException().c_str());
		isExceptionOccured = true;
		exc.clear();
	}
	catch (std::exception&)
	{
		isExceptionOccured = true;
	}
	return nullptr;
}

PyObject* EVEScript::PyEval_CallObjectWithKeywordsHook(PyObject* obj, PyObject* arg, PyObject* kw)
{
	static auto EVEScriptInstance = std::make_unique<EVEScript>();

	const auto result = EVEScriptInstance->onCall(obj, arg, kw);
	if (result == nullptr)
		return OriginalPyEval_CallObjectWithKeywords(obj, arg, kw);
	else
		return result;
}

int MainFunction()
{
	using HookType = std::unique_ptr<PVOID, void(*)(PVOID*)>;

	try
	{
		if (!Mhook_SetHook(reinterpret_cast<PVOID*>(&OriginalPyEval_CallObjectWithKeywords),
			EVEScript::PyEval_CallObjectWithKeywordsHook
		))
		{
			throw std::runtime_error("Can't set hook");
		}

		MessageBoxA(0, "Close", "Close", MB_OK);

		auto hookInstance = HookType{
			reinterpret_cast<PVOID*>(&OriginalPyEval_CallObjectWithKeywords),
			[](PVOID* hook)
			{
				using namespace std::chrono_literals;
				Mhook_Unhook(hook);
				std::this_thread::sleep_for(1s);
			}
		};

		return 0;
	}
	catch (std::exception&)
	{

	}

	return 0;
}


DWORD __stdcall EntryFunc(void* dllHandle)
{
	auto result = MainFunction();
	//return result;
	FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(dllHandle), result);
}
