#pragma once

class ActionBase {
	public:
		virtual void execute() = 0;
		virtual ~ActionBase() = default;
};

template <typename T>
class Action : public ActionBase {
	public:
		Action(T* obj, void (T::*method)()) : obj_(obj), method_(method) {}
		void execute() override {
			(obj_->*method_)();
		}

	private:
		T* obj_;
		void (T::*method_)();
};