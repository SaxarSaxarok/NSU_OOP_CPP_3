#pragma once
#include <utility>
#include<iterator>
#include <type_traits>
#include <cassert>
template<typename T>
class Container{
private:
	struct Node{
		T data_ {};
		Node* next_;
		Node* prev_;

		Node(): next_( this ), prev_( this ){}
		Node( T const& data ): data_( data ), next_( this ), prev_( this ){}
		~Node(){ unlink(); }

		void insert_back( Node* node ){
			node->next_ = this;
			node->prev_ = prev_;
			prev_->next_ = node;
			prev_ = node;
		}

		void insert_front( Node* node ){
			node->prev_ = this;
			node->next_ = next_;
			next_->prev_ = node;
			next_ = node;
		}

		void unlink(){
			Node* next = next_;
			Node* prev = prev_;
			next->prev_ = prev;
			prev->next_ = next;
			next_ = this;
			prev_ = this;
		}

		void swap( Node& other ){
			std::swap( next_->prev_, other.next_->prev_ );
			std::swap( prev_->next_, other.prev_->next_ );
			std::swap( next_, other.next_ );
			std::swap( prev_, other.prev_ );
		}

	};

	template<typename U>
	class Iterator{
		using node_ptr_type = std::conditional<std::is_const_v<U>, Node const*, Node*>::type;
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = ptrdiff_t;
		using value_type = T;
		using pointer = U*;
		using reference = U&;

		node_ptr_type node_;
		Iterator( node_ptr_type node ): node_( node ){}

	public:
		Iterator& operator++(){
			node_ = node_->next_;
			return *this;
		}
		Iterator& operator--(){
			node_ = node_->prev_;
			return *this;
		}

		Iterator operator++( int ){
			Iterator tmp = *this;
			++( *this );
			return tmp;
		}
		Iterator operator--( int ){
			Iterator tmp = *this;
			--( *this );
			return tmp;
		}

		bool operator==( Iterator other ) const{ return node_ == other.node_; };
		bool operator!=( Iterator other ) const{ return node_ != other.node_; };

		reference operator*() const{ return node_->data_; }
		pointer operator->() const{ return &( node_->data_ ); }
		operator Iterator<U const>() const{ return Iterator<U const>( node_ ); }
	};

public:
	using iterator = Iterator<T>;
	using const_iterator = Iterator<T const>;

	Container() = default;
	Container( std::initializer_list<T> init ){ for ( auto const& i : init ) { insert_back( i ); } }
	~Container(){ clear(); }

	iterator begin(){ return iterator( fictiveNode_.next_ ); }
	iterator end(){ return iterator( &fictiveNode_ ); }

	const_iterator cbegin() const{ return const_iterator( fictiveNode_.next_ ); }
	const_iterator cend() const{ return const_iterator( &fictiveNode_ ); }

	void insert_back( T const& data ){ fictiveNode_.insert_back( new Node( data ) ); ++size_; }
	void insert_front( T const& data ){ fictiveNode_.insert_front( new Node( data ) ); ++size_; }

	void erase( const_iterator it ){
		if ( isEmpty() ) throw std::out_of_range( "Container is empty" );
		delete it.node_;
		--size_;
	}
	void pop_back(){ erase( --end() ); }
	void pop_front(){ erase( begin() ); }

	T& front(){
		if ( isEmpty() )throw std::out_of_range( "Container is empty" );
		return *( begin() );
	}

	T& back(){
		if ( isEmpty() )throw std::out_of_range( "Container is empty" );
		return *( --end() );
	}

	T const& front() const{
		if ( isEmpty() )throw std::out_of_range( "Container is empty" );
		return *( cbegin() );
	}

	T const& back() const{
		if ( isEmpty() )throw std::out_of_range( "Container is empty" );
		return *( --cend() );
	}

	size_t size() const{ return size_; };
	bool isEmpty() const{ return size_ == 0; };

	void swap( Container& other ){
		fictiveNode_.swap( other.fictiveNode_ );
		std::swap( size_, other.size_ );
	}

	void reverse(){
		if ( isEmpty() ) return;
		size_t mid = size() / 2; //I decided to do this in the end
		auto left = begin();
		auto right = --end();
		for ( size_t i = 0; i <= mid; i++ )
		{
			auto leftNode = left.node_;
			auto rightNode = right.node_;
			++left;
			--right;
			leftNode->swap( *rightNode );
		}
	}

	void clear(){ while ( !isEmpty() ) erase( begin() ); }

private:
	Node fictiveNode_; //For iterator::end() and handling special cases (empty, front=back, etc...)
	size_t size_ = 0;
};