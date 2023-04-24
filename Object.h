#pragma once
#include <string>
#include"Container.h"
#include <format>

class Object{
public:
	inline static int numberInstances = 0;
	virtual std::string toString() const = 0;

	Object(){ ++numberInstances; }
	virtual ~Object(){ --numberInstances; }
};

class Task: public virtual Object{
public:
	virtual void run() = 0;
	bool isCompleted = false;
	virtual bool hasResult() const{ return false; }
};

template< typename Result_t>
class TaskWithResult: public Task{
public:
	bool hasResult() const override{ return true; }
protected:
	Result_t result_ {};
};

class Named: public virtual Object{
public:
	Named( std::string const& name ): name_( name ){}
	std::string toString() const override{ return name_; }
protected:
	std::string name_;
};

class BinaryOperatorTask: public TaskWithResult<double>, public Named{
	double left_;
	double right_;
	char operation_;

public:
	BinaryOperatorTask( std::string const& name, double left, double right, char  operation ):
		Named( name ),
		left_( left ),
		right_( right ),
		operation_( operation ){
	}

	void run() override{
		if ( isCompleted ) throw "The task has already been completed";
		switch ( operation_ )
		{
		case '+':
			result_ = left_ + right_;
			break;
		case '-':
			result_ = left_ - right_;
			break;
		case '/':
			result_ = left_ / right_;
			break;
		case '*':
			result_ = left_ * right_;
			break;
		default:
			throw "Bad operation";
		}
		isCompleted = true;
	}

	std::string toString() const override{
		return name_ + " : " + std::format( "{} {} {} = {} ({})",
											left_, operation_, right_, isCompleted ? std::to_string( result_ ) : "?", isCompleted ? "DONE" : "IN PROCESS" );
	}
};

class AddTask: public Task, public Named{
	Container<Task*>& container_;
	Task* const task_;

public:
	AddTask( std::string const& name, Container<Task*>& container, Task* const task ): Named( name ), container_( container ), task_( task ){}

	void run() override{
		if ( isCompleted ) throw "The task has already been completed";
		container_.insert_back( task_ );
		isCompleted = true;
	}

	std::string toString() const override{
		return name_ + " : " + std::format( "Add \"{}\" to container ({})",
											task_->toString(), isCompleted ? "DONE" : "IN PROCESS" );
	}
};

class CountObjectTask: public TaskWithResult<int>, public Named{
	Container<Object*>& container_;
public:
	CountObjectTask( std::string const& name, Container<Object*>& container ): Named( name ), container_( container ){}

	void run() override{
		if ( isCompleted ) throw "The task has already been completed";
		result_ = container_.size();
	}

	std::string toString() const override{
		return name_ + " : " + std::format( "Container сontain {} objects ({})",
											isCompleted ? std::to_string( result_ ) : "?", isCompleted ? "DONE" : "IN PROCESS" );
	}
};

class CountTaskTask: public TaskWithResult<int>, public Named{
	Container<Task*>& container_;
public:
	CountTaskTask( std::string const& name, Container<Task*>& container ): Named( name ), container_( container ){}

	void run() override{
		for ( auto& task : container_ ) { if ( task->hasResult() ) result_++; }
		isCompleted = true;
	}

	std::string toString() const override{
		return name_ + " : " + std::format( "Container сontain {} result task ({})",
											isCompleted ? std::to_string( result_ ) : "?", isCompleted ? "DONE" : "IN PROCESS" );
	}
};

class ClearTask: public Task{
	Container<Object*>& container;

	ClearTask( Container<Object*>& container ): container( container ){}

	void run() override{ container.clear(); }
};

class GetNumberInstancesTask: public TaskWithResult<int>, public Named{
public:

	GetNumberInstancesTask( std::string const& name ): Named( name ){}

	void run()override{
		result_ = Object::numberInstances;
		isCompleted = true;
	}

	std::string toString() const override{
		return name_ + " : " + std::format( "Now there are {} objects ({})",
											isCompleted ? std::to_string( result_ ) : "?", isCompleted ? "DONE" : "IN PROCESS" );
	}
};