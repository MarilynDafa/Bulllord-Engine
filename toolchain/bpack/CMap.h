#pragma once

template <typename KeyType, typename ValueType>
class c_map
{
private:
	template <typename KeyTypeRB, typename ValueTypeRB>
	class rb_tree
	{
	private:
		rb_tree* m_left_child;
		rb_tree* m_right_child;
		rb_tree* m_parent;
		KeyTypeRB m_key;
		ValueTypeRB	m_value;
		bool m_red;
	public:
		rb_tree(const KeyTypeRB& k_, const ValueTypeRB& v_):m_left_child(0), m_right_child(0), m_parent(0), m_key(k_), m_value(v_), m_red(true) {}
		void set_left_child(rb_tree* p_){m_left_child=p_;	if(p_)p_->set_parent(this);}
		void set_right_child(rb_tree* p_){m_right_child=p_;	if(p_)p_->set_parent(this);}
		void set_parent(rb_tree* p_){m_parent=p_;}
		void set_value(const ValueTypeRB& v_){m_value = v_;}
		void set_red(){m_red = true;}
		void set_black(){m_red = false;}
		rb_tree* get_left_child(){return m_left_child;}
		rb_tree* get_right_child(){return m_right_child;}
		rb_tree* get_parent(){return m_parent;}
		const rb_tree* get_left_child() const{return m_left_child;}
		const rb_tree* get_right_child() const	{return m_right_child;}
		const rb_tree* get_parent() const{return m_parent;}
		const ValueTypeRB& second() const{return m_value;}
		ValueTypeRB& second(){return m_value;}
		const KeyTypeRB& first() const{return m_key;}
		KeyTypeRB& first(){return m_key;}
		bool is_root() const{return m_parent==0;}
		bool is_left_child() const{return (m_parent != 0) && (m_parent->get_left_child()==this);}
		bool is_right_child() const{return (m_parent!=0) && (m_parent->get_right_child()==this);}
		bool is_leaf() const{return (m_left_child==0) && (m_right_child==0);}
		unsigned int get_level() const{if (is_root()) return 1; else return get_parent()->get_level() + 1;}
		bool is_red() const{return m_red;}
		bool is_black() const{return !m_red;}
	}; 
	typedef rb_tree<KeyType,ValueType> node;
public:
	class iterator
	{
	private:
		node* m_root;
		node* m_current;
	public:
		iterator() : m_root(0), m_current(0) {}
		iterator(node* root) : m_root(root){reset();}
		iterator(const iterator& src_) : m_root(src_.m_root), m_current(src_.m_current) {}
		iterator& operator=(const iterator& src_){m_root = src_.m_root;m_current = src_.m_current;return (*this);}
		bool operator==(const iterator& oth_){return m_root == oth_.m_root&&m_current == oth_.m_current;}
		bool operator!=(const iterator& oth_){return !(*this==oth_);}		  
		void operator++(int){inc();}
		void operator++(){inc();}
		node* operator->(){return get_node();}
		node& operator*(){return *m_current;}
		void reset(){m_current = get_min(m_root);}
		node* get_node(){return m_current;}
		void set_cur(const node* n_){m_current = const_cast<node*>(n_);}
	private:
		node* get_min(node* n_)
		{
			while(n_!=0 && (n_->get_left_child()!=0 || n_->get_right_child()!=0))
			{
				if (n_->get_left_child()) n_ = n_->get_left_child();
				else n_ = n_->get_right_child();				
			}
			return n_;
		}
		void inc()
		{
			if (m_current==0)
				return;
			if (m_current->is_left_child() && m_current->get_parent()->get_right_child())
				m_current = get_min(m_current->get_parent()->get_right_child());
			else
				m_current = m_current->get_parent();
		}
	};
	class const_iterator
	{
	private:
		node* m_root;
		node* m_current;
	public:
		const_iterator() : m_root(0), m_current(0) {}
		const_iterator(node* root_) : m_root(root_){reset();}
		const_iterator(const const_iterator & src_) : m_root(src_.m_root), m_current(src_.m_current) {}
		const_iterator& operator=(const const_iterator& src_){m_root = src_.m_root;m_current = src_.m_current;return (*this);}
		bool operator==(const const_iterator& oth_){return m_root == oth_.m_root&&m_current == oth_.m_current;}
		bool operator!=(const const_iterator& oth_){return !(*this==oth_);}
		void operator++(int){inc();}
		void operator++(){inc();}
		const node* operator->()const{return get_node();}
		const node& operator*() const{return *m_current;}
		void reset(){m_current = get_min(m_root);}
		const node* get_node() const{return m_current;}
		void set_cur(const node* n_){m_current = const_cast<node*>(n_);}
	private:
		node* get_min(node* n_)
		{
			while(n_!=0 && (n_->get_left_child()!=0 || n_->get_right_child()!=0))
			{
				if (n_->get_left_child())	n_ = n_->get_left_child();
				else	n_ = n_->get_right_child();
			}
			return n_;
		}
		void inc()
		{
			if (m_current==0)
				return;
			if (m_current->is_left_child() && m_current->get_parent()->get_right_child())
				m_current = get_min(m_current->get_parent()->get_right_child());
			else
				m_current = m_current->get_parent();
		}
	};		
private:
	node* m_root;
	unsigned int m_size;
public:
	c_map();
	c_map(const c_map<KeyType , ValueType>& src_);
	~c_map();
public:
	c_map<KeyType , ValueType>& operator=(const c_map<KeyType , ValueType>& src_);
	ValueType& operator[](const KeyType& k_);
	const ValueType& operator[](const KeyType& k_) const;
public:
	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
	bool empty() const;
	unsigned int size() const;
	void clear();
	void insert(const KeyType& keynew_, const ValueType& v_);
	void erase(iterator it_);
	void erase(const_iterator it_);
	iterator find(const KeyType& key2find_);
	const_iterator find(const KeyType& key2find_) const;
private:
	node* _delink(const KeyType& k_);
	void _set_root(node* newroot_);
	node* _find_node(const KeyType& key2find_) const;
	iterator _insert_node(const KeyType& keynew_, const ValueType& v_);
	bool _insert_node(node* newnode_);
	void _rotate_left(node* p_);
	void _rotate_right(node* p_);
};
/////////////////////////////////////////////////////////////////////	
template <typename KeyType, typename ValueType>
c_map<KeyType , ValueType>::c_map() : m_root(nullptr), m_size(0) {}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
c_map<KeyType , ValueType>::c_map(const c_map<KeyType , ValueType>& src_): m_root(nullptr), m_size(0)
{
	*this = src_;
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
c_map<KeyType , ValueType>::~c_map()
{
	clear();
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
c_map<KeyType , ValueType>& c_map<KeyType , ValueType>::operator=(const c_map<KeyType , ValueType>& src_)
{
	clear();
	for(const_iterator iter = src_.begin() ; iter!= src_.end() ; ++iter)
		insert(iter->first() , iter->second());
	return *this;
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
ValueType& c_map<KeyType , ValueType>::operator[](const KeyType& k_)
{
	node* p = _find_node(k_);
	if (p)
		return p->second();
	else
	{
		ValueType val;
		node* newnode = new node(k_ , val);
		_insert_node(newnode);
		return newnode->second();
	}
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
const ValueType& c_map<KeyType , ValueType>::operator[](const KeyType& k_) const
{
	return find(k_)->second();
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
void c_map<KeyType , ValueType>::clear()
{
	iterator i(m_root);
	while(i.get_node()!=0)
	{
		node* p = i.get_node();
		++i;
		delete p;
		p =0;
	}
	m_root = 0;
	m_size= 0;
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
unsigned int c_map<KeyType , ValueType>::size() const
{
	return m_size;
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
bool  c_map<KeyType , ValueType>::empty() const
{
	return m_root == 0;
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
typename c_map<KeyType , ValueType>::iterator c_map<KeyType , ValueType>::begin()
{
	return iterator(m_root);
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
typename c_map<KeyType , ValueType>::const_iterator c_map<KeyType , ValueType>::begin() const
{
	return const_iterator(m_root);
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
typename c_map<KeyType , ValueType>::iterator c_map<KeyType , ValueType>::end()
{
	iterator ret(m_root);
	ret.set_cur(0);
	return ret;
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
typename c_map<KeyType , ValueType>::const_iterator c_map<KeyType , ValueType>::end() const
{
	const_iterator ret(m_root);
	ret.set_cur(0);
	return ret;
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
void c_map<KeyType , ValueType>::insert(const KeyType& keynew_, const ValueType& v_)
{
	node* p = _find_node(keynew_);
	if(p)
		p->set_value(v_);
	else
		_insert_node(keynew_,v_);
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
void c_map<KeyType , ValueType>::erase(iterator p_)
{
	if (p_ == end())
		return;
	while(p_->get_right_child())
		_rotate_left(p_.get_node());
	node* left = p_->get_left_child();
	if (p_->is_left_child())
		p_->get_parent()->set_left_child(left);
	else if (p_->is_right_child())
		p_->get_parent()->set_right_child(left);
	else
		_set_root(left);
	delete p_.get_node();
	p_.set_cur(0);
	--m_size;
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
void c_map<KeyType , ValueType>::erase(const_iterator p_)
{
	if (p_ == end())
		return;
	while(p_->get_right_child())
		_rotate_left(p_.get_node());
	node* left = p_->get_left_child();
	if (p_->is_left_child())
		p_->get_parent()->set_left_child(left);
	else if (p_->is_right_child())
		p_->get_parent()->set_right_child(left);
	else
		_set_root(left);
	delete p_.get_node();
	p_.get_node() = 0;
	--m_size;
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
typename c_map<KeyType , ValueType>::iterator c_map<KeyType , ValueType>::find(const KeyType& key2find_)
{
	iterator ptr(m_root);
	ptr.set_cur(_find_node(key2find_) );
	return ptr;
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
typename c_map<KeyType , ValueType>::const_iterator c_map<KeyType , ValueType>::find(const KeyType& key2find_) const
{
	const_iterator ptr(m_root);
	ptr.set_cur(_find_node(key2find_) );
	return ptr;
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
typename c_map<KeyType , ValueType>::node* c_map<KeyType , ValueType>::_delink(const KeyType& k_)
{
	node* p = find(k_);
	if (p == 0)
		return 0;
	while(p->get_right_child())
		_rotate_left(p);
	node* left = p->get_left_child();
	if (p->is_left_child())
		p->get_parent()->set_left_child(left);
	else if (p->is_right_child())
		p->get_parent()->set_right_child(left);
	else
		_set_root(left);
	--m_size;
	return p;
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
void c_map<KeyType , ValueType>::_set_root(node* newroot_)
{
	m_root = newroot_;
	if (m_root != 0)
	{
		m_root->set_parent(0);
		m_root->set_black();
	}
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
typename c_map<KeyType , ValueType>::node* c_map<KeyType , ValueType>::_find_node(const KeyType& key2find_) const
{
	node* nodeptr = m_root;
	while(nodeptr!=0)
	{
		KeyType key(nodeptr->first());
		if (key2find_ == key)
			return nodeptr;
		else if (key2find_ < key)
			nodeptr = nodeptr->get_left_child();
		else
			nodeptr = nodeptr->get_right_child();
	}
	return nullptr;
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
typename c_map<KeyType , ValueType>::iterator c_map<KeyType , ValueType>::_insert_node(const KeyType& keynew_, const ValueType& v_)
{
	node* newnode = new node(keynew_ , v_);
	if (!_insert_node(newnode))
	{
		delete newnode;
		newnode = nullptr;
		return end();
	}
	while (!newnode->is_root() && (newnode->get_parent()->is_red()))
	{
		if (newnode->get_parent()->is_left_child())
		{
			node* newNodesUncle = newnode->get_parent()->get_parent()->get_right_child();
			if ( newNodesUncle!=0 && newNodesUncle->is_red())
			{
				newnode->get_parent()->set_black();
				newNodesUncle->set_black();
				newnode->get_parent()->get_parent()->set_red();
				newnode = newnode->get_parent()->get_parent();
			}
			else
			{
				if ( newnode->is_right_child())
				{
					newnode = newnode->get_parent();
					_rotate_left(newnode);
				}
				newnode->get_parent()->set_black();
				newnode->get_parent()->get_parent()->set_red();
				_rotate_right(newnode->get_parent()->get_parent());
			}
		}
		else
		{
			node* newNodesUncle = newnode->get_parent()->get_parent()->get_left_child();
			if ( newNodesUncle!=0 && newNodesUncle->is_red())
			{
				newnode->get_parent()->set_black();
				newNodesUncle->set_black();
				newnode->get_parent()->get_parent()->set_red();
				newnode = newnode->get_parent()->get_parent();
			}
			else
			{
				if (newnode->is_left_child())
				{
					newnode = newnode->get_parent();
					_rotate_right(newnode);
				}
				newnode->get_parent()->set_black();
				newnode->get_parent()->get_parent()->set_red();
				_rotate_left(newnode->get_parent()->get_parent());
			}
		}
	}
	m_root->set_black();
	iterator ret(m_root);
	ret.set_cur(newnode);
	return ret;
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
bool c_map<KeyType , ValueType>::_insert_node(node* newnode_)
{
	bool result = true;
	if (m_root==nullptr)
	{
		_set_root(newnode_);
		m_size = 1;
	}
	else
	{
		node* nodeptr = m_root;
		KeyType newkey = newnode_->first();
		while (nodeptr)
		{
			KeyType key(nodeptr->first());
			if (newkey == key)
			{
				result = false;
				nodeptr = 0;
			}
			else if (newkey < key)
			{
				if (nodeptr->get_left_child() == nullptr)
				{
					nodeptr->set_left_child(newnode_);
					nodeptr = nullptr;
				}
				else
					nodeptr = nodeptr->get_left_child();
			}
			else
			{
				if (nodeptr->get_right_child()==nullptr)
				{
					nodeptr->set_right_child(newnode_);
					nodeptr = 0;
				}
				else
					nodeptr = nodeptr->get_right_child();
			}
		}
		if (result)
			++m_size;
	}
	return result;
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
void c_map<KeyType , ValueType>::_rotate_left(node* p_)
{
	node* right = p_->get_right_child();
	p_->set_right_child(right->get_left_child());
	if (p_->is_left_child())
		p_->get_parent()->set_left_child(right);
	else if (p_->is_right_child())
		p_->get_parent()->set_right_child(right);
	else
		_set_root(right);
	right->set_left_child(p_);
}
//--------------------------------------------------------
template <typename KeyType, typename ValueType>
void c_map<KeyType , ValueType>::_rotate_right(node* p_)
{
	node* left = p_->get_left_child();
	p_->set_left_child(left->get_right_child());
	if (p_->is_left_child())
		p_->get_parent()->set_left_child(left);
	else if (p_->is_right_child())
		p_->get_parent()->set_right_child(left);
	else
		_set_root(left);
	left->set_right_child(p_);
}