/*
	// global function :

	MeasureFunctionTimeElapsed([&](auto d) {
		auto du = chrono::duration_cast<chrono::nanoseconds>(d).count();
		Event_Type event(string("event identifier"), du);
		CLogSystem::PropagateNotification(nullptr, &event);	// ������ �Ѱ������ object�� ���� ���

	}
	, &function, function arguments...);



	// class member function :

	MeasureFunctionTimeElapsed([&](auto d) {
	auto du = chrono::duration_cast<chrono::nanoseconds>(d).count();
	Event_Type event(string("event identifier"), du);
	CLogSystem::PropagateNotification(nullptr, &event);	// ������ �Ѱ������ object�� ���� ���

	}
	, &class::function, this, member function arguments...);
*/

#pragma once

#define use_Measure_Time_Elapsed true

struct function_invoke_return_late_for_measure_time_elapsed
{	
	template<
		  class _Callable
		, class _OutputFn
		, class... _Types>
		static auto _Call(_Callable&& _Obj, _OutputFn&& _Out, _Types&&... _Args)
	{	
		auto startTime = std::chrono::high_resolution_clock::now();

		auto retval = _STD invoke(
			  _STD forward<_Callable>(_Obj)
			, _STD forward<_Types>(_Args)...
		);

		auto du = std::chrono::high_resolution_clock::now() - (startTime);

		_STD invoke(_STD forward<_OutputFn>(_Out), du);

		return retval;
	}
};

struct function_invoke_no_return_for_measure_time_elapsed
{	
	template<
		  class _Callable
		, class _OutputFn
		, class... _Types>
		static auto _Call(_Callable&& _Obj, _OutputFn&& _Out, _Types&&... _Args)
	{	
		auto startTime = std::chrono::high_resolution_clock::now();

		_STD invoke(
			  _STD forward<_Callable>(_Obj)
			, _STD forward<_Types>(_Args)...
		);

		auto du = std::chrono::high_resolution_clock::now() - (startTime);

		_STD invoke(_STD forward<_OutputFn>(_Out), du);
	}
};

template<
	  typename returnType
	, bool _Is_void = is_void<returnType>::value
> 
struct InvokerMeasureFunc;

template<typename Ty>
struct InvokerMeasureFunc<Ty, true>
	: function_invoke_no_return_for_measure_time_elapsed
{
};

template<typename Ty>
struct InvokerMeasureFunc<Ty, false>
	: function_invoke_return_late_for_measure_time_elapsed
{
};

/// <summary>
/// <para>�Լ��� ���� �ð��� �����ϴ� �Լ��Դϴ�. </para>
/// <para>use_Measure_Time_Elapsed ��ó����� ����� ������� ���� �� �ֽ��ϴ�. </para>
/// <para>�ð��� �����ϴ� �Լ��� ���ڷ� duration��long long, std::nano�� Ÿ���� ���ڸ� �޽��ϴ�. </para>
/// </summary>
///	<param name = "outputFunc"> �ð��� �����ϴ� �Լ��Դϴ�. �ð��� �˰� �ʹٸ� ���ϴ� �ð� �����ڸ� ����ϼ���.  </param>
///	<param name = "Function"> ���� �Լ��Դϴ�. ��ȯ���� ���� ���, �ش� �Լ��� ��ȯ������ ��ȯ�մϴ�. </param>
///	<param name = "args"> ���� �Լ��� �����Դϴ�. </param>
/// <remarks> 
///	<para>�ð��� �����ϴ� �Լ��� ���ڸ� duration �ϳ����� ������ �Լ��Դϴ�. 	  </para>
///	<para>���� �Լ��� ��ȯ���� void�� ��� ��ȯ���� �������� �õ��� ���� ���ʽÿ�. </para>
///	<para>�����ε�� �Լ��� ���, �Լ� �������� Ÿ���� ��üȭ���Ѿ� �մϴ�. </para>
///	<para>���� ���, ������ int(*)(int) �� void(*)(std::string) �� �����ε�� �Լ��� ���� ���, </para>
///	<para>static_cast �� ����Ͽ� �Լ��� Ÿ���� Ȯ������� �մϴ�. </para>
///	<para>���ŷο� ���, MeasureBlockTimeElapsed �Լ��� ����Ͻʽÿ�. </para>
///	</remarks>
/// <returns> ���� �Լ��� ��ȯ���� �ִٸ� �װ��� ��ȯ�մϴ�. ���� �Լ��� ��ȯ���� ���� ��� ������ ������ �߻��մϴ�. </returns>
template<typename FnOutput, typename FnFunc, typename... Args> inline
constexpr auto MeasureFunctionTimeElapsed(FnOutput&& outputFunc, FnFunc&& Function, Args&&... args) noexcept
{
	
#if use_Measure_Time_Elapsed
	return (InvokerMeasureFunc<decltype(_STD invoke(_STD forward<FnFunc>(Function), _STD forward<Args>(args)...) )>::_Call(
		_STD forward<FnFunc>(Function), _STD forward<FnOutput>(outputFunc), _STD forward<Args>(args)...)
		);
#else
	return (_STD forward<FnFunc>(Function)(_STD forward<Args>(args)...));
#endif
}

/// <summary>
/// <para>���ٷ� ���� ���� ���� �ð��� �����ϴ� �Լ��Դϴ�. </para>
/// <para>use_Measure_Time_Elapsed ��ó����� ����� ������� ���� �� �ֽ��ϴ�. </para>
/// <para>�ð��� �����ϴ� �Լ��� ���ڷ� duration��long long, std::nano�� Ÿ���� ���ڸ� �޽��ϴ�. </para>
/// </summary>
///	<param name = "outputFunc"> �ð��� �����ϴ� �Լ��Դϴ�. �ð��� �˰� �ʹٸ� ���ϴ� �ð� �����ڸ� ����ϼ���.  </param>
///	<param name = "Function"> ���� �����ϴ� ���� ���ٷ� ���� �����Դϴ�. ��ȯ���� �������� �ʽ��ϴ�. </param>
/// <remarks> 
///	<para>�ð��� �����ϴ� �Լ��� ���ڸ� duration �ϳ����� ������ �Լ��Դϴ�. </para>
///	<para><param>Function</param> ���ڴ� ���ٸ� ����Ͽ� �����ϵ��� �ǵ��߽��ϴ�. </para>
///	<para>������ ����� ��ȯ�ؾ� �� ���, ���� ĸ�� ����� ����ϼ���. </para>
///	<para>��ȯ���� �������� �õ��� ���� ���ʽÿ�. </para>
///	</remarks>
/// <returns> ��ȯ���� �����ϴ�. </returns>
template<typename FnOutput, typename FnFunc> inline
void MeasureBlockTimeElapsed(FnOutput&& outputFunc, FnFunc&& Function) noexcept
{
#if use_Measure_Time_Elapsed

	auto startTime = std::chrono::high_resolution_clock::now();

	_STD forward<FnFunc>(Function)();

	auto du = std::chrono::high_resolution_clock::now() - (startTime);

	_STD invoke(_STD forward<FnOutput>(outputFunc), du);

#else
	_STD forward<FnFunc>(Function)();
#endif
}
