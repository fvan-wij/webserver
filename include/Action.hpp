#pragma once

class ActionBase
{
	public:
		virtual void execute(short events) = 0;
		virtual ~ActionBase() = default;
};

template <typename T>
class Action : public ActionBase {
	public:
		Action(T* obj, void (T::*cb_method)(short)) : _obj(obj), _cb_method(cb_method)
		{

		}

		void execute(short events) override
		{
			(_obj->*_cb_method)(events);
		}

	private:
		T* _obj;
		void (T::*_cb_method)(short);
};
