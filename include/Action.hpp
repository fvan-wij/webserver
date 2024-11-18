#pragma once

class ActionBase {
	public:
		virtual void execute(short events) = 0;
		virtual ~ActionBase() = default;
};

template <typename T>
class Action : public ActionBase {
	public:
		Action(T* obj, void (T::*method)(short)) : obj_(obj), method_(method) {}
		void execute(short events) override {
			(obj_->*method_)(events);
		}

	private:
		T* obj_;
		void (T::*method_)(short);
};
