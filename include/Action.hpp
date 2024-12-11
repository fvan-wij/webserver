#pragma once

class ActionBase {
	public:
		virtual void execute(short events) = 0;
		virtual ~ActionBase() = default;
		virtual void cleanup() = 0;
};

template <typename T>
class Action : public ActionBase {
	public:
		Action(T* obj, void (T::*method)(short)) : _obj(obj), _method(method) {}
		void execute(short events) override {
			(_obj->*_method)(events);
		}
		void cleanup() {
			if (_obj)
				delete _obj;
		}

	private:
		T* _obj;
		void (T::*_method)(short);
};
